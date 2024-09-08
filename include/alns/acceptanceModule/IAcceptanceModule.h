#ifndef IACCEPTANCEMODULE_H_
#define IACCEPTANCEMODULE_H_

#include <iostream>
#include "alns/improvedALNS/ISolution.h"
#include "alns/improvedALNS/ALNS_Iteration_Status.h"
#include "alns/improvedALNS/IBestSolutionManager.h"

/*!
 * \class IAcceptanceModule.
 * \brief This is an interface to define acceptance modules within the ALNS.
 */

class IAcceptanceModule
{
public:
	//! Indicate if the new created solution have to be accepted or not
	//! \param bestSolutionManager a reference to the best solution manager.
	//! \param currentSolution current solution.
	//! \param newSolution new solution.
	//! \param status the status of the current alns iteration.
	//! \return true if the transition is accepted, false otherwise.
	virtual bool transitionAccepted(IBestSolutionManager& bestSolutionManager, ISolution& currentSolution, ISolution& newSolution, ALNS_Iteration_Status& status) = 0;

	//! Some Acceptance modules needs to initialize some variable
	//! only when the solver actually starts working. In this case
	//! you should override this method.
	virtual void startSignal(){}; 
};


#endif /* ACCEPTANCEMODULE_H_ */
