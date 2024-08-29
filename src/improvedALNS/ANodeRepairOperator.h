#ifndef A_NODE_REPAIROPERATOR_H_
#define A_NODE_REPAIROPERATOR_H_

#include <iostream>
#include <set>
#include <tuple>
#include "AOperator.h"
using namespace std;

class ISolution;
class ALNS_Iteration_Status;

/// @brief this abstract class represents a Node Repair Operator, all repair operator implementations should inherit from this class.
class ANodeRepairOperator : public AOperator 
{
	public:
		ANodeRepairOperator(string s) : AOperator(s) {};

		virtual ~ANodeRepairOperator(){};

		virtual void repairSolNode(ISolution& sol)=0;

		// //! get whether the operator needs update or not
		// bool getNeedUpate() {return needUpdate;};
		
	protected:
		//! The set of destroyed nodes <routeid, arcpos>
		vector<pair<int, int>> repaired_nodeset;

		// //! the set of node positions that cannot be destroyed
		// set<pair<int, int>> forbidden_repaired_nodepos;

		// //! check whether the operator needs update or not
		// bool needUpdate;

		// //! the set of already destroyed arcs
		// vector<tuple<int, int, int>> destroyed_arcpos;

        /*old version */
		// //! update the operator
		// void update(ISolution& sol, ALNS_Iteration_Status& status)
		// {
        //     for(int i = 0; i < destroyed_arcpos.size(); i++)
        //     {
        //         pair<int, int> forbidden_pos = make_pair(get<2>(destroyed_arcpos[i])+1, get<0>(destroyed_arcpos[i]));
        //         forbidden_repaired_nodepos.insert(forbidden_pos);
        //     }
		// }
};

#endif /* AREPAIROPERATOR_H_ */
