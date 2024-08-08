#ifndef A_PATH_DESTROYOPERATOR_H_
#define A_PATH_DESTROYOPERATOR_H_

#include <vector>
#include "src/alns/AOperator.h"
#include "src/alns/ALNS_Parameters.h"
using namespace std;

class ISolution;

/// @brief this class is used to represent Path Destory Operators.
/// Any path destroy operator should inherit from this class and implement the destorySolPath function.
class APathDestroyOperator : public AOperator
{
	protected:
		//! The minimum destroy size (num of path) used.
		size_t minimunDestroy;

		//! The maximum destroy size (num of path) used.
		size_t maximumDestroy;

		//! The generated destroy size used.
		size_t pathDestroySize;

		//! The tabu list for each path
		//! This tabu list will be updated every time a Path Destory Operator is called
		vector<vector<int>> pathTabu;

		//! the vector to store the destroyed paths
		vector<vector<int>> destroyedPaths; //! {{destroyed_routeid, destroyed_arcpos, destroyed_pathid}, ...};

	public:
		//! Constructor.
		//! \param availPathSize the matrix of the size of available paths between a pair of nodes.
		//! \param maxPathDestroyRate the maximum destroy size rate.
		//! \param s the name of the destroy operator.
		APathDestroyOperator(vector<vector<int>>& availPathSize, double maxPathDestroyRate, string s);

		//! Destructor.
		virtual ~APathDestroyOperator() {};

		//! This function is the one called to destroy a solution.
		//! \param sol the solution to be destroyed.
		virtual void destroySolPath(ISolution& sol)=0;
};

#endif
