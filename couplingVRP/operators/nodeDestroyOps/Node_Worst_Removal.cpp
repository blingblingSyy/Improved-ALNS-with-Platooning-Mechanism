#include "couplingVRP/operators/nodeDestroyOps/Node_Worst_Removal.h"
#include "couplingVRP/model/establish/VRPSolution.h"
#include "couplingVRP/model/establish/ARoute.h"
#include "couplingVRP/model/basic/config.h"
#include "src/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
using namespace std;

Node_Worst_Removal::Node_Worst_Removal(string s, ALNS_Parameters& alns_param, int cusnum, vector<tuple<int, int, int>> destroyed_arcpos) : ANodeDestroyOperator(s, alns_param.getNodeDestroyRate(), cusnum)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
    needUpdate = true;
    this->randWorst = alns_param.getRandWorstParam();
}

Node_Worst_Removal::~Node_Worst_Removal()
{

}

void Node_Worst_Removal::destroySolNode(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    int orig_noninserted = vrpsol.getNonInsertedNodes().size();
    calNodeDestroySize(vrpsol.getTotalServedCusNum()); //! get the nodeDestroySize
    vector<pair<int, int>> all_cus_pos = vrpsol.getAllCusPos();
    sort(all_cus_pos.begin(), all_cus_pos.end(), [&](auto A, auto B) -> bool {return calRemovalCost(vrpsol, A) > calRemovalCost(vrpsol, B);});
    int i = 0;
    while(i < nodeDestroySize)
    {
        RandomNumber r1;
        int new_select = int(pow(r1.get_rflt(0,1), randWorst) * (all_cus_pos.size()-1));
        destroyed_nodeset.push_back(all_cus_pos[new_select]);
        all_cus_pos.erase(all_cus_pos.begin() + new_select);
        i++;
    }
    for(int k = 0; k < destroyed_nodeset.size(); k++)
    {
        vrpsol.removeNode(destroyed_nodeset[k].second, destroyed_nodeset[k].first);
    }
    // if(orig_noninserted - vrpsol.getNonInsertedNodes().size() == 0) //! no nodes are actually removed
    // {
    //     setEmpty(true);
    //     setToSelectNext(false);
    // }
}

/* new version */
double Node_Worst_Removal::calRemovalCost(VRPSolution& vrpsol, pair<int, int> removed_node)
{
    // vrpsol.recomputeCost();
    double orig_cost = vrpsol.getPenalizedObjectiveValue(true);
    VRPSolution& vrpsol_new = dynamic_cast<VRPSolution&>(*(vrpsol.getCopy()));
    vrpsol_new.removeNode(removed_node.second, removed_node.first); //! removed_node.second = arcpos, removed_node.first = routeid;
    if(vrpsol.getSolCurOperation() == ARoute::NoChange) //! cannot remove the node
    {
        return -INF;
    }
    else
    {
        // vrpsol_new.getNonInsertedNodes().erase(remove(vrpsol_new.getNonInsertedNodes().begin(), vrpsol_new.getNonInsertedNodes().end(), 
        //                                 vrpsol_new.getOneRoute(removed_node.first)->getCompactRoute()[removed_node.second]));
        vrpsol_new.recomputeCost(true); //! rebuild platoons
        double new_cost = vrpsol_new.getPenalizedObjectiveValue(true); //! modified = true -> without the unserved request costs
        return orig_cost - new_cost;
    }
}

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

void Node_Worst_Removal::update(ISolution& sol, ALNS_Iteration_Status& status)
{
    if(hasSelectedCur && needUpdate)
    {
        forbidden_destroyed_nodepos.clear();
        VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
        vector<tuple<int, int, int>> destroyed_arcpos = vrpsol.getDestroyedArcsPos();
        for(int i = 0; i < destroyed_arcpos.size(); i++)
        {
            pair<int, int> forbidden_pos1 = make_pair(get<2>(destroyed_arcpos[i]), get<0>(destroyed_arcpos[i]));
            pair<int, int> forbidden_pos2 = make_pair(get<2>(destroyed_arcpos[i])+1, get<0>(destroyed_arcpos[i]));
            forbidden_destroyed_nodepos.insert(forbidden_pos1);
            forbidden_destroyed_nodepos.insert(forbidden_pos2);
        }        
    }
}