#ifndef MIXEXPONENTIALCOOLINGSCHEDULE_H_
#define MIXEXPONENTIALCOOLINGSCHEDULE_H_

#include <time.h>
#include "src/acceptanceModule/ICoolingSchedule.h"

class ISolution;
class CoolingSchedule_Parameters;

/*!
 * \class ExponentialCoolingSchedule.
 * \brief An exponential cooling schedule based on a mix of the maximum running time
 * and the number of iterations.
 */

class MixExponentialCoolingSchedule: public ICoolingSchedule
{
	public:
		//! Constructor.
		//! \param initSol the initial solution.
		//! \param csParam the cooling schedule parameters to be used.
		MixExponentialCoolingSchedule(ISolution& initSol, CoolingSchedule_Parameters& csParam);

		//! Destructor.
		virtual ~MixExponentialCoolingSchedule();

		//! Compute and return the current temperature.
		//! \return the current temperature.
		double getCurrentTemperature();

		//! This method is called when the optimization process start.
		virtual void startSignal();
		
	private:
		//! The current iteration
		long long currentIt;

		//! The maximum iteration
		long long maximumIt;

		//! The time at which the algorithm started.
		clock_t startingTime;

		//! The time at which the algorithm should end.
		clock_t endingTime;

		//! The current threshold.
		int currentThreshold;

		//! The number of thresholds.
		int nbThresholds;

		//! The current temperature.
		double currentTemperature;

		//! The value multiplying the temperature between each threshold.
		double decreasingFactor;

		double runTime;
};

#endif /* EXPONENTIALCOOLINGSCHEDULE_H_ */
