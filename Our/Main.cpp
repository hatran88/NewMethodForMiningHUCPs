/* Source.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// This program mining high-prevalent co-location patterns
//
// References:
//	1. Effciently Finding High Utility-Frequent Itemsets using Cuto and Sux Utility
//  -> to mine high-prevalent co-location patterns
// 
//	2. On the overall and delay complexity of the CLIQUES and Bron-Kerbosch algorithms
//  -> to find maximal cliques -> using CLIQUES algorithm

// The objective: discover all patterns that PUI > threshold and PI > threshold
//
// Method: use cliques + hash table
//
// Function:
//		1. Threshold for each feature is a portion of its number instances
//		2. Calculate execution time
//		3. Collect memory usage
//
//
Date: 05/08/2024
// Writtern by: Hatran
*/


#include <iostream> 
#include <fstream> 
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <map>
#include <set>
#include "Object.h"
#include "ReaderCSV.h"
#include "PrintFunctions.h"
#include "ToolFunctions.h"


#include <ctime>
#include <unordered_map>
#include <unordered_set>


// For collect memory usage
#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")


//#include "single_include/nlohmann/json.hpp" // using json
//using json = nlohmann::json;


using namespace std;


int main()
{

	// Set time
	clock_t startTime, endTime; // start and end time
	clock_t timeMaterializeNeighbor; // time for materializing star neighborhoods	
	clock_t timeFindMaxCliques;
	clock_t timeFilCliqueInstance; // time for collecting co-location instances
	clock_t timeFilPrevCoLoPat; // time for filtering co-location instances

	double time_taken; // count total execution time
	double totalTimeMatStarNei = 0.0f; // total time for materializing star neighborhoods	
	double totalTimeFinMaxCliques = 0.0f;
	double totalTimeFilPrevCoLoPat = 0.0f; // total time for filgering candidates

	startTime = clock(); // Begin count time

	// 1. Set parameters	
	float dist_thres = 100.0f;  // set the distance threshold	
	float alpha = 0.0f; // set the size of subgrahp
	float prev_thres = 0.2f;
	float uti_thres = 0.1f;


	// 2. Load data sets
	string file_name = "./Data/Ex1/LasVegas_x_y_alphabet_version_03_2.csv";
	freopen("./Data/Ex1/LasVegas_x_y_alphabet_version_03_2_distance_100_pi_02_uti_015_3.txt", "w", stdout);

	// Creating an object of CSVReader/CSVWriter
	CSVReader reader(file_name);
	std::vector<std::vector<std::string>> dataList = reader.getData(); // Get the data from CSV File																   
	// Delete the first line
	dataList.erase(dataList.begin());

	// 3. Make grid and find neighbors
	// 3.1 Make grid and get all instances (vertices)	
	std::map<std::pair<int, int>, std::vector<Objects>> grid = makeGrid(dataList, dist_thres);
	dataList.clear();
	//printGrid(grid);

	// 3.2 Save all instances to S, get neighbors itSN and itBN, and count the instance number of each feature to compute threshold, minf
	std::vector<ObjWithoutCoordinate> S;
	genStarNeighborhoods(S, grid, dist_thres);
	std::sort(S.begin(), S.end());
	//printNumInstEachFeat();	

	// 3.3 Gen threshold -> return minf
	// This function computes the utility of the dataset, US and utility of each feature, uf
	// The utility of each feature is normalized
	computeUSandUtiEachFeature();
	//std::cout << "Number instances of each feature: \n";
	//printu(totNumInsFeat);
	//std::cout << "Total utility of the dataset US: " << US << endl;
	//std::cout << "Utility of each feature: \n";
	//printu(uf);	

	timeMaterializeNeighbor = clock();
	totalTimeMatStarNei = totalTimeMatStarNei + double(timeMaterializeNeighbor - startTime);

	// 4. Finding cliques by iterator each instance and its neighbors
	timeFindMaxCliques = clock();
	// Compute the degency			
	//std::vector<ObjWithoutCoordinate> orderDeg;
	//OrderedDegeneracy(orderDeg);

	// Find maximal cliques
	//std::vector<ObjWithoutCoordinate> X;
	//BronKerboschDeg(S, X, orderDeg);

	// Use CLIQUES algorithm
	//std::vector<ObjWithoutCoordinate> Q;
	CLIQUES(S, S);


	// 4.2. Get cliques from the candidate map to construct hash map

	totalTimeFinMaxCliques = totalTimeFinMaxCliques + double(clock() - timeFindMaxCliques) - totalTimeConstCoLHashmap;

	// 5. Find PCPs		
	//std::cout << "Co-location hash map: \n";
	//printCoLHM();

	// Ste 4. Construct co-location hashmap	
	// This step is included in the listing maximal cliques step.
	timeFilPrevCoLoPat = clock();
	//std::unordered_map<std::string, float> allHUCPs;
	//filterallHUCPs(allHUCPs, utility_thres);
	std::vector<Patterns> HUPCPs;
	filterHUandPCPs(HUPCPs, uti_thres, prev_thres);

	std::cout << "Number of PCPs: " << HUPCPs.size() << endl;
	printPrevPatts(HUPCPs);
	totalTimeFilPrevCoLoPat = double(clock() - timeFilPrevCoLoPat);

	// Step 4. Count time and print time
	endTime = clock();
	time_taken = double(endTime - startTime);

	std::cout << "Time for meterializing neighbors: " << totalTimeMatStarNei / CLOCKS_PER_SEC << " s." << endl;
	std::cout << "Time for finding cliques: " << totalTimeFinMaxCliques / CLOCKS_PER_SEC << " s." << endl;
	std::cout << "Time for constructingco-location hashmap: " << totalTimeConstCoLHashmap / CLOCKS_PER_SEC << " s." << endl;
	std::cout << "Time for filtering PCPs: " << totalTimeFilPrevCoLoPat / CLOCKS_PER_SEC << " s." << endl;
	std::cout << "Execution time (no materialize neighbors) is : " << (totalTimeFinMaxCliques + totalTimeFilPrevCoLoPat) / CLOCKS_PER_SEC << " s." << endl;
	std::cout << "Time taken by the program is : " << time_taken / CLOCKS_PER_SEC << " s." << endl;

	// Show memory usage
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS memCounter;
	GetProcessMemoryInfo(handle, &memCounter, sizeof(memCounter));
	SIZE_T physMemUsedByMe = memCounter.WorkingSetSize;
	SIZE_T peakMemUsedByMe = memCounter.PeakWorkingSetSize;
	std::cout << "Memory usage: " << physMemUsedByMe / 1024 << "(kB)" << std::endl;
	std::cout << "Peak memory usage: " << peakMemUsedByMe / 1024 / 1024 << "(MB)" << std::endl;

	// Get the maximal size of patterns
	std::vector<int> sizePats;
	for (auto const& c : HUPCPs)
	{
		sizePats.push_back(c.patts.size());
	}	
	int maxSize = *std::max_element(sizePats.begin(), sizePats.end());
	std::cout << "The maximal size of patterns is: " << maxSize << endl;

	//2. Classify pattern by sizes
	std::map<int, std::vector<Patterns>> classPattBySize;
	classifyPattsBySize(HUPCPs, classPattBySize);
	printNumberPattsBySize(classPattBySize);


	return 0;

}
