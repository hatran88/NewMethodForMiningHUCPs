
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <ctime>
#include "boost/dynamic_bitset.hpp"
#include <unordered_set>
#include <numeric>
#include <stdlib.h>     /* using abs */


#include "Object.h"
#include "ReaderCSV.h"
#include "PrintFunctions.h"
#include "SetNumberPointFloat.h"
#include "Patterns.h"



using namespace std;


// Definition global variables
clock_t timeConstCoLHashmap;
double totalTimeConstCoLHashmap;

// Save co-location hash map
std::unordered_map<std::string, std::unordered_map<char, std::unordered_set<ObjWithoutCoordinate, myHashFunc>>> CoLHM;
std::unordered_map<std::string, std::unordered_map<char, std::unordered_set <ObjWithoutCoordinate, myHashFunc>>>::iterator itCoLHM;

std::unordered_map<char, float> uf; // utility of each feature
float US = 0.0f; // utility of the dataset
std::unordered_map<char, float> totNumInsFeat; // Save the minimum prevalence of each feature, this value = alpha x total number instancces of each feature
std::unordered_map<ObjWithoutCoordinate, std::vector<ObjWithoutCoordinate>, myHashFunc> SN;
std::unordered_map<ObjWithoutCoordinate, std::vector<ObjWithoutCoordinate>, myHashFunc>::iterator itSN;

std::unordered_map<std::string, float> checkedCands; // save all non PCPs

std::vector<ObjWithoutCoordinate> Q; // a global variable representing a clique



/**
* @brief This function make a grid on an input dataset
* @param dataList: an input dataset; dist_thres: a distance threshold
* @retval a hash map that stores cell id with instances fall in it.
*/
std::map<std::pair<int, int>, std::vector<Objects>> makeGrid(
	std::vector<std::vector<std::string>> & dataList, float dist_thres)
{
	std::map<std::pair<int, int>, std::vector<Objects>> grid;

	int cell_x, cell_y;
	pair<int, int> cell_key;

	std::vector<Objects> value;

	for (auto const& vec : dataList)
	{
		// calc the cell id
		cell_x = ceil(std::stof(vec[2]) / dist_thres); // x coordinate
		cell_y = ceil(std::stof(vec[3]) / dist_thres); // y coordinate		
		cell_key = make_pair(cell_x, cell_y);

		Objects instance = { vec[0][0], std::stoi(vec[1]), std::stof(vec[4]), std::stof(vec[2]), std::stof(vec[3])};
		value.insert(value.end(), instance);

		// check if this key is exsiting?
		if (grid.empty()) //The grid is empty
		{
			grid[cell_key] = value;
		}
		else if (grid.find(cell_key) == grid.end()) // if the key is not exist
		{
			grid[cell_key] = value;
		}
		else // the key has already existed
		{
			grid.find(cell_key)->second.push_back(instance);
		}
		// Clear
		value.clear();
	}

	return grid;
}


/**
* @bref This function calculates the distances of instances in the current block.
* @param alll instance in the current block
* @retval A vector that save the distance of instances.
*/
float calculateDistanceTwoInstances(Objects currentInst, Objects checkInst)
{
	return sqrt((currentInst.x - checkInst.x)*(currentInst.x - checkInst.x)
		+ (currentInst.y - checkInst.y)*(currentInst.y - checkInst.y));
}



/**
* @brief: This function generates star neighborhoods of instances.
* @param: grid: a grid posing of the input dataset
*         dist_thres: a distance threshold
* @retval: SN: a hash map that stores as <instance, <<neighbors>,<neighbors>>. This struture is different with star neighbors in Join-less
*/
void genStarNeighborhoods(
	std::vector<ObjWithoutCoordinate>& S,
	std::map<std::pair<int, int>, std::vector<Objects>>& grid,
	float dist_thres)
{
	std::unordered_map<ObjWithoutCoordinate, std::unordered_set<ObjWithoutCoordinate, myHashFunc>, myHashFunc> tempNei;
	std::unordered_map<char, float>::iterator ittotNumInsFeat;
	std::unordered_map<char, float>::iterator ituf;

	int i, j; // the index of cells in x and y
	std::vector<std::pair<int, int>> fiveCells;
	float dist;
	std::vector<Objects> fiveCellInst; // save all instance in the five cells and two neighboring instances.	
	// For get index of elements in vectors
	std::vector<Objects>::iterator itvert;
	unsigned int indexCurrentInst, indexCheckInst;

	// Loop each cell in grid and computation neighbors in five cells
	for (std::map<std::pair<int, int>, std::vector<Objects>>::iterator it = grid.begin(); it != grid.end(); ++it)
	{
		// Create the five cells
		i = it->first.first;
		j = it->first.second;

		fiveCells.push_back(it->first);
		fiveCells.push_back(std::make_pair(i, j + 1));
		fiveCells.push_back(std::make_pair(i + 1, j + 1));
		fiveCells.push_back(std::make_pair(i + 1, j));
		fiveCells.push_back(std::make_pair(i + 1, j - 1));

		// Get all instances in the current cell and five cells	
		for (auto const& cell : fiveCells)
		{
			if (grid.count(cell))
			{
				fiveCellInst.insert(fiveCellInst.end(), grid.find(cell)->second.begin(), grid.find(cell)->second.end());
			}
		}

		// Sort all instances in the five cells
		std::sort(fiveCellInst.begin(), fiveCellInst.end());

		// Iterator each instance in the currentCellInst and check its neighbors		
		for (Objects const& currentInst : it->second)
		{
			ObjWithoutCoordinate currentInstNoCoor{ currentInst.feature, currentInst.instance, currentInst.utility };
			
			// Save all instances
			S.push_back(currentInstNoCoor);			
			// Get number instances of features
			ittotNumInsFeat = totNumInsFeat.find(currentInst.feature);
			if (ittotNumInsFeat != totNumInsFeat.end())
			{
				ittotNumInsFeat->second = ittotNumInsFeat->second + 1.0f;
			}
			else
			{
				totNumInsFeat.insert({ currentInst.feature, 1.0f });
			}
			// Get utility
			ituf = uf.find(currentInst.feature);
			if (ituf != uf.end())
			{
				ituf->second = ituf->second + currentInst.utility;
			}
			else
			{
				uf.insert({ currentInst.feature, currentInst.utility });
			}
			// Put thi point in the neighbor first
			//indexCurrentInst = lower_bound(vertices.begin(), vertices.end(), currentInst) - vertices.begin();
			if (tempNei.find(currentInstNoCoor) == tempNei.end())
			{
				tempNei.insert({ currentInstNoCoor, std::unordered_set<ObjWithoutCoordinate, myHashFunc> {} });
			}

			for (Objects const& checkInst : fiveCellInst) // check with each instance in the five cells
			{
				if (currentInst.feature != checkInst.feature) // only check two instances belong to different features.
				{
					dist = calculateDistanceTwoInstances(currentInst, checkInst);

					if (dist <= dist_thres) // the two instances have neighbor relationship
					{
						// Find index of instances						
						//indexCheckInst = lower_bound(vertices.begin(), vertices.end(), checkInst) - vertices.begin();
						ObjWithoutCoordinate checkInstNoCoor{ checkInst.feature, checkInst.instance, checkInst.utility };

						// Put the current instance into neighbors
						if (tempNei.find(currentInstNoCoor) != tempNei.end()) // this instance has already existed
						{
							// update value							
							tempNei.find(currentInstNoCoor)->second.insert(checkInstNoCoor);
						}
						else // This feature has not existed in SN, directly put into SN	
						{
							// Put into SN							
							tempNei.insert({ currentInstNoCoor, std::unordered_set<ObjWithoutCoordinate, myHashFunc>{checkInstNoCoor} });
						}

						// Put the current instance into neighbors
						if (tempNei.find(checkInstNoCoor) != tempNei.end()) // this instance has already existed
						{
							// Update value							
							tempNei.find(checkInstNoCoor)->second.insert(currentInstNoCoor);
						}
						else // This feature has not existed in SN, directly put into SN	
						{
							tempNei.insert({ checkInstNoCoor, std::unordered_set<ObjWithoutCoordinate, myHashFunc>{currentInstNoCoor} });
						}
					}
				}
			}
		}
		// clear all element for the next iterator
		fiveCells.clear();
		fiveCellInst.clear();
	}

	// Convert to neighbors (using vector)
	std::unordered_map<ObjWithoutCoordinate, std::unordered_set<ObjWithoutCoordinate, myHashFunc>, myHashFunc>::iterator itN = tempNei.begin();
	while (itN != tempNei.end())
	{
		std::vector<ObjWithoutCoordinate> value(itN->second.begin(), itN->second.end());
		std::sort(value.begin(), value.end());

		SN.insert({ itN->first, value });

		itN = tempNei.erase(itN);
	}
}



/**
* @brief: This function sorts vertices by their degeneracy.
* @param: vertices: a set of indexes of all vertices
*         neighbors: star neighbors of all vertices
* @retval: a vector of oredered vertices sorted by degeneracy
*/
void OrderedDegeneracy(std::vector<ObjWithoutCoordinate>& orderDeg)
{
	// Store vertices and their neighbor number in to a map
	std::map<ObjWithoutCoordinate, int> vertDeg;

	itSN = SN.begin();
	while (itSN != SN.end())
	{
		vertDeg.insert({ itSN->first, itSN->second.size() });
		++itSN;
	}

	std::map<ObjWithoutCoordinate, int>::iterator itVertDeg;
	// Loop each vertices and calc deg
	while (!vertDeg.empty())
	{
		// Assum the first item is the minimum
		ObjWithoutCoordinate minV{ vertDeg.begin()->first };
		int minDeg = vertDeg.begin()->second;

		// Loop to find the real minimum
		itVertDeg = vertDeg.begin();
		// start index number 1
		++itVertDeg;

		while (itVertDeg != vertDeg.end())
		{
			if (itVertDeg->second < minDeg)
			{
				minV = itVertDeg->first;
				minDeg = itVertDeg->second;
			}
			++itVertDeg;
		}
		//Put the vertex to the result
		orderDeg.push_back(minV);
		// Delete this minV from vertDeg
		vertDeg.erase(minV);
		// Minus one deg form vertices which are neighbor of minV
		for (auto const& item : SN.find(minV)->second)
		{
			// get value of item and minus 1
			itVertDeg = vertDeg.find(item);
			if (itVertDeg != vertDeg.end())
			{
				itVertDeg->second--;
			}
		}
	}
}



/**
* @brief: This function get maximal cliques
* @param: vertices: a set of indexes of all vertices
*         neighbors: star neighbors of all vertices
* @retval: a vector of oredered vertices sorted by degeneracy
*/
void BronKerboschDeg(std::vector<ObjWithoutCoordinate> P,
	std::vector<ObjWithoutCoordinate> X,
	std::vector<ObjWithoutCoordinate>& orderDeg)
{
	for (auto const& v : orderDeg)
	{
		std::vector<ObjWithoutCoordinate> newP, newX, newR;

		newR.push_back(v);

		std::set_intersection(P.begin(), P.end(),
			SN.find(v)->second.begin(), SN.find(v)->second.end(),
			std::back_inserter(newP));

		std::set_intersection(X.begin(), X.end(),
			SN.find(v)->second.begin(), SN.find(v)->second.end(),
			std::back_inserter(newX));

		BronKerboschPivot(newP, newR, newX);

		// Delete
		P.erase(std::remove(P.begin(), P.end(), v), P.end()); // check this xxxx
		//std::sort(P.begin(), P.end());
		// and add v to X
		X.push_back(v);
		std::sort(X.begin(), X.end());
	}
}



/**
* @brief: This function enumerates all maximal cliques based on bron-kerbosch pivot.
* @param: vertices: a set of all vertices (instances)
*         neighbors: a hashmap stores neighbors of each instance
* @retval: allMaxCliques: all maximal cliques.
*/
void BronKerboschPivot(std::vector<ObjWithoutCoordinate> P, std::vector<ObjWithoutCoordinate> R, std::vector<ObjWithoutCoordinate> X)
{
	if (P.empty() && X.empty())
	{
		if (R.size() > 1)
		{
			/*std::cout << "get one : ";
			for (auto const& inst : R)
			{
				std::cout << inst.feature << "." << inst.instance << ", ";
			}
			std::cout << endl;*/
			// Get one maximal cliques and construct hashmap
			timeConstCoLHashmap = clock();
			constructCoLoHashmap(R);
			totalTimeConstCoLHashmap = totalTimeConstCoLHashmap + double(clock() - timeConstCoLHashmap);
		}
	}
	else
	{
		// Find the pivot		
		std::vector<ObjWithoutCoordinate> unionPX;
		std::set_union(P.begin(), P.end(), X.begin(), X.end(), std::back_inserter(unionPX));

		// Find maximum
		std::vector<ObjWithoutCoordinate> intersecPNu;
		std::vector<ObjWithoutCoordinate> maxu{ unionPX[0] };
		int maxnei = SN.find(unionPX[0])->second.size();

		for (size_t j = 1; j < unionPX.size(); ++j)
		{
			std::set_intersection(P.begin(), P.end(),
				SN.find(unionPX[j])->second.begin(), SN.find(unionPX[j])->second.end(),
				std::back_inserter(intersecPNu));

			if (intersecPNu.size() > maxnei)
			{
				maxnei = intersecPNu.size();
				maxu.clear();
				maxu.push_back(unionPX[j]);
			}
		}

		std::vector<ObjWithoutCoordinate> deffPNu;
		std::set_difference(P.begin(), P.end(), SN.find(maxu[0])->second.begin(), SN.find(maxu[0])->second.end(),
			std::back_inserter(deffPNu));

		// Loop and recursion
		int psize = deffPNu.size();
		for (size_t i = 0; i < psize; ++i)
		{
			ObjWithoutCoordinate v = deffPNu[i];
			std::vector<ObjWithoutCoordinate> newP, newX;

			std::set_intersection(P.begin(), P.end(),
				SN.find(v)->second.begin(), SN.find(v)->second.end(),
				back_inserter(newP));
			std::sort(newP.begin(), newP.end());

			std::set_intersection(X.begin(), X.end(),
				SN.find(v)->second.begin(), SN.find(v)->second.end(),
				back_inserter(newX));
			std::sort(newX.begin(), newX.end());

			std::vector<ObjWithoutCoordinate> newR = R;
			newR.push_back(v);
			std::sort(newR.begin(), newR.end());

			BronKerboschPivot(newP, newR, newX);

			// Delete v from P
			P.erase(std::remove(P.begin(), P.end(), v), P.end());
			// and add v to X
			X.push_back(v);
			std::sort(X.begin(), X.end());
		}
	}
}



/**
* @brief: This function computes the utility of the dataset and utility of each feature
* @param: minf, alphas:
* @retval: minf
*/
void computeUSandUtiEachFeature()
{
	// compute utility of each feature
	// the utility of each feature is the average utility of its
	float minuf = 0.0f, maxuf = 0.0f;
	std::unordered_map<char, float>::iterator ittotal;
	std::unordered_map<char, float>::iterator ituf = uf.begin();
	while (ituf != uf.end())
	{
		// get utility of the dataset
		US += ituf->second;
		// compute utility of each feature 
		ittotal = totNumInsFeat.find(ituf->first);
		if (ittotal != totNumInsFeat.end())
		{
			// compute utility of each feature
			ituf->second = ituf->second / (float)ittotal->second;			
		}
		// next
		++ituf;
	}
	// Normalization
	minuf = uf.begin()->second;
	maxuf = uf.begin()->second;

	ituf = uf.begin();
	while (ituf != uf.end())
	{
		if (ituf->second > maxuf)
		{
			maxuf = ituf->second;
		}
		if (ituf->second < minuf)
		{
			minuf = ituf->second;
		}

		++ituf;
	}
	ituf = uf.begin();
	while (ituf != uf.end())
	{
		ituf->second = ituf->second/maxuf;
		++ituf;
	}
	//std::cout << "maxuf:" << maxuf << endl;
	//std::cout << "minuf:" << minuf << endl;
}


/**
* @brief: This function buids the co-location hashmap structure.
* @param: allMaxCliques: all maximal cliques.
* @retval: CoLoHM: a co-location hashmap.
*/
void constructCoLoHashmap(std::vector<ObjWithoutCoordinate> R)
{	
	std::sort(R.begin(),R.end());
	// Temporary varibales
	std::string pattern;

	// Loop each instance in the maximal clique to build key
	for (auto const& inst : R)
	{
		pattern += inst.feature;			
	}
	// Check this key has not already in CoLHM
	itCoLHM = CoLHM.find(pattern);
	if (itCoLHM != CoLHM.end()) // This key has existed, update old values
	{
		// update other instances in the clique
		for (auto const& inst : R)
		{			
			itCoLHM->second.find(inst.feature)->second.insert(inst);
		}
	}
	else // not exist
	{
		// build value and add new item
		std::unordered_map<char, std::unordered_set<ObjWithoutCoordinate, myHashFunc>> outValue;
		for (auto const& inst : R) {

			outValue.insert({ inst.feature, std::unordered_set<ObjWithoutCoordinate, myHashFunc> {inst} });
		}
		CoLHM.insert({ pattern, outValue });
	}
}




/*
*@brief: This function generate all combination of a char vector
*@param: c: a vector of charl; combo: the size of combo; C(n, m) = n!/(m!(n-m)!)
*@retval: a vector of sub vectors of the vector
*/
template<typename T>
std::string getCombination(const T& c, int combo)
{
	std::string result;
	int n = c.size();
	for (int i = 0; i < n; ++i) {
		if ((combo >> i) & 1)
			result.push_back(c[i]);
	}
	return result;
}


template<typename T>
std::vector<std::string> combo(const T& c, int k)
{
	std::vector<std::string> combination;

	int n = c.size();
	int combo = (1 << k) - 1;       // k bit sets

	while (combo < 1 << n)
	{
		combination.insert(combination.end(), getCombination(c, combo));
		int x = combo & -combo;
		int y = combo + x;
		int z = (combo & ~y);
		combo = z / x;
		combo >>= 1;
		combo |= y;
	}

	return combination;
}



/**
* @brief: This function filters prevalent patterns
* @param: CoLHM: the co-location hash map.
*         pre_thress: a minimum prevalent threshold.
* @retval: allPrevPats: a co-location hashmap.
*/
int findMaxSizeCand(std::map<std::string, int>& candPatts)
{
	std::map<std::string, int>::iterator it = candPatts.begin();
	int t = 1;
	while (it != candPatts.end())
	{
		if (it->first.size() > t)
		{
			t = it->first.size();
		}

		++it;
	}
	return t;
}


/**
* @brief: This function compares the size of two patterns
* @param: cand1 and cand2: two candidates
* @retval: bool
*/
struct compareBySize
{
	bool operator()(std::string const& cand1, std::string const& cand2)
	{
		return cand1.size() <= cand2.size();
	}
};


/*
* * @brief: This function fids all maximal cliques
* @param: S: a set of instances
*	      Q: global variable to constitutue a clique
* Method:
*	The worst-case time complexity for generating all maximal cliques and computational experiments
* @retval: a set of maximal cliques
*/
void CLIQUES(std::vector<ObjWithoutCoordinate>& SUBG, std::vector<ObjWithoutCoordinate>& CAND)
{
	if (SUBG.empty())
	{
		// Q is a maximal clique
		/*std::cout << "get one MC: ";
			for (auto const& inst : Q)
			{
				std::cout << inst.feature << "." << inst.instance << ", ";
			}
			std::cout << endl;*/
		// Get one maximal cliques and construct hashmap
		timeConstCoLHashmap = clock();
		if (Q.size() > 1)
		{
			constructCoLoHashmap(Q);			
		}
		totalTimeConstCoLHashmap = totalTimeConstCoLHashmap + double(clock() - timeConstCoLHashmap);
	}
	else
	{
		// chose a pivot
		int t,s,u=0,maxnei = 0,sSUBG= SUBG.size(), sdefCANRu;
		std::vector<ObjWithoutCoordinate> intersecCANDRu; // consider set as a global variable
		for (t=0; t < sSUBG; ++t)
		{
			std::set_intersection(CAND.begin(), CAND.end(),
				SN.find(SUBG[t])->second.begin(), SN.find(SUBG[t])->second.end(),
				std::back_inserter(intersecCANDRu));

			s = intersecCANDRu.size();
			if ( s > maxnei)
			{
				maxnei = s;
				// noted 
				u = t;
			}
			intersecCANDRu.clear();
		}
		// get difference, line 7: CAND \ R(u)
		std::vector<ObjWithoutCoordinate> deffCANDRu;
		std::set_difference(CAND.begin(), CAND.end(), SN.find(SUBG[u])->second.begin(), SN.find(SUBG[u])->second.end(),
			std::back_inserter(deffCANDRu));
		//sdefCANRu = deffCANDRu.size();
		while (deffCANDRu.size())
		{
			// get a point p and add to Q
			Q.push_back(deffCANDRu.back()); // lines 8-9
			// update new SUBG
			std::vector<ObjWithoutCoordinate> SUBGp, CANDp;
			itSN = SN.find(deffCANDRu.back());
			if (itSN != SN.end())
			{
				// line 10
				std::set_intersection(SUBG.begin(), SUBG.end(),
					itSN->second.begin(), itSN->second.end(),
					std::back_inserter(SUBGp));
				// line 11
				std::set_intersection(CAND.begin(), CAND.end(),
					itSN->second.begin(), itSN->second.end(),
					std::back_inserter(CANDp));
				// re
				CLIQUES(SUBGp, CANDp);
				// back, line 13
				//std::cout << "be: " << CAND.size()<<endl;
				CAND.erase(std::remove(CAND.begin(), CAND.end(), deffCANDRu.back()), CAND.end());
				//std::cout<<"af: "<< CAND.size() << endl;				
				// back, line 14
				//Q.pop_back();
				Q.erase(std::remove(Q.begin(), Q.end(), deffCANDRu.back()), Q.end());
				// remove current node
				deffCANDRu.pop_back();
			}
		}
	}
}


/*
* * @brief: This function calculates Pattern Utility Ratio and filter maximal HUCPs
* @param: CoLHM: a co-location hash map
*	utility_thres: the utility threshold
* Method:
*	first: get all key and sort by size put into Ck
*	second: get the largest size of key
*	third: calculate upi of this key c,
	fourth: generate subsets of c and put into new candidate set.
* @retval: a set of all high utility co-location patterns
*/
void filterHUandPCPs(std::vector<Patterns>& HUPCPs, float uti_thres, float pre_thres)
{
	// 0. Create a set the save the non-prevalent CPs		
	std::unordered_map<std::string, float>::iterator itchecked; // if a candidate enter, checking it having already processed
	//1. Get all candidate and they are sorted by sizes of keys	
	std::vector<std::string> candidates;	
	for (auto const& item : CoLHM)
	{
		candidates.push_back(item.first);
	}
	// Sort candidates by sizes of keys, since the mining process starts from the end, thus sort size by decrease order
	//std::sort(candidates.begin(), candidates.end(), compareBySize());
	//printcandidates(candidates);	

	// Loop each candidate
	float UPI;
	int k; // ck is the current size of candidate	
	std::vector<std::string> sizekSubsets; // save the 
	bool isInHashMap;
	std::vector<float> PRs;

	while (!candidates.empty())
	{
		// 4.1 Get the first element
		std::string currCandidate = candidates.back();
		candidates.pop_back();		
		// 4.2 Check if this candidate is already checked
		itchecked = checkedCands.find(currCandidate);
		if (itchecked != checkedCands.end()) // find out
		{
			//std::cout << "Checked \n";
			continue;
		}
		else
		{
			// a new candidate
			checkedCands.insert({ currCandidate, 1.0f });
			k = currCandidate.size();
			// get its participating instances
			std::vector<std::unordered_set<ObjWithoutCoordinate, myHashFunc>> insts(k); // save instances	
			//std::vector<std::unordered_map<std::string, std::unordered_map<char, std::unordered_set <ObjWithoutCoordinate, myHashFunc>>>::iterator> superItV;
			itCoLHM = CoLHM.begin();
			while (itCoLHM != CoLHM.end())
			{
				// check currCandidate is not or subset of keys in chash
				isInHashMap = std::includes(itCoLHM->first.begin(), itCoLHM->first.end(),
					currCandidate.begin(), currCandidate.end());
				if (isInHashMap)
				{
					// get all instances
					for (size_t t = 0; t < k; ++t)
					{
						insts[t].insert(itCoLHM->second.find(currCandidate[t])->second.begin(),
							itCoLHM->second.find(currCandidate[t])->second.end());
					}
				}
				++itCoLHM;
			}

			// compute it UPI
			PRs.clear();
			for (size_t t = 0; t < k; ++t)
			{
				//UPI += (float)insts[t].size() * uf.find(currCandidate[t])->second;
				PRs.push_back((float)insts[t].size() / (float)totNumInsFeat.find(currCandidate[t])->second * uf.find(currCandidate[t])->second);
			}
			//UPI = Precision(UPI / US, 3);

			// Compute new UPI
			UPI = 0.0f;
			for (auto const& f : PRs)
			{
				UPI += f;
			}
			UPI = Precision(UPI/(float)currCandidate.size(), 3);
			
			if (UPI >= uti_thres)
			{
				Patterns c{currCandidate, UPI };
				HUPCPs.push_back(c);
			}
			// generate new candidates
			if (k > 2)
			{
				sizekSubsets.clear();
				sizekSubsets = combo(currCandidate, k - 1);
				// check new candidate has been processed
				for (auto const& newc : sizekSubsets)
				{
					itchecked = checkedCands.find(newc);
					if (itchecked != checkedCands.end())
					{
						continue;
					}
					else
					{
						candidates.push_back(newc);
					}
				}
			}
		}
	}
}




// Comparator function to sort pairs
// according to second value
bool cmp(std::pair<std::string, float>& a,
	std::pair<std::string, float>& b)
{
	return a.first.size() <= b.first.size();
}


/**
* @brief: This function classify patterns by sizes
* @param: allPrevPats: all patterns
*         classPattBySize: classified pattens by sizes
* @retval: 
*/
void classifyPattsBySize(std::vector<Patterns> & HUPCPs, std::map<int, std::vector<Patterns>>& classPattBySize)
{
	int sizeP;
	std::map<int, std::vector<Patterns>>::iterator itc;

	for (auto const& c: HUPCPs)
	{
		sizeP = c.patts.size();
		itc = classPattBySize.find(sizeP);
		if (itc != classPattBySize.end())
		{			
			itc->second.push_back(c);
		}
		else
		{
			classPattBySize.insert({ sizeP, std::vector<Patterns>{c} });
		}
	}
}


