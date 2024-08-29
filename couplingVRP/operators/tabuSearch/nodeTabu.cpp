#include "couplingVRP/operators/tabuSearch/NodeTabu.h"
#include "couplingVRP/model/establish/VRPSolution.h"
#include <vector>
#include <tuple>
#include <algorithm>
using namespace std;

void NodeTabu::update(ISolution& sol, ALNS_Iteration_Status& status)
{
	VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    vrpsol.getDestroyedArcsPos().clear();
}

void NodeTabu::keepRemovablePos(VRPSolution& vrpsol, vector<pair<int, int>>& all_cus_pos)
{
    //! find out the positions that cannot be removed
    vector<pair<int, int>> non_remove_pos;
    vector<tuple<int, int, int>> all_modified_arcs = vrpsol.getDestroyedArcsPos();
    for(int a = 0; a < all_modified_arcs.size(); a++)
    {
        int rid = get<2>(all_modified_arcs[a]);
        int arcpos = get<0>(all_modified_arcs[a]);
        non_remove_pos.push_back(make_pair(rid, arcpos));
        non_remove_pos.push_back(make_pair(rid, arcpos+1));
    }
    sort(non_remove_pos.begin(), non_remove_pos.end(), [&](pair<int, int> A, pair<int, int> B) {return A > B;});
    sort(all_cus_pos.begin(), all_cus_pos.end(), [&](pair<int, int> A, pair<int, int> B) {return A < B;});
    //! erase the unremovable positions from the all_cus_pos
    for(auto it = non_remove_pos.begin(); it != non_remove_pos.end(); it++)
    {
        all_cus_pos.erase(remove(all_cus_pos.begin(), all_cus_pos.end(), *it));
    }
}

void NodeTabu::keepRepairablePos(VRPSolution& vrpsol, vector<pair<int, int>>& all_cus_pos)
{

}