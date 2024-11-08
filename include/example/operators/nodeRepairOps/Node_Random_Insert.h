#ifndef NODE_RANDOM_INSERT_H_
#define NODE_RANDOM_INSERT_H_

// #include "ALNS_inc.h"
#include "alns/improvedALNS/ANodeRepairOperator.h"
#include <vector>
using namespace std;

class ISolution;
class ANodeRepairperator;
class VRPSolution;

class Node_Random_Insert: public ANodeRepairOperator
{
    public:
        //! constructor
        Node_Random_Insert(string s);

        //! destructor
        virtual ~Node_Random_Insert();

        //! repair operator
        void repairSolNode(ISolution& sol);

    protected:
        //! non-repairable node positions <rid, pos> -> type-specific
        vector<vector<pair<int, int>>> nonRepairablePos; 
        
        //! update the forbidden destroyed node positions
        void keepRepairablePos(VRPSolution& vrpsol, vector<pair<int, int>>& all_cus_pos_copy, int type);
        // vector<int> nonRepairableNodes(VRPSolution& vrpsol);

        //! initialize the non-repairable positions
        void initNonRepairablePos(VRPSolution& vrpsol);

        //! update the non-repairable position once an insertion occurs
        void updateNonRepairablePos(VRPSolution& vrpsol, int insert_rid, int insert_pos);
        
        //! update all customer positions once an insertion occurs
        void updateAllCusPos(VRPSolution& vrpsol, vector<pair<int, int>>& all_cus_pos, pair<int, int> insert_vehpos);
};

#endif
