#ifndef A_NODE_DESTROYOPERATOR_H_
#define A_NODE_DESTROYOPERATOR_H_

#include "src/alns/AOperator.h"
#include "src/alns/ALNS_Parameters.h"
using namespace std;

class ISolution;
class ALNS_Parameters;

/// @brief this is an abstract class used to represetnt Node Destory Operators.
/// Any Node Destroy operator should inherit from this class and implement the destroySolNode Function
class ANodeDestroyOperator : public AOperator 
{
	protected:
		//! The minimum destroy size used.
		size_t minimunDestroy;

		//! The maximum destroy size used.
		size_t maximumDestroy;

		//! The generated destroy size used.
		size_t nodeDestroySize;

	public:
		//! Constructor.
		//! \param nodenum the total number of customers.
		//! \param maxNodeDestroyRate the maximum destroy size rate.
		//! \param s the name of the destroy operator.
		ANodeDestroyOperator(int cusnum, double maxNodeDestroyRate, string s) : AOperator(s)
		{
			minimunDestroy = 1;
			maximumDestroy = maxNodeDestroyRate * cusnum;
		}

		//! Destructor.
		virtual ~ANodeDestroyOperator(){};

		//! This function is the one called to destroy a solution.
		//! \param sol the solution to be destroyed.
		virtual void destroySolNode(ISolution& sol)=0;
};

#endif
