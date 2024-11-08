#include "example/operators/nodeDestroyOps/Node_Worst_Removal.h"
#include "example/model/establish/VRPSolution.h"
#include "example/model/establish/ARoute.h"
#include "example/model/basic/config.h"
#include "example/operators/nodeDestroyOps/Node_Random_Removal.h"
#include "example/operators/Operators_Parameters.h"
#include "alns/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
using namespace std;

Node_Worst_Removal::Node_Worst_Removal(string s, Operators_Parameters& ops_param, int cusnum) : Node_Random_Removal(s, ops_param, cusnum)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
    this->randWorst = ops_param.getRandWorstParam();
}

Node_Worst_Removal::~Node_Worst_Removal()
{
    
}

void Node_Worst_Removal::destroySolNode(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    calNodeDestroySize(vrpsol.getTotalServedCusNum()); //! get the nodeDestroySize
    vector<pair<int, int>> all_cus_pos = vrpsol.getAllCusPos();
    keepRemovablePos(vrpsol, all_cus_pos); //! this will modify all_cus_pos
    vector<double> removalCosts_AllCusPos;
    for(auto it = all_cus_pos.begin(); it != all_cus_pos.end(); it++)
    {
        double removalcosts = vrpsol.evaluateRemoveNode((*it).second, (*it).first);  //! removed_node.second = arcpos, removed_node.first = routeid;
        removalCosts_AllCusPos.push_back(removalcosts);  //! removal costs are negative -> the more negative it is, the more costs it holds at the current position
    }
    sortVec(all_cus_pos, removalCosts_AllCusPos, false);  //! greater = false;
    // sort(all_cus_pos.begin(), all_cus_pos.end(), [&](auto A, auto B) -> bool {return calRemovalCost(vrpsol, A) > calRemovalCost(vrpsol, B);});
    int i = 0;
    while(i < nodeDestroySize && !all_cus_pos.empty())
    {
        RandomNumber r1;
        int new_select = int(pow(r1.get_rflt(0,1), randWorst) * (all_cus_pos.size()-1));
        destroyed_nodeset.push_back(all_cus_pos[new_select]);
        all_cus_pos.erase(all_cus_pos.begin() + new_select);
        i++;
    }
    sort(destroyed_nodeset.begin(), destroyed_nodeset.end(), [&](pair<int, int> A, pair<int, int> B) {return A > B;});
    for(int k = 0; k < destroyed_nodeset.size(); k++)
    {
        vrpsol.removeNode(destroyed_nodeset[k].second, destroyed_nodeset[k].first);
    }
    destroyed_nodeset.clear();
    // if(destroyed_nodeset.empty()) //! no nodes are actually removed
    // {
    //     setEmpty(true);
    //     setToSelectNext(false);
    // }
}

/* old version 1 -> consider platooning -> very very slow */
// double Node_Worst_Removal::calRemovalCost(VRPSolution& vrpsol, pair<int, int> removed_node)
// {
//     // vrpsol.recomputeCost();
//     double removalCost = -INF;
//     double orig_cost = vrpsol.getPenalizedObjectiveValue(true);
//     VRPSolution& vrpsol_new = dynamic_cast<VRPSolution&>(*(vrpsol.getCopy()));
//     vrpsol_new.removeNode(removed_node.second, removed_node.first); //! removed_node.second = arcpos, removed_node.first = routeid;
//     if(vrpsol_new.getSolCurOperation() == VRPSolution::RemoveOneNode) //! can remove the node
//     {
//         // vrpsol_new.getNonInsertedNodes().erase(remove(vrpsol_new.getNonInsertedNodes().begin(), vrpsol_new.getNonInsertedNodes().end(), 
//         //                                 vrpsol_new.getOneRoute(removed_node.first)->getCompactRoute()[removed_node.second]));
//         vrpsol_new.makePlatoons();
//         vrpsol_new.recomputeCost(); //! rebuild platoons
//         double new_cost = vrpsol_new.getPenalizedObjectiveValue(true); //! modified = true -> without the unserved request costs
//         removalCost = orig_cost - new_cost;
//     }
//     delete &vrpsol_new;
//     return removalCost;
// }

/*old version 2 -> only consider total distance before platooning*/
// double Node_Worst_Removal::calRemovalCost(VRPSolution& vrpsol, pair<int, int> removed_node)
// {
//     double cost = vrpsol.evaluateRemoveNode(removed_node.second, removed_node.first); //! removed_node.second = arcpos, removed_node.first = routeid;
//     // vrpsol.recomputeCost();
//     double removalCost = -INF;
//     double orig_cost = vrpsol.getTotalDistBeforePlatooning();
//     VRPSolution& vrpsol_new = dynamic_cast<VRPSolution&>(*(vrpsol.getCopy()));
//     vrpsol_new.removeNode(removed_node.second, removed_node.first); //! removed_node.second = arcpos, removed_node.first = routeid;
//     if(vrpsol_new.getSolCurOperation() == VRPSolution::RemoveOneNode) //! can remove the node
//     {
//         double new_cost = vrpsol_new.calTotalDist(); //! modified = true -> without the unserved request costs
//         removalCost = orig_cost - new_cost;
//     }
//     delete &vrpsol_new;
//     return removalCost;
// }

/* old version */
// double calRemovalCost(VRPSolution& vrpsol, pair<int, int> removed_node)
// {
//     // vrpsol.recomputeCost();
//     double orig_cost = vrpsol.getPenalizedObjectiveValue(true);
//     VRPSolution& vrpsol_new = dynamic_cast<VRPSolution&>(*(vrpsol.getCopy()));
//     if(vrpsol.evaluateRemoveNode(removed_node.second, removed_node.first) < INF) // vrpsol.getNonInsertedNodes().size() == vrpsol_new.getNonInsertedNodes().size()) //! cannot remove the node
//     {
//         vrpsol_new.removeNode(removed_node.second, removed_node.first); //! removed_node.second = arcpos, removed_node.first = routeid;
//         vrpsol_new.recomputeCost(true); //! rebuild platoons
//         double new_cost = vrpsol_new.getPenalizedObjectiveValue(true); //! modified = true -> without the unserved request costs
//         return orig_cost - new_cost;
//     }
//     else
//     {
//         return -1;
//     }
// }


/*old version*/
// void Node_Worst_Removal::update(ISolution& sol, ALNS_Iteration_Status& status)
// {
//     if(hasSelectedCur && needUpdate)
//     {
//         forbidden_destroyed_nodepos.clear();
//         VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//         vector<tuple<int, int, int>> destroyed_arcpos = vrpsol.getDestroyedArcsPos();
//         for(int i = 0; i < destroyed_arcpos.size(); i++)
//         {
//             pair<int, int> forbidden_pos1 = make_pair(get<2>(destroyed_arcpos[i]), get<0>(destroyed_arcpos[i]));
//             pair<int, int> forbidden_pos2 = make_pair(get<2>(destroyed_arcpos[i])+1, get<0>(destroyed_arcpos[i]));
//             forbidden_destroyed_nodepos.insert(forbidden_pos1);
//             forbidden_destroyed_nodepos.insert(forbidden_pos2);
//         }        
//     }
// }