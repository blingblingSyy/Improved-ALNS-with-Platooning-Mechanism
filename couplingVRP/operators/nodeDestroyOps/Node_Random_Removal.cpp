#include "couplingVRP/operators/nodeDestroyOps/Node_Random_Removal.h"
#include "couplingVRP/model/establish/VRPSolution.h"
#include "couplingVRP/model/establish/ARoute.h"
#include "src/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
using namespace std;

Node_Random_Removal::Node_Random_Removal(string s, double maxNodeDestroyRate, int cusnum) : ANodeDestroyOperator(s, maxNodeDestroyRate, cusnum)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
}

Node_Random_Removal::~Node_Random_Removal()
{
    destroyed_arcpos.clear();
    forbidden_destroyed_nodepos.clear();
}

void Node_Random_Removal::destroySolNode(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    int orig_noninserted = vrpsol.getNonInsertedNodes().size();
    calNodeDestroySize(vrpsol.getTotalServedCusNum()); //! get the nodeDestroySize
    vector<pair<int, int>> all_cus_pos = vrpsol.getAllCusPos();
    for(auto it = all_cus_pos.begin(); it != all_cus_pos.end(); ) //! update the available all_cus_pos
    {
        if(forbidden_destroyed_nodepos.find(*it) != forbidden_destroyed_nodepos.end())
        {
            it = all_cus_pos.erase(it);
        }
    }
    int i = 0;
    while(i < nodeDestroySize)
    {
        RandomNumber r1;
        int select_pos = r1.get_rint(0, all_cus_pos.size()-1);
        // if(forbidden_destroyed_nodepos.find(all_cus_pos[select_pos]) != forbidden_destroyed_nodepos.end())
        // {
        //     destroyed_nodeset.push_back(all_cus_pos[select_pos]);
        // }
        // destroyed_nodeset.push_back(all_cus_pos[select_pos]);
        all_cus_pos.erase(all_cus_pos.begin() + select_pos);
        i++;
    }
    for(int k = 0; k < destroyed_nodeset.size(); k++)
    {
        //! after removal, the solution may become infeasible;
        //! after insertion, the solution must be feasible.
        vrpsol.removeNode(destroyed_nodeset[k].second, destroyed_nodeset[k].first);
    }
    // if(orig_noninserted - vrpsol.getNonInsertedNodes().size() == 0) //! no nodes are actually removed
    // {
    //     setEmpty(true);
    //     setToSelectNext(false);
    // }
    destroyed_arcpos = vrpsol.getDestroyedArcsPos();
}

/* old version */
// void Node_Random_Removal::update(ISolution& sol, ALNS_Iteration_Status& status)
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