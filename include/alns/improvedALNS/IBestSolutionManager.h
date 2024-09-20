#ifndef IBESTSOLUTIONMANAGER_H_
#define IBESTSOLUTIONMANAGER_H_

#include <list>

class ISolution;
class ALNS_Iteration_Status;

/// @brief this virtual class defines the framework of managing a best solution
class IBestSolutionManager
{
	public:
		//! This method evaluate if a solution is a new best solution, and adds it to the
		//! best solution pool in this case.
		//! \param sol the solution to be tested.
		//! \return true if the solution is a new best solution, false otherwise.
		virtual bool isNewBestSolution(ISolution& sol)=0;

		//! Return a pointer to a best solution.
		virtual std::list<ISolution*>::iterator begin()=0;

		//! Return a pointer to a best solution.
		virtual std::list<ISolution*>::iterator end()=0;

		//! This function take care of reloading the best known
		//! solution, as the current solution, if needed.
		//! \param currSol a pointer to the current solution.
		//! \param status the status of the current iteration.
		//! \return a pointer to the current solution.
		virtual ISolution* reloadBestSolution(ISolution* currSol, ALNS_Iteration_Status& status)=0;
};


#endif /* IBESTSOLUTIONMANAGER_H_ */
