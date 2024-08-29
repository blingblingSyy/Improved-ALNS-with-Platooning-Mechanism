#ifndef NODE_TABU_H_
#define NODE_TABU_H_

#include <vector>
#include <set>
#include <tuple>
#include "src/improvedALNS/IUpdatable.h"
using namespace std;

class ISolution;
class ALNS_Iteration_Status;
class IUpdatable;
class VRPSolution;

/// @brief this class is used to represent Node Tabu Operator.
//! Node Tabu Operator restricts that certain node positions cannot be removed or inserted due to the modified arc positions.
class NodeTabu : public IUpdatable
{
	public:
		//! Constructor.
		NodeTabu() {};
		
		//! Destructor.
		virtual ~NodeTabu() {};

		//! update the operator for each iteration of the ALNS algorithm
		void update(ISolution& sol, ALNS_Iteration_Status& status);

        //! keep the removable positions
        /*old version*/
        // vector<vector<int>> findRemovablePos(VRPSolution& vrpsol, vector<vector<int>> all_cus_pos);
        /*new version*/
        void keepRemovablePos(VRPSolution& vrpsol, vector<pair<int, int>>& all_cus_pos);

        //! keep the repairable positions
        void keepRepairablePos(VRPSolution& vrpsol, vector<pair<int, int>>& all_cus_pos);

};

#endif
