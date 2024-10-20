#ifndef NODE_REGRET_INSERT_H_
#define NODE_REGRET_INSERT_H_

// #include "ALNS_inc.h"
#include "example/operators/nodeRepairOps/Node_Random_Insert.h"
using namespace std;

class ISolution;
// class ANodeRepairperator;
class Node_Random_Insert;

class Node_Regret_Insert: public Node_Random_Insert
{
    public:
        //! constructor
        Node_Regret_Insert(string s, int regret_k);

        //! destructor
        virtual ~Node_Regret_Insert();

        //! destroy operator
        void repairSolNode(ISolution& sol);
    
    private:
        //! the step size for regret heuristic
        int regret_k;

        //! calculate the ordered insertion costs of all non-inserted nodes to a solution
        vector<vector<tuple<double, int, int>>> calNoisedOrderedInsertCostsAllNodes(VRPSolution& vrpsol, vector<int> all_non_inserted);

        //! calculate the regret value of all non-inserted nodes
        vector<double> calRegretValAllNodes(vector<int> all_non_inserted, vector<vector<tuple<double, int, int>>> orderedCostsAllNodes);

        //! determine the first feasible position of all nodes to be inserted to a solution 
        vector<tuple<double, int, int>> findFeasPosAllNodes(VRPSolution& vrpsol, vector<int> all_non_inserted, vector<vector<tuple<double, int, int>>> orderedCostsAllNodes);
};

#endif
