#ifndef TOOLFUNCTIONS_H
#define TOLLFUNCTIONS_H

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
//#include <bitset>
#include <ctime>
#include <unordered_set>
#include <boost/dynamic_bitset.hpp>
#include <stdlib.h>     /* using abs */

#include "Object.h"
#include "SetNumberPointFloat.h"
#include "Patterns.h"




// Definition global variables
extern clock_t timeConstCoLHashmap;
extern double totalTimeConstCoLHashmap;

// Save co-location hash map
extern std::unordered_map<std::string, std::unordered_map<char, std::unordered_set<ObjWithoutCoordinate, myHashFunc>>> CoLHM;
extern std::unordered_map<std::string, std::unordered_map<char, std::unordered_set <ObjWithoutCoordinate, myHashFunc>>>::iterator itCoLHM;

extern std::unordered_map<char, float> uf; // utility of each feature
extern float US; // utility of the dataset
extern std::unordered_map<char, float> totNumInsFeat; // Save the minimum prevalence of each feature, this value = alpha x total number instancces of each feature
extern std::unordered_map<ObjWithoutCoordinate, std::vector<ObjWithoutCoordinate>, myHashFunc> SN;
extern std::unordered_map<ObjWithoutCoordinate, std::vector<ObjWithoutCoordinate>, myHashFunc>::iterator itSN;

extern std::unordered_map<std::string, float> checkedCands; // save all non PCPs

extern std::vector<ObjWithoutCoordinate> Q; // a global variable representing a clique




/**
* @brief This function make a grid on an input dataset
* @param dataList: an input dataset; dist_thres: a distance threshold
* @retval a hash map that stores cell id with instances fall in it.
*/
std::map<std::pair<int, int>, std::vector<Objects>> makeGrid(
	std::vector<std::vector<std::string>> &dataList, float dist_thres);


/**
* @bref This function calculates the distances of instances in the current block.
* @param alll instance in the current block
* @retval A vector that save the distance of instances.
*/
float calculateDistanceTwoInstances(Objects currentInst, Objects checkInst);


/**
* @brief: This function generates star neighborhoods of instances.
* @param: grid: a grid posing of the input dataset
*         dist_thres: a distance threshold
* @retval: SN: a hash map that stores as <instance, <<neighbors>,<neighbors>>. This struture is different with star neighbors in Join-less
*/
void genStarNeighborhoods(
	std::vector<ObjWithoutCoordinate>& S,
	std::map<std::pair<int, int>, std::vector<Objects>>& grid,
	float dist_thres);


/**
* @brief: this function computes the utility of the dataset and utility of each feature
* @param: minf, alphas:
* @retval: minf
*/
void computeUSandUtiEachFeature();


/**
* @brief: This function sorts vertices by their degeneracy.
* @param: vertices: a set of indexes of all vertices
*         neighbors: star neighbors of all vertices
* @retval: a vector of oredered vertices sorted by degeneracy
*/
void OrderedDegeneracy(std::vector<ObjWithoutCoordinate>& orderDeg);


/**
* @brief: This function enumerates all maximal cliques based on bron-kerbosch pivot.
* @param: vertices: a set of all vertices (instances)
*         neighbors: a hashmap stores neighbors of each instance
* @retval: allMaxCliques: all maximal cliques.
*/
void BronKerboschPivot(std::vector<ObjWithoutCoordinate> P, std::vector<ObjWithoutCoordinate> R, std::vector<ObjWithoutCoordinate> X);

/**
* @brief: This function get maximal cliques
* @param: vertices: a set of indexes of all vertices
*         neighbors: star neighbors of all vertices
* @retval: a vector of oredered vertices sorted by degeneracy
*/
void BronKerboschDeg(std::vector<ObjWithoutCoordinate> P,
	std::vector<ObjWithoutCoordinate> X,
	std::vector<ObjWithoutCoordinate>& orderDeg);


/**
* @brief: This function filters prevalent patterns
* @param: CoLHM: the co-location hash map.
*         pre_thress: a minimum prevalent threshold.
* @retval: allPrevPats: a co-location hashmap.
*/
int findMaxSizeCand(std::map<std::string, int>& candPatts);


/**
* @brief: This function buids the co-location hashmap structure.
* @param: allMaxCliques: all maximal cliques.
* @retval: CoLoHM: a co-location hashmap.
*/
void constructCoLoHashmap(std::vector<ObjWithoutCoordinate> R);


/*
* * @brief: This function fids all maximal cliques
* @param: S: a set of instances
*	      Q: global variable to constitutue a clique
* Method:
*	The worst-case time complexity for generating all maximal cliques and computational experiments
* @retval: a set of maximal cliques
*/
void CLIQUES(std::vector<ObjWithoutCoordinate>& S, std::vector<ObjWithoutCoordinate>& Q);


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
void filterHUandPCPs(std::vector<Patterns>& HUPCPs, float uti_thres, float pre_thres);


/*
*@brief: This function generate all combination of a char vector
*@param: c: a vector of charl; combo: the size of combo; C(n, m) = n!/(m!(n-m)!)
*@retval: a vector of sub vectors of the vector
*/
template<typename T>
std::string getCombination(const T& c, int combo);

template<typename T>
std::vector<std::string> combo(const T& c, int k);



// Comparator function to sort pairs
// according to second value
bool cmp(std::pair<std::string, float>& a,
	std::pair<std::string, float>& b);


/**
* @brief: This function classify patterns by sizes
* @param: allPrevPats: all patterns
*         classPattBySize: classified pattens by sizes
* @retval:
*/
void classifyPattsBySize(std::vector<Patterns>& HUPCPs, std::map<int, std::vector<Patterns>>& classPattBySize);




#endif