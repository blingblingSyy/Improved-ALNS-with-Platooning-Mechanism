#include "couplingVRP/operators/nodeRepairOps/Node_Regret_Insert.h"
#include "couplingVRP/model/establish/VRPSolution.h"
#include "couplingVRP/model/establish/ARoute.h"
#include "src/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
#include <algorithm>
using namespace std;

Node_Regret_Insert::Node_Regret_Insert(string s, int regret_k) : Node_Random_Insert(s)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
    this->K = regret_k;
}

Node_Regret_Insert::~Node_Regret_Insert()
{

}

void Node_Regret_Insert::repairSolNode(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    vector<int> all_non_inserted = vrpsol.getNonInsertedNodes();
    initNonRepairablePos(vrpsol);  //! initialize nonRepairablePos
    // assert(nonRepairablePos.size() < vrpsol.getAllCusPos().size());  //! nonRepariablePos must be smaller than all_cus_pos;
    while(!all_non_inserted.empty())
    {
        vector<tuple<double, int, int>> mincost_noninserted;
        vector<double> regretval_noninserted;
        for(int i = 0; i < all_non_inserted.size(); i++)
        {
            vector<tuple<double, int, int>> ordered_insert_costs = vrpsol.calOrderedInsertCostsAllRoutes(all_non_inserted[i]);
            //! insert to the first available position
            for(int s = 0; s < ordered_insert_costs.size(); s++)
            {
                double selected_cost_i = get<0>(ordered_insert_costs[s]);
                int selected_rid_i = get<2>(ordered_insert_costs[s]);
                int selected_pos_i = get<1>(ordered_insert_costs[s]);
                auto it = find(nonRepairablePos.begin(), nonRepairablePos.end(), make_pair(selected_rid_i, selected_pos_i));
                if(it == nonRepairablePos.end()) //! if the insertion position is not at the modified arcpos
                {
                    mincost_noninserted.push_back(ordered_insert_costs[s]);
                    break;
                }
            }
            vector<double> regret_val_i; //! take the average regret value
            for(int k = 1; k < K; k++)  //int k = pos+1; k < pos+K; k++
            {
                regret_val_i.push_back(get<0>(ordered_insert_costs[k]) - get<0>(ordered_insert_costs[0]));
            }
            regretval_noninserted.push_back(calAverage(regret_val_i));
        }
        auto it = min_element(regretval_noninserted.begin(), regretval_noninserted.end());
        int selected_node = all_non_inserted[it - regretval_noninserted.begin()];
        tuple<double, int, int> selected_vehpos = mincost_noninserted[it - regretval_noninserted.begin()];
        if(get<0>(selected_vehpos) < INF)
        {
            int insert_rid = get<2>(selected_vehpos);
            int insert_pos = get<1>(selected_vehpos);
            vrpsol.getOneRoute(insert_rid)->setRouteByInsertNode(insert_pos, selected_node);
            vrpsol.getNonInsertedNodes().erase(remove(vrpsol.getNonInsertedNodes().begin(), vrpsol.getNonInsertedNodes().end(), selected_node));
            vrpsol.getTotalDistBeforePlatooning() += get<0>(selected_vehpos);
            vrpsol.getTotalUnservedRequests() -= 1;
            vrpsol.setSolCurOperation(VRPSolution::InsertOneNode);
            updateNonRepairablePos(insert_rid, insert_pos);
            all_non_inserted.erase(all_non_inserted.begin());
        }
        else //! if the mincost >= INF, no insertion will occur, can end the algorithm.
        {
            all_non_inserted.clear();
            break;
        }
    }
}



/*old version*/
// void Node_Regret_Insert::repairSolNode(ISolution& sol)
// {
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     vector<int> all_non_inserted = vrpsol.getNonInsertedNodes();
//     while(!all_non_inserted.empty())
//     {
//         vector<tuple<double, int, int>> mincost_noninserted;
//         vector<double> regretval_noninserted;
//         for(int i = 0; i < all_non_inserted.size(); i++)
//         {
//             vector<tuple<double, int, int>> ordered_insert_costs = vrpsol.calOrderedInsertCostsAllRoutes(all_non_inserted[i]);
//             mincost_noninserted.push_back(ordered_insert_costs[0]);
//             double regret_val_i;
//             for(int k = 1; k < K; k++)
//             {
//                 regret_val_i += get<0>(ordered_insert_costs[k]) - get<0>(ordered_insert_costs[0]);
//             }
//             regretval_noninserted.push_back(regret_val_i);
//         }
//         auto it = min_element(regretval_noninserted.begin(), regretval_noninserted.end());
//         int selected_node = all_non_inserted[it - regretval_noninserted.begin()];
//         tuple<double, int, int> selected_vehpos = mincost_noninserted[it - regretval_noninserted.begin()];
//         if(get<0>(selected_vehpos) < INF)
//         {
//             int insert_rid = get<2>(selected_vehpos);
//             int insert_pos = get<1>(selected_vehpos);
//             vrpsol.getOneRoute(insert_rid)->setRouteByInsertNode(insert_pos, selected_node);
//             vrpsol.getNonInsertedNodes().erase(remove(vrpsol.getNonInsertedNodes().begin(), vrpsol.getNonInsertedNodes().end(), selected_node));
//             vrpsol.getTotalDistBeforePlatooning() += get<0>(selected_vehpos);
//             vrpsol.getTotalUnservedRequests() -= 1;
//             vrpsol.setSolCurOperation(VRPSolution::InsertOneNode);
//         }
//         all_non_inserted.erase(all_non_inserted.begin());
//     }
//     vrpsol.getDestroyedArcsPos().clear();
// }


// void Node_Regret_Insert::update(ISolution& sol, ALNS_Iteration_Status& status)
// {
//     if(hasSelectedCur && needUpdate)
//     {
//         forbidden_repaired_nodepos.clear();
//         VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//         vector<tuple<int, int, int>> destroyed_arcpos = vrpsol.getDestroyedArcsPos();
//         for(int i = 0; i < destroyed_arcpos.size(); i++)
//         {
//             pair<int, int> forbidden_pos = make_pair(get<2>(destroyed_arcpos[i])+1, get<0>(destroyed_arcpos[i]));
//             forbidden_repaired_nodepos.insert(forbidden_pos);
//         }        
//     }
// }