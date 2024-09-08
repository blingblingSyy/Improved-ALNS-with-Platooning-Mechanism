#ifndef A_PATH_DESTROYOPERATOR_H_
#define A_PATH_DESTROYOPERATOR_H_

#include <vector>
#include <set>
#include "alns/improvedALNS/AOperator.h"
#include "utility.h"
using namespace std;

class AOperator;
class PathTabu;
class ISolution;
// class ALNS_Parameters;

/// @brief this class is used to represent Path Destory Operators.
/// Any path destroy operator should inherit from this class and implement the destorySolPath function.
class APathDestroyOperator : public AOperator
{
	protected:
		//! path destroy rate
		double pathDesSizeRate;

		//! The tabu list for each path
		//! This tabu list will be updated every time a Path Destory Operator is called
		PathTabu* pathTabuObj;

		// //! the destroyed arc
		// set<vector<int>> destroyed_arc_config;

	public:
		//! Constructor.
		//! \param pathtabu_obj a specific object of a PathTabu class.
		//! \param pathDesRate the path destroy rate (the rate to destroy a non-tabu path)
		//! \param s the name of the destroy operator.
		APathDestroyOperator(string s, double maxPathDestroyRate, PathTabu& pathtabu_obj) : 
							pathDesSizeRate(maxPathDestroyRate), pathTabuObj(&pathtabu_obj), AOperator(s)
		{
				// destroyed_arc_config.clear();
		}

		//! a simple constructor
		APathDestroyOperator(string s) : AOperator(s) {};
		
		//! Destructor.
		virtual ~APathDestroyOperator() {};

		//! This function is the one called to destroy a solution.
		//! \param sol the solution to be destroyed.
		virtual void destroySolPath(ISolution& sol)=0;
};

#endif
