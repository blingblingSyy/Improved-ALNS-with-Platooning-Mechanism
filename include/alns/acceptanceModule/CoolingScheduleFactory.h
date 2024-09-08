#ifndef COOLINGSCHEDULEFACTORY_H_
#define COOLINGSCHEDULEFACTORY_H_

class ISolution;
class ICoolingSchedule;
class CoolingSchedule_Parameters;

/*!
 * \class CoolingScheduleFactory.
 * \brief This class instantiate cooling schedules.
 *
 */

class CoolingScheduleFactory
{
	public:
		//! Generate a cooling schedule using the initial solution and the cooling schedule parameters.
		//! \param sol the initial solution.
		//! \param param the parameters to be used to generate the cooling schedule.
		//! \return a pointer to a cooling schedule.
		//! make the cooling schedule given the specified cooling schedule
		static ICoolingSchedule* makeCoolingSchedule(ISolution& sol, CoolingSchedule_Parameters& param);
};

#endif /* COOLINGSCHEDULEFACTORY_H_ */
