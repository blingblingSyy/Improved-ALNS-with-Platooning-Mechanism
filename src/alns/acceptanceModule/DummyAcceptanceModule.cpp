#include "alns/acceptanceModule/DummyAcceptanceModule.h"
#include "alns/ImprovedALNS/ISolution.h"
#include "alns/ImprovedALNS/ALNS_Iteration_Status.h"
#include "alns/ImprovedALNS/IBestSolutionManager.h"
#include "alns/acceptanceModule/IAcceptanceModule.h"

DummyAcceptanceModule::DummyAcceptanceModule() {
	// Nothing to be done.

}

DummyAcceptanceModule::~DummyAcceptanceModule() {
	// Nothing to be done.
}

bool DummyAcceptanceModule::transitionAccepted(IBestSolutionManager& bestSolutionManager, ISolution& currentSolution, ISolution& newSolution, ALNS_Iteration_Status& status)
{
	return newSolution.isFeasible();
}



