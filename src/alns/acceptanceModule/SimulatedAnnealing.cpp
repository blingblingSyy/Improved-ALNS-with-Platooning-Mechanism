#include <assert.h>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include "alns/acceptanceModule/SimulatedAnnealing.h"
#include "alns/acceptanceModule/ICoolingSchedule.h"
#include "alns/improvedALNS/ISolution.h"
#include "alns/improvedALNS/ALNS_Iteration_Status.h"
#include "alns/improvedALNS/IBestSolutionManager.h"

SimulatedAnnealing::SimulatedAnnealing(ICoolingSchedule& cs) {
	coolingSchedule = &cs;
}

SimulatedAnnealing::~SimulatedAnnealing() {
	delete coolingSchedule;
}

bool SimulatedAnnealing::transitionAccepted(IBestSolutionManager& bestSolutionManager, ISolution& currentSolution, ISolution& newSolution, ALNS_Iteration_Status& status)
{
	double temperature = coolingSchedule->getCurrentTemperature();
	if(newSolution < currentSolution)
	{
		return true;
	}
	else
	{
		double difference = newSolution.getPenalizedObjectiveValue() - currentSolution.getPenalizedObjectiveValue(); //! modified = false;
		double randomVal = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
		return (exp(-1*difference/temperature) > randomVal);
	}
}

void SimulatedAnnealing::startSignal()
{
	coolingSchedule->startSignal();
}
