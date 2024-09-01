#ifndef ILOCALSEARCHMANAGER_H_
#define ILOCALSEARCHMANAGER_H_

class ISolution;
class ALNS_Iteration_Status;

class ILocalSearchManager
{
	public:

		//! \param sol the solution to be improved.
		//! \param status the status of the alns iteration.
		//! \return true if the solution has been improved.
		virtual bool useLocalSearch(ISolution& sol, ALNS_Iteration_Status& status)=0;

		//! Indicate that the optimization process starts.
		virtual void startSignal()=0;
};

#endif /* ILOCALSEARCHMANAGER_H_ */
