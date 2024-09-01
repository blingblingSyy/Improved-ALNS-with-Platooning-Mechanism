#ifndef ICOOLINGSCHEDULE_H_
#define ICOOLINGSCHEDULE_H_

#include <iostream>

/*!
 * \class ICoolingSchedule.
 * \brief This is an interface to define cooling schedule to be used by a simulated annealing.
 * This is the template used to compute temparature.
 */

class ICoolingSchedule
{
public:
	//! \return the current temperature.
	virtual double getCurrentTemperature()=0;

	//! This method should be called when the optimization
	//! process start. The cooling schedules that actually need
	//! this should override this method.
	virtual void startSignal(){};
};


#endif /* ICOOLINGSCHEDULE_H_ */
