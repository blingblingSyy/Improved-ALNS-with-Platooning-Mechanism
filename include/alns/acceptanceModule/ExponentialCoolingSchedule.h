#ifndef EXPONENTIALCOOLINGSCHEDULE_H_
#define EXPONENTIALCOOLINGSCHEDULE_H_

#include <time.h>
#include "alns/acceptanceModule/ICoolingSchedule.h"

class ISolution;
class CoolingSchedule_Parameters;

/*!
 * \class ExponentialCoolingSchedule.
 * \brief An exponential cooling schedule decreases the temperature by a factor for each iteration.
 */

class ExponentialCoolingSchedule: public ICoolingSchedule
{
	public:
		//! Constructor.
		//! \param initSol the initial solution.
		//! \param csParam the cooling schedule parameters to be used.
		ExponentialCoolingSchedule(ISolution& initSol, CoolingSchedule_Parameters& csParam);

		//! Constructor.
		//! \param startingTemperature the initial temperature.
		ExponentialCoolingSchedule(double startingTemperature, double endingTemparature);

		//! Destructor.
		virtual ~ExponentialCoolingSchedule();

		//! Compute and return the current temperature.
		//! \return the current temperature.
		double getCurrentTemperature();

		//! This method is called when the optimization process start.
		virtual void startSignal();

	private:
		//! The starting temperature
		double start_temp;

		//! The ending temperature
		double end_temp;

		//! The value multiplying the temperature between each threshold.
		double decreasingFactor;

};

#endif /* EXPONENTIALCOOLINGSCHEDULE_H_ */
