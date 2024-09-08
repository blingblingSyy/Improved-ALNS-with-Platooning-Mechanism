#include <iostream>
#include <fstream>
#include <vector>
#include "alns/statistics/Statistics.h"

using namespace std;

Statistics::~Statistics()
{
	for(size_t i = 0; i < nbCalls.size(); i++)
	{
		delete nbCalls[i];
		delete weights[i];
	}
	delete operatorNames;
}

void Statistics::addEntry(double timeStamp,
							size_t iteration,
							string strategyName,
							string nodeDestroyName,
							string nodeRepairName,
							string pathDestroyName,
							string pathRepairName,
							double newCost,
							double currentCost,
							double bestCost,
							int cumKS)
{
	timeStamps.push_back(timeStamp);
	iterations.push_back(iteration);
	strategyNames.push_back(strategyName);
	nodeDestroyNames.push_back(nodeDestroyName);
	nodeRepairNames.push_back(nodeRepairName);
	pathDestroyNames.push_back(pathDestroyName);
	pathRepairNames.push_back(pathRepairName);
	newCosts.push_back(newCost);
	currentCosts.push_back(currentCost);
	bestCosts.push_back(bestCost);
	cumulativeKnownSolutions.push_back(cumKS);
}

void Statistics::addOperatorEntry(vector<double>* weight,
					  			vector<size_t>* calls)
{
	timeStampsOperators.push_back(static_cast<double>(clock()-start)/CLOCKS_PER_SEC);
	weights.push_back(weight); //! sequence: nodeFirstStrategy, pathFirstStrategy, nodeDestroyOperators, nodeRepairOperators, pathDestroyOperators, pathRepairOperators
	nbCalls.push_back(calls);
}

void Statistics::generateStatsFile(string pathGlob, string pathOp)
{
	ofstream myfile;
	myfile.open(pathGlob.c_str(), ios::out | ios::trunc);
	myfile << "iterations\ttimeStamps\tStrategy\tNode Destroy Operator\tNode Repair Operator\tPath Destroy Operator\tPath Repair Operator\tNew Cost\tCurrent Cost\tBest Cost\tCumulative Known Solutions\n";
	for(size_t i = 0; i < iterations.size(); i++)
	{
		myfile << iterations[i] << "\t" << timeStamps[i] << "\t" << strategyNames[i] << "\t" << nodeDestroyNames[i] << "\t" << nodeRepairNames[i] << "\t" << pathDestroyNames[i] << "\t" << pathRepairNames[i] << "\t" << newCosts[i] << "\t" << currentCosts[i] << "\t" << bestCosts[i] << "\t" << cumulativeKnownSolutions[i] << "\n";
	}
	myfile.close();

	ofstream myfileOp;
	myfileOp.open(pathOp.c_str(), ios::out | ios::trunc);
	myfileOp << "time stamp\t";
	for(size_t i = 0; i < operatorNames->size(); i++)
	{
		myfileOp << "Weight_" << operatorNames->at(i) << "\tCalls_" << operatorNames->at(i) << "\t";
	}
	myfileOp << "\n";

	for(size_t i = 0; i < nbCalls.size(); i++)
	{
		myfileOp << timeStampsOperators[i] << "\t";
		for(size_t j = 0; j < operatorNames->size(); j++)
		{
			myfileOp << weights[i]->at(j) << "\t" << nbCalls[i]->at(j) << "\t"; //! nbCalls: getNumberOfCallsSinceLastEvaluation()
		}
		myfileOp << "\n";
	}
	myfileOp.close();
}
