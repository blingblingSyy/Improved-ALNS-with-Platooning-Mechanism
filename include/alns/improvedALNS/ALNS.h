#ifndef ALNS_H_
#define ALNS_H_

#include <cstring>
#include <math.h>
#include <time.h>
#include <set>
#include <vector>
#include "alns/statistics/Statistics.h"
#include "alns/improvedALNS/ALNS_Iteration_Status.h"
#include "alns/improvedALNS/IBestSolutionManager.h"
using namespace std;

class IAcceptanceModule;
class ISolution;
class IUpdatable;
class Statistics;
class IBestSolutionManager;
class ILocalSearchManager;
class AStrategy;
class AOperator;
class AStrategyManager;
class ALNS_Parameters;
class ALNS_Iteration_Status;
class ANodeDestroyOperator;
class ANodeRepairOperator;
class APathDestroyOperator;
class APathRepairOperator;

/*!
 * \class ALNS.
 * \brief This class contains the ALNS logic.
 *
 * This class contains the logic of the improved ALNS (Improved Adaptive Large Neighborhood Search).
 * The general idea of the ALNS is to iteratively destroy then repair a solution on nodes or paths
 * to improve its quality. Non improving solution may be accepted as the new current
 * solution according to some acceptance criteria.
 */

class ALNS
{
	private:

		//! The current solution.
		ISolution* currentSolution;

		//! The acceptance criteria to be used.
		IAcceptanceModule* acceptanceCriterion;

		//! The parameters to be used.
		ALNS_Parameters* param;

		//! Manager of the operators.
		AStrategyManager* stManager;

		//! Manager of the best solutions.
		IBestSolutionManager* bestSolManager;

		//! Manager of the local search operators.
		ILocalSearchManager* lsManager;

		//! The number of iterations since last weight recomputation.
		size_t nbIterationsWC;

		//! The current number of iterations.
		size_t nbIterations;

		//! The current number of iterations without improvement.
		size_t nbIterationsWithoutImprovement;

		//! The number of iterations without improvement of the current solution.
		size_t nbIterationsWithoutImprovementCurrent;

		//! The number of iterations without acceptation of a transition.
		size_t nbIterationsWithoutTransition;

		//! The number of iterations since the last call to a local search operator.
		size_t nbIterationsWithoutLocalSearch;

		//! The time the optimization process started.
		clock_t startingTime;

		//! A lower bound on the optimum solution cost.
		double lowerBound;

		//! A set containing the hash keys of the encountred solutions.
		set<long long> knownKeys;

		//! An object to compute some statistics about the solving process.
		Statistics stats;

		//! An object representing the status of the last iteration.
		ALNS_Iteration_Status status;

		//! A list of object that we need to update at the end of each iteration.
		vector<IUpdatable*> updatableStructures;

		//! the name of the input instance
		string name;

		//! cpu time for the overall algorithm
		double cpu;

	public:
		//! Constructor.
		//! \param name the name of the instance.
		//! \param initialSolution the starting solution that is going to be optimized.
		//! \param acceptanceCrit the module that determine whether or not a new solution
		//! is accepted as the current solution.
		//! \param parameters the set of parameters to be use by the ALNS.
		//! \param stMan an strategy manager.
		ALNS(std::string instanceName,
			ISolution& initialSolution,
			IAcceptanceModule& acceptanceCrit,
			ALNS_Parameters& parameters,
			AStrategyManager& stMan,
			IBestSolutionManager& solMan,
			ILocalSearchManager& lsMan);

		//! Destructor.
		virtual ~ALNS();

		//! This method launch the solving process.
		//! \return true if a feasible solution is found,
		//! false otherwise.
		bool solve();

		//! This method seeks if a solution is already known,
		//! if not it is added to the set of known solutions.
		//! \param sol the solution to be checked.
		//! \return true if the solution was unknown, false otherwise.
		bool checkAgainstKnownSolution(ISolution& sol);

		//! This method perform one iteration of the ALNS solving process.
		void performOneIteration();

		//! This method check whether or not the stopping criteria is met.
		bool isStoppingCriterionMet();

		//! Determine whether or not the new solution is better than the
		//! best known solution.
		bool isNewBest(ISolution* newSol);

		//! \return the number of known solutions.
		size_t getNumberKnownSolutions(){return knownKeys.size();};

		//! Determine whether or not the new solution should be accepted
		//! as the current solution.
		bool transitionCurrentSolution(ISolution* newSol);

		//! Return a pointer to the best solution manager.
		IBestSolutionManager* getBestSolutionManager(){return bestSolManager;};

		//! Return a pointer to the best known solution.
		ISolution* getBestSolution(){return *(bestSolManager->begin());};

		//! Add an object to the list of object to be updated at the end of each
		//! iteration of the ALNS.
		//! \param up the updatable object to be added.
		void addUpdatable(IUpdatable& up){updatableStructures.push_back(&up);};

		//! Destroy the manager that have been provided at the construction of
		//! the instance.
		void end();

		//! get cpu time
		double getCpuTime();
};

#endif
