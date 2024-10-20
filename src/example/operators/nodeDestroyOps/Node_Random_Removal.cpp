#include "example/operators/nodeDestroyOps/Node_Random_Removal.h"
#include "example/model/establish/VRPSolution.h"
#include "example/model/establish/ARoute.h"
#include "example/operators/Operators_Parameters.h"
// #include "example/operators/tabuSearch/nodeTabu.h"
#include "alns/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
using namespace std;

Node_Random_Removal::Node_Random_Removal(string s, Operators_Parameters& ops_param, int cusnum) : ANodeDestroyOperator(s, ops_param.getNodeDestroySizeRate(), cusnum)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
}

Node_Random_Removal::~Node_Random_Removal()
{

}

/*old version*/
void Node_Random_Removal::destroySolNode(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    calNodeDestroySize(vrpsol.getTotalServedCusNum()); //! get the nodeDestroySize
    vector<pair<int, int>> all_cus_pos = vrpsol.getAllCusPos();
    keepRemovablePos(vrpsol, all_cus_pos); //! this will modify all_cus_pos
    int i = 0;
    while(i < nodeDestroySize && !all_cus_pos.empty())
    {
        RandomNumber r1;
        int select_pos = r1.get_rint(0, all_cus_pos.size()-1);
        destroyed_nodeset.push_back(all_cus_pos[select_pos]);
        all_cus_pos.erase(all_cus_pos.begin() + select_pos);
        i++;
    }
    sort(destroyed_nodeset.begin(), destroyed_nodeset.end(), [&](pair<int, int> A, pair<int, int> B) {return A > B;});
    for(int k = 0; k < destroyed_nodeset.size(); k++)
    {
        //! after removal, the solution may become infeasible; after insertion, the solution must be feasible.
        vrpsol.removeNode(destroyed_nodeset[k].second, destroyed_nodeset[k].first);
        if(vrpsol.getSolCurOperation() == VRPSolution::RemoveOneNode)
        {
            updateDestroyedArcPos(vrpsol, destroyed_nodeset[k]);
        }
    }
    destroyed_nodeset.clear();
    // if(destroyed_nodeset.empty()) //! no nodes are actually removed
    // {
    //     setEmpty(true);
    //     setToSelectNext(false);
    // }
}


void Node_Random_Removal::keepRemovablePos(VRPSolution& vrpsol, vector<pair<int, int>>& all_cus_pos)
{
    //! find out the positions that cannot be removed
    vector<pair<int, int>> non_remove_pos;
    // vector<pair<int, int>> all_cus_pos_copy = all_cus_pos;
    vector<tuple<int, int, int>> all_modified_arcs = vrpsol.getDestroyedArcsPos();
    for(int a = 0; a < all_modified_arcs.size(); a++)
    {
        int rid = get<2>(all_modified_arcs[a]);
        int arcpos = get<0>(all_modified_arcs[a]);
        non_remove_pos.push_back(make_pair(rid, arcpos)); //! include the starting depot
        non_remove_pos.push_back(make_pair(rid, arcpos+1)); //! include the ending depot
        // if(arcpos > 0)
        // {
        //     non_remove_pos.push_back(make_pair(rid, arcpos)); //! not removing the starting depot
        // }
        // if(arcpos+1 < vrpsol.getOneRoute(rid)->getCompactRoute().size()-1)
        // {
        //     non_remove_pos.push_back(make_pair(rid, arcpos+1)); //! not removing the ending depot
        // }
    }
    process_intersections(all_cus_pos, non_remove_pos, false); //! remove the intersections from all_cus_pos
    // sort(non_remove_pos.begin(), non_remove_pos.end(), [&](pair<int, int> A, pair<int, int> B) {return A > B;});
    // sort(all_cus_pos.begin(), all_cus_pos.end(), [&](pair<int, int> A, pair<int, int> B) {return A < B;});
    //! erase the unremovable positions from the all_cus_pos
    // for(auto it = non_remove_pos.begin(); it != non_remove_pos.end(); it++)
    // {
    //     all_cus_pos.erase(remove(all_cus_pos.begin(), all_cus_pos.end(), *it));
    // }
}

void Node_Random_Removal::updateDestroyedArcPos(VRPSolution& vrpsol, pair<int, int> destroyed_nodepos)
{
    vector<tuple<int, int, int>>& destroyedArcPosSet = vrpsol.getDestroyedArcsPos();
    for(auto it = destroyedArcPosSet.begin(); it != destroyedArcPosSet.end(); it++)
    {
        if(get<2>(*it) == destroyed_nodepos.first && get<0>(*it) > destroyed_nodepos.second)
        {
            get<0>(*it) -= 1;
        }
    }
}

/*old version*/
// void Node_Random_Removal::destroySolNode(ISolution& sol)
// {
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     calNodeDestroySize(vrpsol.getTotalServedCusNum()); //! get the nodeDestroySize
//     vector<vector<int>> all_cus_pos = vrpsol.getAllCusCopy();
//     vector<vector<int>> all_remove_pos = findRemovablePos(vrpsol, all_cus_pos);
//     vector<int> removable_routes;
//     for(int r = 0; r < all_remove_pos.size(); r++)
//     {
//         //! some routes may not be removed at all
//         if(count(all_remove_pos[r].begin(), all_remove_pos[r].end(), -1) < all_remove_pos[r].size())
//         {
//             removable_routes.push_back(r);
//         }
//     }
//     int i = 0;
//     while(i < nodeDestroySize)
//     {
//         //! find all removable routes
//         if(removable_routes.empty()) 
//         {
//             break; //! no node can be removed
//         }
//         else
//         {
//             RandomNumber r1;
//             int select_route = removable_routes[r1.get_rint(0, removable_routes.size()-1)];
//             //! find all removable positions in the selected route
//             vector<int> removable_pos;
//             for(int s = 0; s < all_remove_pos[select_route].size(); s++)
//             {
//                 if(all_remove_pos[select_route][s] != -1)
//                 {
//                     removable_pos.push_back(s);
//                 }
//             }
//             RandomNumber r2;
//             int select_pos = removable_pos[r2.get_rint(0, removable_pos.size()-1)];
//             int select_cus = all_cus_pos[select_route][select_pos];
//             all_cus_pos[select_route].erase(all_cus_pos[select_route].begin() + select_pos);
//             all_remove_pos[select_route].erase(all_remove_pos[select_route].begin() + select_pos);
//             if(count(all_remove_pos[select_route].begin(), all_remove_pos[select_route].end(), -1) == all_remove_pos[select_route].size())
//             {
//                 removable_routes.erase(remove(removable_routes.begin(), removable_routes.end(), select_route));
//             }
//             vrpsol.removeNode(select_pos, select_route);  //! after removal, the solution may become infeasible; after insertion, the solution must be feasible.
//             i++;
//         }
//     }
//     // if(vrpsol.getSolCurOperation() == VRPSolution::NoChange) //! no nodes are actually removed
//     // {
//     //     setEmpty(true);
//     //     setToSelectNext(false);
//     // }
// }

// vector<vector<int>> Node_Random_Removal::findRemovablePos(VRPSolution& vrpsol, vector<vector<int>> all_cus_pos)
// {
//     //! initialize with all customer positions
//     vector<vector<int>> all_remove_pos(all_cus_pos.size());
//     for(int i = 0; i < all_cus_pos.size(); i++)
//     {
//         for(int j = 0; j < all_cus_pos[i].size(); j++)
//         {
//             if(j > 0 || j < all_cus_pos.size()-1)
//             {
//                 all_remove_pos[i].push_back(j);
//             }
//             else
//             {
//                 all_remove_pos[i].push_back(-1); //! cannot remove the starting and ending depot
//             }
//         }
//     }
//     //! find out the positions that cannot be removed
//     vector<tuple<int, int, int>> all_modified_arcs = vrpsol.getDestroyedArcsPos();
//     for(int a = 0; a < all_modified_arcs.size(); a++)
//     {
//         int rid = get<2>(all_modified_arcs[a]);
//         int arcpos = get<0>(all_modified_arcs[a]);
//         all_remove_pos[rid][arcpos] = -1;
//         all_remove_pos[rid][arcpos] = -1;
//     }
//     //! erase the unremovable positions from the all_remove_pos
//     // for(int i = 0; i < all_remove_pos.size(); i++)
//     // {
//     //     all_remove_pos[i].erase(remove(all_remove_pos[i].begin(), all_remove_pos[i].end(), -1));
//     // }
//     return all_remove_pos;
// }