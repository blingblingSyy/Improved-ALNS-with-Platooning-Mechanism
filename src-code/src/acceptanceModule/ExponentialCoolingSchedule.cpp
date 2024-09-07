#include "src/acceptanceModule/ExponentialCoolingSchedule.h"
#include <algorithm>
#include <math.h>
#include <iostream>
#include <assert.h>
#include "src/improvedALNS/ISolution.h"
#include "src/acceptanceModule/CoolingSchedule_Parameters.h"

ExponentialCoolingSchedule::ExponentialCoolingSchedule(ISolution& initSol, CoolingSchedule_Parameters& csParam)
{
	// The fields are instantiated to default values.
	start_temp = (csParam.setupPercentage*initSol.getPenalizedObjectiveValue(true))/(-log(0.5)); //! modified = true
	end_temp = csParam.endingTemparature;
}

ExponentialCoolingSchedule::ExponentialCoolingSchedule(double startingTemperature, double endingTemparature)
{
	this->start_temp = startingTemperature;
	end_temp = endingTemparature;
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
