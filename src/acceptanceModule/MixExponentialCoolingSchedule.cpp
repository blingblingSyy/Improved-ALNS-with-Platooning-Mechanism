#include "src/acceptanceModule/MixExponentialCoolingSchedule.h"
#include <algorithm>
#include <math.h>
#include <iostream>
#include "src/improvedALNS/ISolution.h"
#include "src/acceptanceModule/CoolingSchedule_Parameters.h"

MixExponentialCoolingSchedule::MixExponentialCoolingSchedule(ISolution& initSol, CoolingSchedule_Parameters& csParam)
{
	// The fields are instantiated to default values.
	this->maximumIt = csParam.maxIt;
	this->currentIt = 0;
	this->currentThreshold = 0;
	this->nbThresholds = csParam.nbThresholds;
	this->decreasingFactor = csParam.expPercentageKept;
	this->runTime = csParam.maxRT;
	currentTemperature = (csParam.setupPercentage*initSol.getPenalizedObjectiveValue(true))/(-log(0.5)); //! modified = true
}

MixExponentialCoolingSchedule::~MixExponentialCoolingSchedule() {
	// Nothing to be done
}

double MixExponentialCoolingSchedule::getCurrentTemperature()
{
	currentIt++;
	clock_t currentTime = clock();
	double percentageTime = ((double)(currentTime - startingTime))/(endingTime- startingTime);
	double percentageIt = ((double)currentIt)/maximumIt;
	double percentageM = percentageTime;  //! take the max(percentageTime, percentageIt)
	if(percentageIt > percentageM)
	{
		percentageM = percentageIt;
	}
	int aimedThreshold = (int)(percentageM * nbThresholds);
	while(currentThreshold < aimedThreshold) //! while current threshold is larger than the aimed threshold, the temperature will not be recomputed anymore
	{
		currentThreshold++;
		currentTemperature *= decreasingFactor;
	}
	return currentTemperature;
}

void MixExponentialCoolingSchedule::startSignal()
{
	startingTime = clock();
	endingTime = startingTime + CLOCKS_PER_SEC*runTime;
}
