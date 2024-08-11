#include "couplingVRP/operators/pathDestroy/Path_Cardinality_Removal.h"
#include "couplingVRP/model/establish/VRPSolution.h"
#include "couplingVRP/model/establish/ARoute.h"
#include "couplingVRP/model/basic/Nodes.h"
#include "src/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
#include <set>
using namespace std;

Path_Cardinality_Removal::Path_Cardinality_Removal(string s, ALNS_Parameters& alns_param, Nodes& nodes) : APathDestroyOperator(s, alns_param, nodes.getAllAvailPathSize())
{
    empty = false;
    hasSelectedCurOp = true;
    toSelectNextOp = true;
}

Path_Cardinality_Removal::~Path_Cardinality_Removal()
{

}

void Path_Cardinality_Removal::destroySolPath(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    vector<tuple<int, int, int>> all_destroyable_arcpos = vrpsol.getDestroyableArcPos();  //! <destroyed_arcpos, destroyed_pathid, routeid>
    vector<vector<int>> all_destroyable_arcconfig = vrpsol.getDestroyableArcConfig(); //! {node1, node2}
    calPathDestroySize(all_destroyable_arcpos.size()); //! pathDestroySize <= all_destroyable_arcpos.size()
    vrpsol.getDestroyedArcsPos().clear();
    destroyed_arc_config.clear();

    vector<int> destroyable_arc_card;
    for(int i = 0; i < all_destroyable_arcconfig.size(); i++)
    {
        destroyable_arc_card.push_back(availPathSetCardi[all_destroyable_arcconfig[i][0]][all_destroyable_arcconfig[i][1]]);
    }
    vector<int> indices(destroyable_arc_card.size());
    iota(indices.begin(), indices.end(), 0);
    sort(indices.begin(), indices.end(), [&](int A, int B) -> bool {return destroyable_arc_card[A] < destroyable_arc_card[B];});

    int i = 0;
    while(i < pathDestroySize)
    {
        tuple<int, int, int> selected_arcpos = all_destroyable_arcpos[indices[0]]; //! <destroyed_arcpos, destroyed_pathid, routeid>
        vector<int> selected_arcconfig = all_destroyable_arcconfig[indices[0]]; //! {node1, node2}
        if(pathTabu[selected_arcconfig[0]][selected_arcconfig[1]] == 0) //! after the tabu tenure
        {
            vrpsol.getDestroyedArcsPos().push_back(selected_arcpos);
            destroyed_arc_config.insert({selected_arcconfig[0], selected_arcconfig[1]});
            pathTabu[selected_arcconfig[0]][selected_arcconfig[1]] = tabu_tenure;
        }
        all_destroyable_arcpos.erase(all_destroyable_arcpos.begin()+indices[0]);
        all_destroyable_arcconfig.erase(all_destroyable_arcconfig.begin()+indices[0]);
        indices.erase(indices.begin());
        i++;
    }

    if(destroyed_arc_config.size() == 0) 
    {
        setEmpty(true);
        setToSelectNext(false);
    }
}