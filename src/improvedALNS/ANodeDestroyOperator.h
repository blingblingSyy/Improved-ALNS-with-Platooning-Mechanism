#ifndef A_NODE_DESTROYOPERATOR_H_
#define A_NODE_DESTROYOPERATOR_H_

#include <algorithm>
#include <tuple>
#include <set>
#include "src/improvedALNS/AOperator.h"
#include "src/improvedALNS/ALNS_Parameters.h"
#include "utility.h"
using namespace std;

class ISolution;
class ALNS_Parameters;
class ALNS_Iteration_Status;
class IUpdatable;

/// @brief this is an abstract class used to represetnt Node Destory Operators.
/// Any Node Destroy operator should inherit from this class and implement the destroySolNode Function
class ANodeDestroyOperator : public AOperator
{
	protected:
		//! The minimum destroy size used.
		int minimunDestroy;

		//! The maximum destroy size used.
		int maximumDestroy;

		//! The actual node destroy size
		int nodeDestroySize;

		//! This function is to get the actual node destroy size based on the solution size
		void calNodeDestroySize(int served_num)
		{
			RandomNumber r;
			nodeDestroySize = r.get_rint(min(minimunDestroy, served_num), min(served_num, maximumDestroy));
		}

		//! The set of destroyed nodes <routeid, arcpos>
		vector<pair<int, int>> destroyed_nodeset;

		// //! The tabu operator for each node operator
		// NodeTabu* nodeTabuObj;

		// //! the set of node positions that cannot be destroyed
		// set<pair<int, int>> forbidden_destroyed_nodepos;
		
		// //! check whether the operator needs update or not
		// bool needUpdate;

        // //! find the forbidden destroyed node positions by the set of already destroyed arcs
        // void findForbiddenDesNodePos(vector<tuple<int, int, int>> destroyed_arcpos);

	public:
		//! Constructor.
		//! \param nodenum the total number of customers.
		//! \param maxNodeDestroyRate the maximum destroy size rate.
		//! \param s the name of the destroy operator.
		ANodeDestroyOperator(string s, ALNS_Parameters& alns_param, int cus_num) : AOperator(s)
		{
			minimunDestroy = 1; 
			maximumDestroy = alns_param.getNodeDestroyRate() * cus_num;
			destroyed_nodeset.clear();
		}

		//! Another constructor
		ANodeDestroyOperator(string s) : AOperator(s) {};

		//! Destructor.
		virtual ~ANodeDestroyOperator() {destroyed_nodeset.clear();};

		//! This function is the one called to destroy a solution.
		//! \param sol the solution to be destroyed.
		//! if a node removal is infeasible, the node will not be removed
		virtual void destroySolNode(ISolution& sol)=0;

		// //! a simple getter
		// vector<pair<int, int>> getDestroyedNodes() {return destroyed_nodeset;};

		// //! update the operator
		// void update(ISolution& sol, ALNS_Iteration_Status& status);

		//! get whether the operator needs update or not
		// bool getNeedUpate() {return needUpdate;};

};

#endif


/* old version */
// void ANodeDestroyOperator::findForbiddenDesNodePos(ISolution& sol, ALNS_Iteration_Status& status)
// {
// 	for(int i = 0; i < destroyed_arcpos.size(); i++)
// 	{
// 		pair<int, int> forbidden_pos1 = make_pair(get<2>(destroyed_arcpos[i]), get<0>(destroyed_arcpos[i]));
// 		pair<int, int> forbidden_pos2 = make_pair(get<2>(destroyed_arcpos[i])+1, get<0>(destroyed_arcpos[i]));
// 		forbidden_destroyed_nodepos.insert(forbidden_pos1);
// 		forbidden_destroyed_nodepos.insert(forbidden_pos2);
// 	}
// }