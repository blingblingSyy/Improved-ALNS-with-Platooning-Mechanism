#ifndef PATH_TABU_H_
#define PATH_TABU_H_

#include <vector>
#include <set>
#include "src/improvedALNS/IUpdatable.h"
using namespace std;

class ISolution;
class ALNS_Parameters;
class ALNS_Iteration_Status;
class IUpdatable;

/// @brief this class is used to represent Path Destory Operators.
/// Any path destroy operator should inherit from this class and implement the destorySolPath function.
class PathTabu : public IUpdatable
{
	protected:
		// //! the size of available path set between pairs of nodes
		// vector<vector<int>> availPathSetCardi;

		//! The tabu list for each path
		//! This tabu list will be updated every time a Path Destory Operator is called
		vector<vector<int>> pathTabuList;  //! modify -> move to VRPSolution or PathTabu (input into the class)

		//! tabu tenure
		int tabu_tenure;

	public:
		//! Constructor.
        //! \param tabuTenure the tenure for each tabu path
		//! \param availPathSize the matrix of the size of available paths between a pair of nodes.
		PathTabu(int tabuTenure, vector<vector<int>> availPathCardiSet);
		
		//! Destructor.
		virtual ~PathTabu() {};

		//! update the operator for each iteration of the ALNS algorithm
		void update(ISolution& sol, ALNS_Iteration_Status& status);

        //! get tabu tenure
        int getTabuTenure() {return tabu_tenure;};

        //! refresh the tabu tenure for a specific path when a path destroy operator is called on this path
        void updatePathTenure(vector<int> input_path);

        //! get the current tabu tenure foro a given path;
        int getPathTenure(vector<int> input_path) {return pathTabuList[input_path[0]][input_path[1]];}; 

};

#endif
