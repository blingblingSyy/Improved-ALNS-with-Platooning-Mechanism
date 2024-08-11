#include "couplingVRP/operators/PathDestroy/Path_Random_Removal.h"
#include "couplingVRP/model/VRPSolution.h"
#include "couplingVRP/model/ARoute.h"
#include "couplingVRP/model/Nodes.h"
#include "src/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
#include <set>
using namespace std;

Path_Random_Removal::Path_Random_Removal(string s, ALNS_Parameters& alns_param, Nodes& nodes) : APathDestroyOperator(s, alns_param, nodes.getAllAvailPathSize())
{
    empty = false;
    hasSelectedCurOp = true;
    toSelectNextOp = true;
}

Path_Random_Removal::~Path_Random_Removal()
{

}

void Path_Random_Removal::destroySolPath(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    vector<tuple<int, int, int>> all_destroyable_arcpos = vrpsol.getDestroyableArcPos();  //! <destroyed_arcpos, destroyed_pathid, routeid>
    vector<vector<int>> all_destroyable_arcconfig = vrpsol.getDestroyableArcConfig(); //! {node1, node2}
    vrpsol.getDestroyedArcsPos().clear();
    destroyed_arc_config.clear();
    calPathDestroySize(all_destroyable_arcpos.size()); //! pathDestroySize <= all_destroyable_arcpos.size()

    int i = 0;
    while(i < pathDestroySize)
    {
        RandomNumber r1;
        int p = r1.get_rint(0, all_destroyable_arcpos.size()-1);
        tuple<int, int, int> selected_arcpos = all_destroyable_arcpos[p]; //! <destroyed_arcpos, destroyed_pathid, routeid>
        vector<int> selected_arcconfig = all_destroyable_arcconfig[p]; //! {node1, node2}
        if(pathTabu[selected_arcconfig[0]][selected_arcconfig[1]] == 0) //! after the tabu tenure
        {
            vrpsol.getDestroyedArcsPos().push_back(selected_arcpos);
            destroyed_arc_config.insert({selected_arcconfig[0], selected_arcconfig[1]});
            pathTabu[selected_arcconfig[0]][selected_arcconfig[1]] = tabu_tenure;
        }
        all_destroyable_arcpos.erase(all_destroyable_arcpos.begin()+p);
        all_destroyable_arcconfig.erase(all_destroyable_arcconfig.begin()+p);
        i++;
    }

    if(destroyed_arc_config.size() == 0) 
    {
        setEmpty(true);
        setToSelectNext(false);
    }
}