#include "couplingVRP/operators/nodeRepairOps/Node_Greedy_Insert.h"
#include "couplingVRP/model/establish/VRPSolution.h"
#include "couplingVRP/model/establish/ARoute.h"
#include "src/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
using namespace std;

Node_Greedy_Insert::Node_Greedy_Insert(string s) : ANodeRepairOperator(s)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
}

Node_Greedy_Insert::~Node_Greedy_Insert()
{

}

void Node_Greedy_Insert::repairSolNode(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    vector<int> all_non_inserted = vrpsol.getNonInsertedNodes();
    while(!all_non_inserted.empty())
    {
        vector<tuple<double, int, int>> mincost_noninserted;
        for(int i = 0; i < all_non_inserted.size(); i++)
        {
            mincost_noninserted.push_back(vrpsol.calCheapestInsertCostsAllRoutes(all_non_inserted[i], 0));
        }
        auto it = min_element(mincost_noninserted.begin(), mincost_noninserted.end(), [&](auto A, auto B){return get<0>(A) < get<0>(B);});
        int selected_node = all_non_inserted[it - mincost_noninserted.begin()];
        if(get<0>(*it) < INF)
        {
            int insert_rid = get<2>(*it);
            int insert_pos = get<1>(*it);
            vrpsol.getOneRoute(insert_rid)->setRouteByInsertNode(insert_pos, selected_node);
            vrpsol.getNonInsertedNodes().erase(remove(vrpsol.getNonInsertedNodes().begin(), vrpsol.getNonInsertedNodes().end(), selected_node));
            vrpsol.getTotalDistBeforePlatooning() += get<0>(*it);
            vrpsol.getTotalUnservedRequests() -= 1;
            vrpsol.setSolCurOperation(VRPSolution::InsertOneNode);
        }
        all_non_inserted.erase(all_non_inserted.begin());
    }
}