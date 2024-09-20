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
	myfile << "iterations\t\ttimeStamps\t\tStrategy\t\tNode Destroy Operator\t\tNode Repair Operator\t\tPath Destroy Operator\t\tPath Repair Operator\t\tNew Cost\t\tCurrent Cost\t\tBest Cost\t\tCumulative Known Solutions\n";
	for(size_t i = 0; i < iterations.size(); i++)
	{
		myfile << iterations[i] << "\t\t" << timeStamps[i] << "\t\t" << strategyNames[i] << "\t\t" << nodeDestroyNames[i] << "\t\t" << nodeRepairNames[i] << "\t\t" << pathDestroyNames[i] << "\t\t" << pathRepairNames[i] << "\t\t" << newCosts[i] << "\t\t" << currentCosts[i] << "\t\t" << bestCosts[i] << "\t\t" << cumulativeKnownSolutions[i] << "\n";
	}
	myfile.close();

	ofstream myfileOp;
	myfileOp.open(pathOp.c_str(), ios::out | ios::trunc);
	myfileOp << "time stamp\t\t";
	for(size_t i = 0; i < operatorNames->size(); i++)
	{
		myfileOp << "Weights of " << operatorNames->at(i) << "\t\tCalls of " << operatorNames->at(i) << "\t\t";
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
