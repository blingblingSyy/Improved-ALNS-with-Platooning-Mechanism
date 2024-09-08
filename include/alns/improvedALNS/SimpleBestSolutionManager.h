#ifndef SIMPLEBESTSOLUTIONMANAGER_H_
#define SIMPLEBESTSOLUTIONMANAGER_H_

#include <list>
#include "alns/improvedALNS/IBestSolutionManager.h"
using namespace std;

class ISolution;
class ALNS_Iteration_Status;
class ALNS_Parameters;

/// @brief this class implements a simple manager for the best solution of the ALNS algorithm
class SimpleBestSolutionManager: public IBestSolutionManager
{
	public:
		SimpleBestSolutionManager(ALNS_Parameters& param);

		virtual ~SimpleBestSolutionManager();

		virtual bool isNewBestSolution(ISolution& sol);

		//! Return a pointer to a best solution.
		list<ISolution*>::iterator begin(){return bestSols.begin();};

		//! Return a pointer to a best solution.
		list<ISolution*>::iterator end(){return bestSols.end();};

		//! This function take care of reloading the best known
		//! solution, as the current solution, if needed.
		//! \param currSol a pointer to the current solution.
		//! \param status the status of the current iteration.
		//! \return a pointer to the current solution.
		virtual ISolution* reloadBestSolution(ISolution* currSol, ALNS_Iteration_Status& status);

		//! Simple getter.
		list<ISolution*>& getBestSols(){return bestSols;};

	private:
		list<ISolution*> bestSols;

		ALNS_Parameters* parameters;

};

#endif /* SIMPLEBESTSOLUTIONMANAGER_H_ */
