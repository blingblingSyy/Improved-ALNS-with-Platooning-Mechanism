#include "alns/acceptanceModule/ExponentialCoolingSchedule.h"
#include <algorithm>
#include <math.h>
#include <iostream>
#include <assert.h>
#include "alns/improvedALNS/ISolution.h"
#include "alns/acceptanceModule/CoolingSchedule_Parameters.h"

ExponentialCoolingSchedule::ExponentialCoolingSchedule(ISolution& initSol, CoolingSchedule_Parameters& csParam)
{
	// The fields are instantiated to default values.
	this->start_temp = (csParam.setupPercentage*initSol.getPenalizedObjectiveValue(true))/(-log(0.5)); //! modified = true
	this->end_temp = csParam.endingTemparature;
	this->decreasingFactor = csParam.expPercentageKept;
}

ExponentialCoolingSchedule::ExponentialCoolingSchedule(double startingTemperature, double endingTemparature, double decreasingfactor)
{
	this->start_temp = startingTemperature;
	this->end_temp = endingTemparature;
	this->decreasingFactor = decreasingfactor;
}

ExponentialCoolingSchedule::~ExponentialCoolingSchedule() {
	// Nothing to be done
}

double ExponentialCoolingSchedule::getCurrentTemperature()
{
	start_temp *= decreasingFactor;
	if(start_temp <= end_temp)
	{
		start_temp = end_temp;
	}
	assert(start_temp > 0);
	return start_temp;
}

void ExponentialCoolingSchedule::startSignal()
{

}
