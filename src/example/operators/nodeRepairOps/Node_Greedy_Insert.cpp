#include "example/operators/nodeRepairOps/Node_Greedy_Insert.h"
#include "example/model/establish/VRPSolution.h"
#include "example/model/establish/ARoute.h"
#include "example/operators/nodeRepairOps/Node_Regret_Insert.h"
#include "alns/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
using namespace std;

Node_Greedy_Insert::Node_Greedy_Insert(string s) : Node_Regret_Insert(s, 1)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
}

Node_Greedy_Insert::~Node_Greedy_Insert()
{

}


/*old version*/
// void Node_Greedy_Insert::repairSolNode(ISolution& sol)
// {
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     vector<int> all_non_inserted = vrpsol.getNonInsertedNodes();
//     initNonRepairablePos(vrpsol);  //! initialize nonRepairablePos
//     // assert(nonRepairablePos.size() < vrpsol.getAllCusPos().size());  //! nonRepariablePos must be smaller than all_cus_pos;
//     while(!all_non_inserted.empty())
//     {
//         vector<tuple<double, int, int>> mincost_noninserted;
//         //! find all mincost insertion for all nodes
//         for(auto it = all_non_inserted.begin(); it != all_non_inserted.end(); it++)  //int i = 0; i < all_non_inserted.size(); i++
//         {
//             vector<tuple<double, int, int>> ordered_insertion = vrpsol.calOrderedInsertCostsAllRoutes(*it);
//             // if(noise)
//             // {
//             //     RandomNumber r;
//             //     for(auto it = ordered_insertion.begin(); it != ordered_insertion.end(); it++)
//             //     {
//             //         double cost = get<0>(*it) * r.get_rflt();
//             //         get<0>(*it) = cost;
//             //     }
//             //     sort(ordered_insertion.begin(), ordered_insertion.end(), [&](auto A, auto B){return get<0>(A) < get<0>(B);});
//             // }
//             // bool canInsert = false;
//             for(int s = 0; s < ordered_insertion.size(); s++)
//             {
//                 double selected_cost_i = get<0>(ordered_insertion[s]);
//                 int selected_rid_i = get<2>(ordered_insertion[s]);
//                 int selected_pos_i = get<1>(ordered_insertion[s]);
//                 auto it = find(nonRepairablePos.begin(), nonRepairablePos.end(), make_pair(selected_rid_i, selected_pos_i));
//                 if(it == nonRepairablePos.end()) //! if the insertion position is not at the modified arcpos
//                 {
//                     mincost_noninserted.push_back(ordered_insertion[s]);
//                     // canInsert = true;
//                     break;
//                 }
//             }
//             // if(!canInsert) //! this means that all positions are forbidden for this node
//             // {
//             //     it = all_non_inserted.erase(it);
//             //     continue;
//             // }
//         }
//         auto it = min_element(mincost_noninserted.begin(), mincost_noninserted.end(), [&](auto A, auto B){return A < B;});  //get<0>(A) < get<0>(B)
//         int selected_node = all_non_inserted[it - mincost_noninserted.begin()];
//         if(get<0>(*it) < INF)
//         {
//             int insert_rid = get<2>(*it);
//             int insert_pos = get<1>(*it);
//             // vrpsol.insertNode(insert_pos, selected_node, insert_rid);
//             vrpsol.getOneRoute(insert_rid)->setRouteByInsertNode(insert_pos, selected_node);
//             vrpsol.getNonInsertedNodes().erase(remove(vrpsol.getNonInsertedNodes().begin(), vrpsol.getNonInsertedNodes().end(), selected_node));
//             vrpsol.getTotalDistBeforePlatooning() += get<0>(*it);
//             // vrpsol.getTotalUnservedRequests() -= 1;
//             vrpsol.setSolCurOperation(VRPSolution::InsertOneNode);
//             updateNonRepairablePos(insert_rid, insert_pos);
//             all_non_inserted.erase(all_non_inserted.begin());
//         }
//         else //! if the mincost >= INF, no insertion will occur, can end the algorithm.
//         {
//             all_non_inserted.clear();
//             break;
//         }
//     }
// }


/*old version*/
// void Node_Greedy_Insert::repairSolNode(ISolution& sol)
// {
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     vector<int> all_non_inserted = vrpsol.getNonInsertedNodes();
//     while(!all_non_inserted.empty())
//     {
//         vector<tuple<double, int, int>> mincost_noninserted;
//         for(int i = 0; i < all_non_inserted.size(); i++)
//         {
//             mincost_noninserted.push_back(vrpsol.calCheapestInsertCostsAllRoutes(all_non_inserted[i], 0));
//         }
//         auto it = min_element(mincost_noninserted.begin(), mincost_noninserted.end(), [&](auto A, auto B){return get<0>(A) < get<0>(B);});
//         int selected_node = all_non_inserted[it - mincost_noninserted.begin()];
//         if(get<0>(*it) < INF)
//         {
//             int insert_rid = get<2>(*it);
//             int insert_pos = get<1>(*it);
//             vrpsol.getOneRoute(insert_rid)->setRouteByInsertNode(insert_pos, selected_node);
//             vrpsol.getNonInsertedNodes().erase(remove(vrpsol.getNonInsertedNodes().begin(), vrpsol.getNonInsertedNodes().end(), selected_node));
//             vrpsol.getTotalDistBeforePlatooning() += get<0>(*it);
//             vrpsol.getTotalUnservedRequests() -= 1;
//             vrpsol.setSolCurOperation(VRPSolution::InsertOneNode);
//         }
//         all_non_inserted.erase(all_non_inserted.begin());
//     }
// }

// void Node_Greedy_Insert::update(ISolution& sol, ALNS_Iteration_Status& status)
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