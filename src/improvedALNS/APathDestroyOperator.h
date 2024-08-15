#ifndef A_PATH_DESTROYOPERATOR_H_
#define A_PATH_DESTROYOPERATOR_H_

#include <vector>
#include <set>
#include "src/improvedALNS/AOperator.h"
#include "src/improvedALNS/IUpdatable.h"
using namespace std;

class ISolution;
class ALNS_Parameters;
class Nodes;
class ALNS_Iteration_Status;
class IUpdatable;

/// @brief this class is used to represent Path Destory Operators.
/// Any path destroy operator should inherit from this class and implement the destorySolPath function.
class APathDestroyOperator : public AOperator, public IUpdatable
{
	protected:
		//! The minimum destroy size (num of path) used.
		int minimunDestroy;

		//! The maximum destroy size (num of path) used.
		int maximumDestroy;

		//! The generated destroy size used.
		int pathDestroySize;

		// //! the size of available path set between pairs of nodes
		// vector<vector<int>>* availPathSetCardi;

		//! The tabu list for each path
		//! This tabu list will be updated every time a Path Destory Operator is called
		vector<vector<int>> pathTabu;  //! modify -> move to VRPSolution or PathTabu (input into the class)

		//! tabu tenure
		int tabu_tenure;

		//! calculate the actual path destroy size
		void calPathDestroySize(int destroyable_pathnum);

		//! the destroyed arc
		set<vector<int>> destroyed_arc_config;

	public:
		//! Constructor.
		//! \param availPathSize the matrix of the size of available paths between a pair of nodes.
		//! \param maxPathDestroyRate the maximum destroy size rate.
		//! \param s the name of the destroy operator.
		APathDestroyOperator(string s, ALNS_Parameters& alns_param, vector<vector<int>> availPathSetCardi);

		//! a simple constructor
		APathDestroyOperator(string s) : AOperator(s) {};
		
		//! Destructor.
		virtual ~APathDestroyOperator() {};

		//! This function is the one called to destroy a solution.
		//! \param sol the solution to be destroyed.
		virtual void destroySolPath(ISolution& sol)=0;

		//! update the operator
		void update(ISolution& sol, ALNS_Iteration_Status& status);
};

#endif
