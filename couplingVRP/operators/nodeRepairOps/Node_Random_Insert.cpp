#include "couplingVRP/operators/nodeRepairOps/Node_Random_Insert.h"
#include "couplingVRP/model/establish/VRPSolution.h"
#include "couplingVRP/model/establish/ARoute.h"
#include "src/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
using namespace std;

Node_Random_Insert::Node_Random_Insert(string s) : ANodeRepairOperator(s)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
    needUpdate = true;
}

Node_Random_Insert::~Node_Random_Insert()
{

}

/* old version */
// void Node_Random_Insert::repairSolNode(ISolution& sol)
// {
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     vector<pair<int, int>> all_cus_pos = vrpsol.getAllCusPos();
//     vector<int> all_non_inserted = vrpsol.getNonInsertedNodes();
//     while(!all_non_inserted.empty())
//     {
//         vector<pair<int, int>> cus_pos_copy = all_cus_pos;
//         while(!cus_pos_copy.empty())
//         {
//             RandomNumber r1;
//             int p = r1.get_rint(0, cus_pos_copy.size()-1);
//             pair<int, int> selected_vehpos = cus_pos_copy[p];
//             vrpsol.insertNode(selected_vehpos.second, all_non_inserted.front(), selected_vehpos.first);
//             if(vrpsol.getSolCurOperation() == ARoute::InsertOneNode)
//             {
//                 vector<int> inserted_compact_route = vrpsol.getOneRoute(selected_vehpos.first)->getCompactRoute();
//                 all_cus_pos.push_back(make_pair(selected_vehpos.first, inserted_compact_route.size()-2));
//                 break;
//             }
//             cus_pos_copy.erase(cus_pos_copy.begin()+p);
//         }
//         all_non_inserted.erase(all_non_inserted.begin());
//     }
// }

/* new version */
void Node_Random_Insert::repairSolNode(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    vector<int> all_non_inserted = vrpsol.getNonInsertedNodes();
    while(!all_non_inserted.empty())
    {
        vector<pair<int, int>> cus_pos_copy = vrpsol.getAllCusPos();
        while(!cus_pos_copy.empty())
        {
            RandomNumber r1;
            int p = r1.get_rint(0, cus_pos_copy.size()-1);
            pair<int, int> selected_vehpos = cus_pos_copy[p];
            vrpsol.insertNode(selected_vehpos.second, all_non_inserted.front(), selected_vehpos.first);
            if(vrpsol.getSolCurOperation() == ARoute::InsertOneNode)
            {
                break;
            }
            cus_pos_copy.erase(cus_pos_copy.begin()+p);
        }
        all_non_inserted.erase(all_non_inserted.begin());
    }
}

void Node_Random_Insert::update(ISolution& sol, ALNS_Iteration_Status& status)
{
    if(hasSelectedCur && needUpdate)
    {
        forbidden_repaired_nodepos.clear();
        VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
        vector<tuple<int, int, int>> destroyed_arcpos = vrpsol.getDestroyedArcsPos();
        for(int i = 0; i < destroyed_arcpos.size(); i++)
        {
            pair<int, int> forbidden_pos = make_pair(get<2>(destroyed_arcpos[i])+1, get<0>(destroyed_arcpos[i]));
            forbidden_repaired_nodepos.insert(forbidden_pos);
        }        
    }
}