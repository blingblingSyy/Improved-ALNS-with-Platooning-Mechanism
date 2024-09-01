#ifndef SIMPLELOCALSEARCHMANAGER_H_
#define SIMPLELOCALSEARCHMANAGER_H_

#include <vector>
#include "ILocalSearchManager.h"


class ILocalSearch;
class ISolution;
class ALNS_Parameters;

class SimpleLocalSearchManager: public ILocalSearchManager
{
	public:

		SimpleLocalSearchManager(ALNS_Parameters& parameters){param = &parameters;};

		virtual ~SimpleLocalSearchManager(){};

		//! \param sol the solution to be improved.
		//! \param status the status of the alns iteration.
		//! \return true if the solution has been improved.
		virtual bool useLocalSearch(ISolution& sol, ALNS_Iteration_Status& status);

		//! Add a local search operator to the manager.
		void addLocalSearchOperator(ILocalSearch& ls);

		virtual void startSignal(){};
		
	private:
		//! A vector containing the local search operators managed by the current instance.
		std::vector<ILocalSearch*> localSearchOperators;

		//! Parameters of the ALNS.
		ALNS_Parameters* param;
};

#endif /* SIMPLELOCALSEARCHMANAGER_H_ */
