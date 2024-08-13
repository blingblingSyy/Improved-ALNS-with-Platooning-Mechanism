#include "couplingVRP/operators/nodeRepair/Node_Regret_Insert.h"
#include "couplingVRP/model/establish/VRPSolution.h"
#include "couplingVRP/model/establish/ARoute.h"
#include "src/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
using namespace std;

Node_Regret_Insert::Node_Regret_Insert(string s, int regret_k) : ANodeRepairOperator(s)
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
    while(!all_non_inserted.empty())
    {
        vector<tuple<double, int, int>> mincost_noninserted;
        vector<double> regretval_noninserted;
        for(int i = 0; i < all_non_inserted.size(); i++)
        {
            vector<tuple<double, int, int>> ordered_insert_costs = vrpsol.calOrderedInsertCostsAllRoutes(all_non_inserted[i]);
            mincost_noninserted.push_back(ordered_insert_costs[0]);
            double regret_val_i;
            for(int k = 1; k < K; k++)
            {
                regret_val_i += get<0>(ordered_insert_costs[k]) - get<0>(ordered_insert_costs[0]);
            }
            regretval_noninserted.push_back(regret_val_i);
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
        }
        all_non_inserted.erase(all_non_inserted.begin());
    }
}