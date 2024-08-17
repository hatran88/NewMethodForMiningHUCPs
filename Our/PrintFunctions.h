
#ifndef PRINTFUNCTIONS_H
#define PRINTFUNCTIONS_H

#include <iostream>
#include "Object.h"
#include <string>
#include <map>
#include <vector>
#include <set>
#include <unordered_map>

#include "ToolFunctions.h"


using namespace std;


/**
* @brief: This function print the star neighborhoods.
* @param: SN: the star neighborhoods of each instances.
* @retval: Nope
*/
void printStarNeighborhood(
	std::unordered_map<ObjWithoutCoordinate, std::vector<ObjWithoutCoordinate>, myHashFunc> & SN);


/**
* @brief: This function prints all instances of the input dataset.
* @param: vertices: a set of all instances.
* @retval: Nope
*/
void printVertices(std::unordered_map<ObjWithoutCoordinate, int, myHashFunc> vertices);


/**
* @brief: This function prints neighbors of an instances of in one block.
* @param: vertices: a set of all instances.
* @retval: Nope
*/
void printNeiOneBlock(std::unordered_map<ObjWithoutCoordinate, std::vector<ObjWithoutCoordinate>, myHashFunc> oneNeiBlock);


/**
* @brief: This function prints a maximal clique
* @param: cl: a clique
* @retval: Nope
*/
void printClique(std::vector<ObjWithoutCoordinate> cl);


/**
* @brief: This function prints the number instance of each feature
* @param: minu
* @retval: Nope
*/
void printNumInstEachFeat();


/**
* @brief: This function prints min prevalence threshold of each feature
* @param: minu
* @retval: Nope
*/
void printminu();

/**
* @brief: This function prints min prevalence threshold of each feature
* @param: minu
* @retval: Nope
*/
void printu(std::unordered_map<char, float> vf);


/**
* @brief: This function prints colohashmap
* @param: CoLHM: a hash map of all maximal cliques
* @retval: Nope
*/
void printCoLHM();


/**
* @brief: This function prints all prevelant co-location patterns.
* @param: allPrevPats: all prevelant patterns.
* @retval: Nope
*/
void printPrevPatts(std::vector<Patterns> allPrevPats);


/**
* @brief: This function prints all candidates.
* @param: candidates: all prevelant patterns.
* @retval: Nope
*/
void printcandidates(std::vector<std::string> candidates);

/**
* @brief: This function prints sorted prevelant co-location patterns by sizes of patterns
* @param: allPrevPats: all prevelant patterns.
* @retval: Nope
*/
void printSortPrevPats(std::vector<std::pair<std::string, float>> sortPrevPats);


/**
* @brief: This function prints patterns by sizes
* @param: classPattBySize: all prevelant patterns are classified by size
* @retval: Nope
*/
void printNumberPattsBySize(std::map<int, std::vector<Patterns>>& classPattBySize);


/**
* @brief: This function prints top-k pattern of size 3, 4, 5
* @param: classPattBySize: all prevelant patterns are classified by size
*		topk: top k patterns
* @retval: Nope
*/
void printTopKPatts(std::map<int, std::vector<Patterns>>& classPattBySize, int tokp);






#endif
