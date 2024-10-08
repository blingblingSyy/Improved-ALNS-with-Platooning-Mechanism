#ifndef ILOCALSEARCH_H_
#define ILOCALSEARCH_H_

#include <string> 

class ISolution;

class ILocalSearch
{
	public:
		//! Perform a local search on the solution.
		//! \return true if the solution is improved.
		//! remember to make platoons for the solution after doing the local search
		virtual bool performLocalSearch(ISolution& sol)=0;

		//! \return the name of the local search operator.
		virtual std::string getName()=0;
};


#endif /* ILOCALSEARCH_H_ */
