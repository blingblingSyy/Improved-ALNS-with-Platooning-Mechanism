#include "example/operators/nodeRepairOps/Node_Regret_Insert.h"
#include "example/model/establish/VRPSolution.h"
#include "example/model/establish/ARoute.h"
#include "alns/ALNS_inc.h"
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
    this->regret_k = regret_k;
}

Node_Regret_Insert::~Node_Regret_Insert()
{

}

/*old version: take more than 30s to finish repairing a solution*/ 
// void Node_Regret_Insert::repairSolNode(ISolution& sol)  //! wrong: insert to the route with the same type as the inserted node
// {
//     cout << "******Start to node repair the solution******" << endl;
//     double startNodeRep = clock(); 
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     initNonRepairablePos(vrpsol); //! initialize nonRepairablePos
//     // if(!nonRepairablePos.empty())  //! check the structure of nonRepaiablePos
//     // {
//     //     for(int i = 0; i < nonRepairablePos.size(); i++)
//     //     {
//     //         for(int j = 0; j < nonRepairablePos[i].size(); j++)
//     //         {
//     //             cout << "(" << nonRepairablePos[i][j].first << "," << nonRepairablePos[i][j].second << ")\t";
//     //         }
//     //     }
//     //     cout << endl;
//     // }
//     // else
//     // {
//     //     cout << "nonRepairablePos is empty" << endl;
//     // }
//     double initRepPosTime = clock();
//     cout << "Intialize non-repairable position time: " << (initRepPosTime - startNodeRep)/(double)CLOCKS_PER_SEC << endl;
//     for(int i = 0; i < VTYPE; i++)
//      {
//         vector<int> all_non_inserted = vrpsol.getNonInsertedNodes()[i];
//         while(!all_non_inserted.empty()) //! iterate through all non-inserted nodes -> insert the first element in the noninserted each time
//         {
//             //! the ordered insertion costs among all routes in the solution for all non-inserted nodes 
//             double startCalOrderedCostsTime = clock();
//             vector<vector<tuple<double, int, int>>> orderedInsertCosts_AllNodes = calNoisedOrderedInsertCostsAllNodes(vrpsol, all_non_inserted);
//             double startCalRegretValTime = clock();
//             cout << "~ time to calculate the ordered costs of all non-inserted nodes: " << (startCalRegretValTime - startCalOrderedCostsTime)/(double)CLOCKS_PER_SEC << endl;
//             //! the regret value for all non-inserted nodes
//             vector<double> regretVal_AllNodes = calRegretValAllNodes(all_non_inserted, orderedInsertCosts_AllNodes);
//             double startCalMinCostPosTime = clock();
//             cout << "~ time to calculate the regret value of all non-inserted nodes: " << (startCalMinCostPosTime - startCalRegretValTime)/(double)CLOCKS_PER_SEC << endl;
//             //! the minimum insertion cost position that is repairable (feasible) for all nodes
//             vector<tuple<double, int, int>> minCostPos_AllNodes = findFeasPosAllNodes(vrpsol, all_non_inserted, orderedInsertCosts_AllNodes);
//             //! for all non-inserted nodes, pick the one with the maximum regret value (-> minimum negative regret value)
//             double startInsertionTime = clock();
//             cout << "~ time to calculate the mininum costs positions that are feasible: " << (startInsertionTime - startCalMinCostPosTime)/(double)CLOCKS_PER_SEC << endl;
//             auto it = max_element(regretVal_AllNodes.begin(), regretVal_AllNodes.end());
//             int selected_node = all_non_inserted[it - regretVal_AllNodes.begin()];
//             tuple<double, int, int> selected_costpos = minCostPos_AllNodes[it - regretVal_AllNodes.begin()];
//             if(get<0>(selected_costpos) < INF)
//             {
//                 int insert_rid = get<2>(selected_costpos);
//                 int insert_pos = get<1>(selected_costpos);
//                 if(vrpsol.getOneRoute(insert_rid)->getVehType() == i)
//                 {
//                     double beforeInsertionTime = clock();
//                     vrpsol.insertNode(insert_pos, selected_node, insert_rid);
//                     double afterInsertionTime = clock();
//                     cout << "Time to insert a node: " << (afterInsertionTime - beforeInsertionTime)/(double)CLOCKS_PER_SEC << endl;
//                     updateNonRepairablePos(vrpsol, insert_rid, insert_pos);
//                     double updateNonRepPosTime = clock();
//                     cout << "Time to update non-repairable positions: " << (updateNonRepPosTime - afterInsertionTime)/(double)CLOCKS_PER_SEC << endl;
//                     // if(!nonRepairablePos.empty())  //! check the structure of nonRepaiablePos
//                     // {
//                     //     for(int i = 0; i < nonRepairablePos.size(); i++)
//                     //     {
//                     //         for(int j = 0; j < nonRepairablePos[i].size(); j++)
//                     //         {
//                     //             cout << "(" << nonRepairablePos[i][j].first << "," << nonRepairablePos[i][j].second << ")\t";
//                     //         }
//                     //     }
//                     //     cout << endl;
//                     // }
//                     // else
//                     // {
//                     //     cout << "nonRepairablePos is empty" << endl;
//                     // }
//                 }
//             }
//             //! No else: should continue to check if the node with the second smallest regret value can be feasibly inserted into the solution
//             // else //! if the mincost >= INF, no insertion will occur, can end the algorithm.
//             // {
//             //     all_non_inserted.clear();
//             //     break;
//             // }
//             all_non_inserted.erase(all_non_inserted.begin());
//         }
//     }
//     double overallInsertionTime = clock();
//     cout << "******Node Regret Insertion Module Time: " << (overallInsertionTime - initRepPosTime)/(double)CLOCKS_PER_SEC << endl;
// }

void Node_Regret_Insert::repairSolNode(ISolution& sol)  //! wrong: insert to the route with the same type as the inserted node
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    initNonRepairablePos(vrpsol); //! initialize nonRepairablePos
    for(int i = 0; i < VTYPE; i++)
     {
        vector<int> all_non_inserted = vrpsol.getNonInsertedNodes()[i];
        // double t1 = clock();
        //! the ordered insertion costs among all routes in the solution for all non-inserted nodes (ordered by all_non_inserted)
        vector<vector<tuple<double, int, int>>> orderedInsertCosts_AllNodes = calNoisedOrderedInsertCostsAllNodes(vrpsol, all_non_inserted);
        // double t2 = clock();
        // cout << "time to calculate the ordered insertion costs: " << (t2-t1)/(double)CLOCKS_PER_SEC << endl;
        //! the regret value for all non-inserted nodes (ordered by all_non_inserted)
        vector<double> regretVal_AllNodes = calRegretValAllNodes(all_non_inserted, orderedInsertCosts_AllNodes);
        //! the minimum insertion cost position that is repairable (feasible) for all nodes (ordered by all_non_inserted)
        vector<tuple<double, int, int>> minCostPos_AllNodes = findFeasPosAllNodes(vrpsol, all_non_inserted, orderedInsertCosts_AllNodes);
        while(!all_non_inserted.empty()) //! iterate through all non-inserted nodes -> insert the first element in the noninserted each time
        {
            //! for all non-inserted nodes, pick the one with the maximum regret value (-> minimum negative regret value)
            auto it = max_element(regretVal_AllNodes.begin(), regretVal_AllNodes.end());
            int selected_idx = it - regretVal_AllNodes.begin();
            int selected_node = all_non_inserted[selected_idx];
            tuple<double, int, int> selected_costpos = minCostPos_AllNodes[selected_idx];
            //! remove the selected node index
            all_non_inserted.erase(all_non_inserted.begin()+selected_idx); ////// wrong: not erasing the first
            orderedInsertCosts_AllNodes.erase(orderedInsertCosts_AllNodes.begin()+selected_idx);
            regretVal_AllNodes.erase(regretVal_AllNodes.begin()+selected_idx);
            minCostPos_AllNodes.erase(minCostPos_AllNodes.begin()+selected_idx);
            if(get<0>(selected_costpos) < INF)
            {
                int insert_rid = get<2>(selected_costpos);
                int insert_pos = get<1>(selected_costpos);
                if(vrpsol.getOneRoute(insert_rid)->getVehType() == i)
                {
                    vrpsol.insertNode(insert_pos, selected_node, insert_rid);
                    if(vrpsol.getSolCurOperation() == VRPSolution::InsertOneNode)
                    {
                        // double t3 = clock();
                        updateNonRepairablePos(vrpsol, insert_rid, insert_pos);
                        // double t4 = clock();
                        // cout << "time to update non-repairable positions: " << (t4-t3)/(double)CLOCKS_PER_SEC << endl;
                        //! update the insertion costs of all the other nodes except the inserted node, as the insertion changes the structure of the inserted route
                        updateOrderedInsertCostsAllNodes(vrpsol, orderedInsertCosts_AllNodes, all_non_inserted, insert_rid, insert_pos);
                        // double t5 = clock();
                        // cout << "time to update ordered costs: " << (t5-t4)/(double)CLOCKS_PER_SEC << endl;
                        regretVal_AllNodes = calRegretValAllNodes(all_non_inserted, orderedInsertCosts_AllNodes);
                        // double t6 = clock();
                        // cout << "time to update the regret value: " << (t6-t5)/(double)CLOCKS_PER_SEC << endl;
                        minCostPos_AllNodes = findFeasPosAllNodes(vrpsol, all_non_inserted, orderedInsertCosts_AllNodes);
                        // double t7 = clock();
                        // cout << "time to update the feasible positions: " << (t7-t6)/(double)CLOCKS_PER_SEC << endl;
                    }

                }
            }
        }
    }
}

vector<vector<tuple<double, int, int>>> Node_Regret_Insert::calNoisedOrderedInsertCostsAllNodes(VRPSolution& vrpsol, vector<int> all_non_inserted)
{
    vector<vector<tuple<double, int, int>>> orderedCostsAllNodes;
    for(int i = 0; i < all_non_inserted.size(); i++)
    {
        vector<tuple<double, int, int>> ordered_insert_costs = vrpsol.calOrderedInsertCostsAllRoutes(all_non_inserted[i]);
        if(noise)
        {
            RandomNumber r;
            for(auto it = ordered_insert_costs.begin(); it != ordered_insert_costs.end(); it++)
            {
                if(get<0>(*it) < INF)
                {
                    double cost = get<0>(*it) * r.get_rflt();
                    get<0>(*it) = cost;
                }
                else
                {
                    break;
                }
            }
            sort(ordered_insert_costs.begin(), ordered_insert_costs.end(), [&](auto A, auto B){return get<0>(A) < get<0>(B);});
        }
        orderedCostsAllNodes.push_back(ordered_insert_costs);
    }
    return orderedCostsAllNodes;
}

void Node_Regret_Insert::updateOrderedInsertCostsAllNodes(VRPSolution& vrpsol, vector<vector<tuple<double, int, int>>>& origOrderedCostsAllNodes, 
                                                            vector<int> all_non_inserted, int insert_rid, int insert_pos)
{
    double t1 = clock();
    RandomNumber r;
    ARoute* inserted_route = vrpsol.getOneRoute(insert_rid);
    for(int i = 0; i < all_non_inserted.size(); i++)
    {
        for(auto cost_it = origOrderedCostsAllNodes[i].begin(); cost_it != origOrderedCostsAllNodes[i].end(); )  //<insert_cost, insert_pos, insert_route>
        {
            if(get<2>(*cost_it) == insert_rid && get<1>(*cost_it) == insert_pos)
            {
                cost_it = origOrderedCostsAllNodes[i].erase(cost_it);
            }
            else if(get<2>(*cost_it) == insert_rid && get<1>(*cost_it) > insert_pos)  //! >= insert_pos+1
            {
                get<1>(*cost_it) += 1;
                cost_it++;
            }
            else
            {
                cost_it++;
            }
        }
        for(int pos = insert_pos; pos <= insert_pos+1; pos++)
        {
            inserted_route->evaluateRouteByInsertNode(pos, all_non_inserted[i]);
            double insertioncosts = inserted_route->getRouteOperatorCosts(ARoute::InsertOneNode);
            if(insertioncosts < INF)
            {
                if(noise)  insertioncosts *= r.get_rflt();
                for(auto it = origOrderedCostsAllNodes[i].begin(); it != origOrderedCostsAllNodes[i].end();)  //! worst case: O(N)
                {
                    if(get<0>(*it) < insertioncosts)
                    {
                        it++;
                    }
                    else
                    {
                        origOrderedCostsAllNodes[i].insert(it, make_tuple(insertioncosts, pos, insert_rid)); //! <insert_cost, insert_pos, insert_route>
                        break;
                    }
                }
            }
            else
            {
                origOrderedCostsAllNodes[i].push_back(make_tuple(insertioncosts, pos, insert_rid)); //! <insert_cost, insert_pos, insert_route>
            }
            // origOrderedCostsAllNodes[i].push_back(make_tuple(insertioncosts, pos, insert_rid)); //! <insert_cost, insert_pos, insert_route>
        }
        // sort(origOrderedCostsAllNodes[i].begin(), origOrderedCostsAllNodes[i].end(), [&](auto A, auto B){return get<0>(A) < get<0>(B);});  //! worst case: O(N^2)
    }
    double t2 = clock();
    cout << "update ordered costs time: " << (t2-t1)/(double)CLOCKS_PER_SEC << endl;
}

vector<double> Node_Regret_Insert::calRegretValAllNodes(vector<int> all_non_inserted, vector<vector<tuple<double, int, int>>> orderedCostsAllNodes)
{
    vector<double> regretval_noninserted;
    for(int i = 0; i < all_non_inserted.size(); i++)
    {
        //! calculate the regret value of the node inserted to the solution
        if(regret_k > 1)
        {
            vector<double> regret_val_i; //! take the average regret value
            for(int k = 1; k < regret_k && k < orderedCostsAllNodes[i].size(); k++)  //int k = pos+1; k < pos+K; k++
            {
                regret_val_i.push_back(get<0>(orderedCostsAllNodes[i][k]) - get<0>(orderedCostsAllNodes[i][0]));
            }
            regretval_noninserted.push_back(calAverage(regret_val_i));
        }
        else
        {
            regretval_noninserted.push_back(-get<0>(orderedCostsAllNodes[i][0])); //! take negative because we want the maximum regret value but the minimum of all minimum costs among the solution 
        }
    }
    return regretval_noninserted;
}

vector<tuple<double, int, int>> Node_Regret_Insert::findFeasPosAllNodes(VRPSolution& vrpsol, vector<int> all_non_inserted, vector<vector<tuple<double, int, int>>> orderedCostsAllNodes)
{
    vector<tuple<double, int, int>> mincostpos_noninserted(all_non_inserted.size(), make_tuple(INF, -1, -1));
    for(int i = 0; i < all_non_inserted.size(); i++) //! the all_non_insert is of specific type
    {
        //! for each node, insert to the first feasible position that is repairable
        for(int s = 0; s < orderedCostsAllNodes[i].size(); s++)
        {
            int selected_cost_i = get<0>(orderedCostsAllNodes[i][s]); //! what if the cost == INF
            int selected_rid_i = get<2>(orderedCostsAllNodes[i][s]);
            int selected_pos_i = get<1>(orderedCostsAllNodes[i][s]);
            int vehtype = vrpsol.getOneRoute(selected_rid_i)->getVehType();
            if(selected_cost_i < INF)
            {
                auto find_comp = [&](pair<int, int> A) -> bool {return A.first == selected_rid_i && A.second == selected_pos_i;};
                auto it = find_if(nonRepairablePos[vehtype].begin(), nonRepairablePos[vehtype].end(), find_comp);  //! nonRepairablePos is a 2-dim vector
                if(it == nonRepairablePos[vehtype].end()) //! if the insertion position is not at the modified arcpos
                {
                    mincostpos_noninserted[i] = orderedCostsAllNodes[i][s];
                    break;
                }  //! else: continue to check the next smallest cost and find whether the insertion position is forbidden
            }
            else  //! the ordered costs are ranked increasingly, if the smallest cost == INF, then no need to compare further
            {
                break;
            }
        }
    }
    return mincostpos_noninserted;
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