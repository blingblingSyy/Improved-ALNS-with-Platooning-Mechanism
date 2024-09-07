#ifndef SIMULATEDANNEALING_H_
#define SIMULATEDANNEALING_H_

#include "src/acceptanceModule/IAcceptanceModule.h"

class ISolution;
class ICoolingSchedule;
class ALNS_Iteration_Status;
class IBestSolutionManager;

/*!
 * \class SimulatedAnnealing.
 * \brief Use a simulated annealing principle to decide whether or not a new solution
 * should be accepted as the current solution.
 *
 * If you are interested by simulated annealing, please refer to:
 * Kirkpatrick, S.; Gelatt, C. D.; Vecchi, M. P. (1983). "Optimization by Simulated
 * Annealing". Science 220 (4598): 671–680
 * or
 * http://en.wikipedia.org/wiki/Simulated_annealing
 */

class SimulatedAnnealing: public IAcceptanceModule
{
	private:
		//! The cooling schedule to be use to compute the temperature each time it
		//! is needed.
		ICoolingSchedule* coolingSchedule;
		
	public:
		//! Constructor.
		//! \param cs the cooling schedule to be used by the simulated annealing.
		SimulatedAnnealing(ICoolingSchedule& cs);

		//! Destructor.
		virtual ~SimulatedAnnealing();

		//! Compute if the newly created solution have to be accepted or not
		bool transitionAccepted(IBestSolutionManager& bestSolutionManager, ISolution& currentSolution, ISolution& newSolution, ALNS_Iteration_Status& status);

		virtual void startSignal();

};

#endif /* SIMULATEDANNEALING_H_ */
