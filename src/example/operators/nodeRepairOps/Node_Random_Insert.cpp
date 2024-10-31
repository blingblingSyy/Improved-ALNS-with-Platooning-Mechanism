#include "example/operators/nodeRepairOps/Node_Random_Insert.h"
#include "example/model/establish/VRPSolution.h"
#include "example/model/establish/ARoute.h"
#include "alns/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <iostream>
using namespace std;

Node_Random_Insert::Node_Random_Insert(string s) : ANodeRepairOperator(s)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
}

Node_Random_Insert::~Node_Random_Insert()
{

}

/* old version */
// void Node_Random_Insert::repairSolNode(ISolution& sol)  //! wrong: insert to the route with the same type as the inserted node
// {
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     vector<int> all_non_inserted = vrpsol.getNonInsertedNodes();
//     vector<pair<int, int>> all_cus_pos = vrpsol.getAllCusPos();
//     initNonRepairablePos(vrpsol); //! initialize nonRepairablePos
//     while(!all_non_inserted.empty()) //! insert the first element in the noninserted each time
//     {
//         vector<pair<int, int>> cus_pos_copy = all_cus_pos;
//         keepRepairablePos(vrpsol, cus_pos_copy); //! will sort nonRepairablePos
//         while(!cus_pos_copy.empty())
//         {
//             RandomNumber r1;
//             int p = r1.get_rint(0, cus_pos_copy.size()-1);
//             pair<int, int> selected_vehpos = cus_pos_copy[p];
//             vrpsol.insertNode(selected_vehpos.second, all_non_inserted.front(), selected_vehpos.first);
//             if(vrpsol.getSolCurOperation() == VRPSolution::InsertOneNode)
//             {
//                 updateAllCusPos(vrpsol, all_cus_pos, selected_vehpos);
//                 updateNonRepairablePos(selected_vehpos.first, selected_vehpos.second);
//                 break;
//             }
//             cus_pos_copy.erase(cus_pos_copy.begin()+p);
//         }
//         all_non_inserted.erase(all_non_inserted.begin());
//     }
// }

/*new version*/
void Node_Random_Insert::repairSolNode(ISolution& sol)  //! wrong: insert to the route with the same type as the inserted node
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    initNonRepairablePos(vrpsol); //! initialize nonRepairablePos
    // for(int i = 0; i < nonRepairablePos.size(); i++)
    // {
    //     cout << "print nonRepairablePos " << i << ": ";
    //     for(int j = 0; j < nonRepairablePos[i].size(); j++)
    //     {
    //         cout << "(" << nonRepairablePos[i][j].first << "," << nonRepairablePos[i][j].second << ") ";
    //     }
    //     cout << endl;
    // }
    // cout << endl;
    for(int i = 0; i < VTYPE; i++)
    {
        vector<int> all_non_inserted = vrpsol.getNonInsertedNodes()[i];
        vector<pair<int, int>> all_cus_pos = vrpsol.getAllCusPos(i); //! only pick the vehicle with the same type as the non-inserted nodes
        while(!all_non_inserted.empty()) //! iterate through all non-inserted nodes -> insert the first element in the noninserted each time
        {
            vector<pair<int, int>> cus_pos_copy = all_cus_pos;
            keepRepairablePos(vrpsol, cus_pos_copy, i);  //! type == i
            while(!cus_pos_copy.empty()) //iterate through all possible insertion positions
            {
                RandomNumber r1;
                int p = r1.get_rint(0, cus_pos_copy.size()-1);
                pair<int, int> selected_vehpos = cus_pos_copy[p];
                vrpsol.insertNode(selected_vehpos.second, all_non_inserted.front(), selected_vehpos.first);
                if(vrpsol.getSolCurOperation() == VRPSolution::InsertOneNode)
                {
                    updateAllCusPos(vrpsol, all_cus_pos, selected_vehpos);
                    updateNonRepairablePos(vrpsol, selected_vehpos.first, selected_vehpos.second);  //! will sort nonRepairablePos
                    break;
                }
                cus_pos_copy.erase(cus_pos_copy.begin()+p);
            }
            all_non_inserted.erase(all_non_inserted.begin());
        }
    }
}

void Node_Random_Insert::initNonRepairablePos(VRPSolution& vrpsol)
{
    nonRepairablePos.clear();
    nonRepairablePos.resize(2);
    vector<tuple<int, int, int>> all_modified_arcs = vrpsol.getDestroyedArcsPos();
    for(int a = 0; a < all_modified_arcs.size(); a++)
    {
        int rid = get<2>(all_modified_arcs[a]);
        int arcpos = get<0>(all_modified_arcs[a]);
        if(vrpsol.getOneRoute(rid)->getVehType() == 0)
        {
            nonRepairablePos[0].push_back(make_pair(rid, arcpos+1));
        }
        else if(vrpsol.getOneRoute(rid)->getVehType() == 1)
        {
            nonRepairablePos[1].push_back(make_pair(rid, arcpos+1));
        }
    }
}

/*old version*/
// void Node_Random_Insert::keepRepairablePos(VRPSolution& vrpsol, vector<pair<int, int>>& all_cus_pos_copy, int type)
// {
//     auto less_comp = [&](pair<int, int> A, pair<int, int> B) {return A < B;};
//     if(type == 0 || type == 1)
//     {
//         vector<pair<int, int>> nonRepairablePos_copy = nonRepairablePos[type];
//         process_intersections(all_cus_pos_copy, nonRepairablePos_copy, false);
//         //! sort nonRepairablePos and all_cus_pos_copy
//         sort(nonRepairablePos[type].begin(), nonRepairablePos[type].end(), less_comp);
//     }
//     else 
//     {
//         vector<pair<int, int>> nonRepairablePos_copy = nonRepairablePos[0];
//         nonRepairablePos_copy.insert(nonRepairablePos_copy.end(), nonRepairablePos[0].begin(), nonRepairablePos[0].end());
//         process_intersections(all_cus_pos_copy, nonRepairablePos_copy, false);
//         //! sort nonRepairablePos and all_cus_pos_copy
//         sort(nonRepairablePos[0].begin(), nonRepairablePos[0].end(), less_comp);
//         sort(nonRepairablePos[1].begin(), nonRepairablePos[1].end(), less_comp);
//     }

//     sort(all_cus_pos_copy.begin(), all_cus_pos_copy.end(), less_comp);

//     // sort(nonRepairablePos.begin(), nonRepairablePos.end(), [&](pair<int, int> A, pair<int, int> B) {return A > B;});
//     // sort(all_cus_pos.begin(), all_cus_pos.end(), [&](pair<int, int> A, pair<int, int> B) {return A < B;});
//     // //! erase the unremovable positions from the all_cus_pos
//     // for(auto it = nonRepairablePos.begin(); it != nonRepairablePos.end(); it++)
//     // {
//     //     all_cus_pos.erase(remove(all_cus_pos.begin(), all_cus_pos.end(), *it));  //! sort nonRepairablePos in this way so that the removal is valid
//     // }
// }

/*new version*/
void Node_Random_Insert::keepRepairablePos(VRPSolution& vrpsol, vector<pair<int, int>>& all_cus_pos_copy, int type)
{
    assert(type == 0 || type == 1);
    vector<pair<int, int>> nonRepairablePos_copy = nonRepairablePos[type];
    process_intersections(all_cus_pos_copy, nonRepairablePos_copy, false);
    sort(nonRepairablePos[type].begin(), nonRepairablePos[type].end(), [&](pair<int, int> A, pair<int, int> B) {return A < B;});
    sort(all_cus_pos_copy.begin(), all_cus_pos_copy.end(), [&](pair<int, int> A, pair<int, int> B) {return A < B;});

    // sort(nonRepairablePos.begin(), nonRepairablePos.end(), [&](pair<int, int> A, pair<int, int> B) {return A > B;});
    // sort(all_cus_pos.begin(), all_cus_pos.end(), [&](pair<int, int> A, pair<int, int> B) {return A < B;});
    // //! erase the unremovable positions from the all_cus_pos
    // for(auto it = nonRepairablePos.begin(); it != nonRepairablePos.end(); it++)
    // {
    //     all_cus_pos.erase(remove(all_cus_pos.begin(), all_cus_pos.end(), *it));  //! sort nonRepairablePos in this way so that the removal is valid
    // }
}

void Node_Random_Insert::updateAllCusPos(VRPSolution& vrpsol, vector<pair<int, int>>& all_cus_pos, pair<int, int> insert_vehpos)
{
    int insert_rid = insert_vehpos.first;
    int insert_pos = insert_vehpos.second;
    int new_pos = vrpsol.getOneRoute(insert_rid)->getCompactRoute().size()-2; //! the size of the inserted compact route increases by one
    // auto it = find(all_cus_pos.begin(), all_cus_pos.end(), pair(insert_rid, new_pos-1));
    // all_cus_pos.insert(it+1, make_pair(insert_rid, new_pos));
    all_cus_pos.push_back(make_pair(insert_rid, new_pos));
}

void Node_Random_Insert::updateNonRepairablePos(VRPSolution& vrpsol, int insert_rid, int insert_pos)  //pair<int, int> insert_vehpos
{
    // int insert_rid = insert_vehpos.first;
    // int insert_pos = insert_vehpos.second;
    int vehtype = vrpsol.getOneRoute(insert_rid)->getVehType();
    sort(nonRepairablePos[vehtype].begin(), nonRepairablePos[vehtype].end(), [&](pair<int, int> A, pair<int, int> B) {return A < B;});
    auto it1 = find_if(nonRepairablePos[vehtype].begin(), nonRepairablePos[vehtype].end(), [&](pair<int, int> A){return A.first == insert_rid;});
    if(it1 != nonRepairablePos[vehtype].end())
    {
        //! nonRepariablePos is sorted descendingly
        // auto it2 = find_if(nonRepairablePos[vehtype].begin(), nonRepairablePos[vehtype].end(), [&](pair<int, int> A){return A.first < insert_rid;});
        auto it2 = find_if(nonRepairablePos[vehtype].begin(), nonRepairablePos[vehtype].end(), [&](pair<int, int> A){return A.first > insert_rid;});
        for(auto it = it1; it != it2; it++)
        {
            if((*it).first == insert_rid && (*it).second > insert_pos)
            {
                (*it).second += 1;
            }
        }
    }
    vector<tuple<int, int, int>>& destroyedArcPosSet = vrpsol.getDestroyedArcsPos();
    for(auto it = destroyedArcPosSet.begin(); it != destroyedArcPosSet.end(); it++)
    {
        if(get<2>(*it) == insert_rid && get<0>(*it) > insert_pos)
        {
            get<0>(*it) += 1;
        }
    }
}

/*old version*/
// vector<int> Node_Random_Insert::nonRepairableNodes(VRPSolution& vrpsol)
// {
//     //! find out the positions that cannot be removed
//     vector<int> non_repair_pos;
//     vector<tuple<int, int, int>> all_modified_arcs = vrpsol.getDestroyedArcsPos();
//     for(int a = 0; a < all_modified_arcs.size(); a++)
//     {
//         int rid = get<2>(all_modified_arcs[a]);
//         int arcpos = get<0>(all_modified_arcs[a]);
//         int insertAtNode = vrpsol.getOneRoute(rid)->getCompactRoute()[arcpos+1];
//         non_repair_pos.push_back(insertAtNode);
//     }
// }

// void Node_Random_Insert::keepRepairablePos(VRPSolution& vrpsol, vector<pair<int, int>> all_cus_pos, vector<pair<int, int>> non_repair_pos)
// {
//     //! find out the positions that cannot be removed
//     vector<pair<int, int>> non_repair_pos;
//     vector<tuple<int, int, int>> all_modified_arcs = vrpsol.getDestroyedArcsPos();
//     for(int a = 0; a < all_modified_arcs.size(); a++)
//     {
//         int rid = get<2>(all_modified_arcs[a]);
//         int arcpos = get<0>(all_modified_arcs[a]);
//         non_repair_pos.push_back(make_pair(rid, arcpos+1));
//     }
//     sort(non_repair_pos.begin(), non_repair_pos.end(), [&](pair<int, int> A, pair<int, int> B) {return A > B;});
//     sort(all_cus_pos.begin(), all_cus_pos.end(), [&](pair<int, int> A, pair<int, int> B) {return A < B;});
//     //! erase the unremovable positions from the all_cus_pos
//     for(auto it = non_repair_pos.begin(); it != non_repair_pos.end(); it++)
//     {
//         all_cus_pos.erase(remove(all_cus_pos.begin(), all_cus_pos.end(), *it));
//     }
// }

/*old version*/
// void Node_Random_Insert::update(ISolution& sol, ALNS_Iteration_Status& status)
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