#ifndef ILOCALSEARCH_H_
#define ILOCALSEARCH_H_

class ISolution;

class ILocalSearch
{
	public:
		//! Perform a local search on the solution.
		//! \return true if the solution is improved.
		virtual bool performLocalSearch(ISolution& sol)=0;

		//! \return the name of the local search operator.
		virtual std::string getName()=0;
};


#endif /* ILOCALSEARCH_H_ */
