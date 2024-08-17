#include <iostream>
#include "Object.h"
#include <string>
#include <map>
#include <vector>
#include <set>
#include "PrintFunctions.h"
#include <unordered_map>

using namespace std;



/**
* @brief: This function print the star neighborhoods.
* @param: SN: the star neighborhoods of each instances.
* @retval: Nope
*/
void printStarNeighborhood(
	std::unordered_map<ObjWithoutCoordinate, std::vector<ObjWithoutCoordinate>, myHashFunc> & SN)
{
	//std::cout << "The star neighborhoods are: " << std::endl;
	for (auto const& feat : SN)
	{
		std::cout << feat.first.feature << "." << feat.first.instance << " : {";
		for (auto const& inst: feat.second)
		{
			std::cout << inst.feature << "." << inst.instance << ", ";
		}
		std::cout << " } \n";
	}
	std::cout << endl;
}



/**
* @brief: This function prints all instances of the input dataset.
* @param: vertices: a set of all instances.
* @retval: Nope
*/
void printVertices(std::unordered_map<ObjWithoutCoordinate, int, myHashFunc> vertices)
{
	//std::cout << "All instances in the input data set are: \n";
	for (auto const& vetex : vertices)
	{
		std::cout << vetex.first.feature << "." << vetex.first.instance << ", ";
	}
	std::cout << endl;
}


/**
* @brief: This function prints the number instance of each feature
* @param: minu
* @retval: Nope
*/
void printNumInstEachFeat()
{
	std::cout << "The number instance of each feature: \n";
	for (auto const& f : totNumInsFeat)
	{
		std::cout << f.first << " : " << f.second << endl;
	}
	std::cout << endl;
}



/**
* @brief: This function prints min prevalence threshold of each feature
* @param: minu
* @retval: Nope
*/
void printu(std::unordered_map<char, float> vf)
{
	//std::cout << "Prevalence threshold of each feature: \n";
	for (auto const& f : vf)
	{
		std::cout << f.first << " : " << f.second << endl;
	}
	std::cout << endl;
}


/**
* @brief: This function prints neighbors of an instances of in one block.
* @param: vertices: a set of all instances.
* @retval: Nope
*/
void printNeiOneBlock(std::unordered_map<ObjWithoutCoordinate, std::vector<ObjWithoutCoordinate>, myHashFunc> oneNeiBlock)
{
	std::cout << "Instances and their neighbors: \n";
	for (auto const& inst : oneNeiBlock)
	{
		std::cout << inst.first.feature << "." << inst.first.instance << " : "; 
		for (auto const& nei : inst.second)
		{
			std::cout << nei.feature << "." << nei.instance << ", ";
		}
		std::cout << endl;
	}
	std::cout << endl;
}



/**
* @brief: This function prints a maximal clique
* @param: cl: a clique
* @retval: Nope
*/
void printClique(std::vector<ObjWithoutCoordinate> cl)
{
	//std::cout << "A clique \n";
	for (auto const& inst : cl)
	{
		std::cout << inst.feature << "." << inst.instance << " ";
	}
	std::cout << endl <<endl<<endl;
}


/**
* @brief: This function prints colohashmap
* @param: CoLHM: a hash map of all maximal cliques
* @retval: Nope
*/
void printCoLHM()
{
	std::cout << "The CoLoHM is: \n";
	for (auto const& item : CoLHM)
	{
		// Print key
		std::cout << item.first << " : \n";
		// Print value
		for (auto const& featinst : item.second)
		{
			// Print char or feature
			std::cout << featinst.first << " : ";
			// Print instances
			for (auto const& inst : featinst.second)
			{
				std::cout << inst.feature<<"."<<inst.instance << ", ";
			}
			std::cout << endl;
		}
		std::cout << endl;
	}
	std::cout << endl;
}

/**
* @brief: This function prints all candidates.
* @param: candidates: all prevelant patterns.
* @retval: Nope
*/
void printcandidates(std::vector<std::string> candidates)
{
	std::cout << "Candidates are: " << endl;
	for (auto const& c : candidates)
	{
		std::cout << c << endl;
	}
	std::cout << endl;
}


/**
* @brief: This function prints all prevelant co-location patterns.
* @param: allPrevPats: all prevelant patterns.
* @retval: Nope
*/
void printPrevPatts(std::vector<Patterns> allPrevPats)
{
	std::cout << "All high utility - prevalent co-location patterns: " << endl;
	for (auto const& item : allPrevPats)
	{
		std::cout << item.patts<< " : "<< item.UPI << endl;
	}
	std::cout << endl;
}


/**
* @brief: This function prints sorted prevelant co-location patterns by sizes of patterns
* @param: allPrevPats: all prevelant patterns.
* @retval: Nope
*/
void printSortPrevPats(std::vector<std::pair<std::string, float>> sortPrevPats)
{
	for (auto const& item : sortPrevPats)
	{
		std::cout << item.first << " : " << item.second << "\n ";
	}
	std::cout << endl;
}



/**
* @brief: This function prints patterns by sizes
* @param: classPattBySize: all prevelant patterns are classified by size
* @retval: Nope
*/
void printNumberPattsBySize(std::map<int, std::vector<Patterns>>& classPattBySize)
{
	std::cout << "Patterns are classified by sizes: \n";
	for (auto const& item : classPattBySize)
	{
		std::cout << item.first << " : " << item.second.size();
		std::cout << endl;
	}
	std::cout << endl;
}


/**
* @brief: This function prints top-k pattern of size 3, 4, 5
* @param: classPattBySize: all prevelant patterns are classified by size
*		topk: top k patterns
* @retval: Nope
*/
void printTopKPatts(std::map<int, std::vector<Patterns>>& classPattBySize, int topk)
{
	for (auto const& item : classPattBySize)
	{
		if (item.first == 3)
		{
			int k = 1;
			for (auto const& pat : item.second)
			{
				if (k <= topk)
				{
					
				}
				else
				{

				}
			}
		}
	}


}





