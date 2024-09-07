#ifndef COOLINGSCHEDULE_PARAMETERS_H_
#define COOLINGSCHEDULE_PARAMETERS_H_

#include <cstring>
#include <iostream>
using namespace std;

class ALNS_Parameters;

/*!
 * \class CoolingSchedule_Parameters.
 * \brief This class represent parameters to be used to instantiate cooling schedules.
 */

class CoolingSchedule_Parameters
{
	public:

		//! Enumeration representing the various kind of cooling schedules that can be handled.
		enum CSKind
		{
			Linear_it,
			Linear_time,
			Linear_mix,
			Exponential_it,
			Exponential_time,
			Exponential_mix
		};

		//! Constructor.
		CoolingSchedule_Parameters(ALNS_Parameters& alnsParam);

		//! Default Constructor.
		CoolingSchedule_Parameters() {};

		//! Copy constructor.
		CoolingSchedule_Parameters(CoolingSchedule_Parameters& p);

		//! Destructor.
		~CoolingSchedule_Parameters();

		//! Perform some sanity checks on the values of the parameters.
		void sanityChecks();

		//! Load the parameters from a text file.
		//! \param path the path to the text file containing the parameters.
		void loadParameters(string path);

		//! Load the parameters from a text file.
		//! \param path the path to the text file containing the parameters.
		void loadXMLParameters(string path);

		//! The kind of cooling schedule to be used.
		CSKind kind;

		//! In case of the use of a mix exponential cooling schedule, the percentage of
		//! the current temperature kept at each temperature recomputation.
		//! -> decreasingFactor
		double expPercentageKept;

		//! To determine the starting temperature, how worse than the initial solution can
		//! a solution be to have a 50% chance of being accepted at the starting temperature.
		//! -> start temparature control parameter
		double setupPercentage;

		//! The number of temperature recomputations during the optimization process.
		//! if the current threshold > aimed threshold: do temparature recomputation
		size_t nbThresholds;

		//! The maximum number of iterations.
		//! used to calculate the aimed threshold in the mix exponential cooling schedule
		size_t maxIt;

		//! The maximum running time.
		//! used to calculate the ending time in the mix exponential cooling schedule
		size_t maxRT;

		//! the ending temparature
		double endingTemparature;

};


#endif /* COOLINGSCHEDULE_PARAMETERS_H_ */
