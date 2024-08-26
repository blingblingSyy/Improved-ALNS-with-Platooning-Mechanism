#include "couplingVRP/operators/pathDestroyOps/Path_Removal_Template.h"
#include "couplingVRP/model/establish/VRPSolution.h"
#include "couplingVRP/model/establish/ARoute.h"
#include "couplingVRP/model/basic/Nodes.h"
#include "couplingVRP/operators/tabuSearch/pathTabu.h"
#include "src/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
#include <set>
using namespace std;

Path_Removal_Template::Path_Removal_Template(string s, ALNS_Parameters& alns_param, PathTabu& tabuObj) : APathDestroyOperator(s, alns_param, tabuObj)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
}

Path_Removal_Template::~Path_Removal_Template()
{

}

void Path_Removal_Template::destroySolPath(ISolution& sol)
{
    //! get the set of destroyable paths
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    vector<tuple<int, int, int>> all_destroyable_arcpos = vrpsol.getDestroyableArcPos();  //! <destroyed_arcpos, destroyed_pathid, routeid>
    vector<vector<int>> all_destroyable_arcconfig = vrpsol.getDestroyableArcConfig(); //! {node1, node2}
    vrpsol.getDestroyedArcsPos().clear();

    //! calculate the average difference of length between alternative paths for each specific path
    path_measurement = calMeasurement(vrpsol, all_destroyable_arcpos, all_destroyable_arcconfig);

    //! rank the measurement for all paths from largest to smallest
    vector<int> indices(path_measurement.size());
    iota(indices.begin(), indices.end(), 0);
    sort(indices.begin(), indices.end(), [&](int A, int B) -> bool {return path_measurement[A] > path_measurement[B];});
    indices.erase(remove_if(indices.begin(), indices.end(), [&](int i){return path_measurement[i] == -INF;}));
    int destroyableSize = indices.size() * pathDesSizeRate;

    while(destroyableSize > 0)
    {
        //! randomly select a destroyale path
        RandomNumber r;
        int select_indpos = int(pow(r.get_rflt(0,1), randSel)*indices.size());
        tuple<int, int, int> selected_arcpos = all_destroyable_arcpos[indices[select_indpos]]; //! <destroyed_arcpos, destroyed_pathid, routeid>
        vector<int> selected_arcconfig = all_destroyable_arcconfig[indices[select_indpos]]; //! {node1, node2}
        //! destroy the selected path
        vrpsol.getDestroyedArcsPos().push_back(selected_arcpos);
        //! update the tabu tenure for the selected path
        pathTabuObj->updatePathTenure(selected_arcconfig);
        //! remove the path from the set of destroyable paths
        indices.erase(indices.begin()+select_indpos);
        destroyableSize--;
    }

    // if(destroyed_arc_config.size() == 0) 
    // {
    //     setEmpty(true);
    //     setToSelectNext(false);
    // }
}