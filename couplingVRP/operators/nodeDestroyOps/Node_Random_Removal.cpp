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

}

void Node_Random_Removal::destroySolNode(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    int orig_noninserted = vrpsol.getNonInsertedNodes().size();
    calNodeDestroySize(vrpsol.getTotalServedCusNum()); //! get the nodeDestroySize
    vector<pair<int, int>> all_cus_pos = vrpsol.getAllCusPos();
    int i = 0;
    while(i < nodeDestroySize)
    {
        RandomNumber r1;
        int select_pos = r1.get_rint(0, all_cus_pos.size()-1);
        destroyed_nodeset.push_back(all_cus_pos[select_pos]);
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
}