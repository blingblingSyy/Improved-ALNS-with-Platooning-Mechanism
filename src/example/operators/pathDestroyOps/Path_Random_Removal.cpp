#include "example/operators/pathDestroyOps/Path_Random_Removal.h"
#include "example/operators/tabuSearch/pathTabu.h"
#include "example/model/establish/VRPSolution.h"
#include "example/model/establish/ARoute.h"
#include "example/model/basic/Nodes.h"
#include "example/operators/Operators_Parameters.h"
#include "example/model/basic/ADijkstraSol.h"
#include "alns/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
#include <set>
using namespace std;

Path_Random_Removal::Path_Random_Removal(string s, Operators_Parameters& ops_param, PathTabu& tabuObj, Nodes& nodes) : nodeset(nodes), APathDestroyOperator(s, ops_param.getPathDestroySizeRate(), tabuObj)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
    randSel = 1;
}

vector<double> Path_Random_Removal::calMeasurement(VRPSolution& vrpsol, vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig)
{
    RandomNumber r;
    vector<double> rand_measure;
    for(int i = 0; i < all_destroyable_arcconfig.size(); i++)
    {
        (pathTabuObj->getPathTenure(all_destroyable_arcconfig[i]) == 0) ? rand_measure.push_back(r.get_rflt(0,1)) : rand_measure.push_back(-INF);
    }
    return rand_measure;
}

void Path_Random_Removal::destroySolPath(ISolution& sol)
{
    //! get the set of destroyable paths
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    vrpsol.findDestroyablePaths();
    vector<tuple<int, int, int>> all_destroyable_arcpos = vrpsol.getDestroyableArcPos();  //! <destroyed_arcpos, destroyed_pathid, routeid>
    vector<vector<int>> all_destroyable_arcconfig = vrpsol.getDestroyableArcConfig(); //! {node1, node2}
    vrpsol.getDestroyedArcsPos().clear(); //! clear for the current iteration
    vrpsol.getDestroyedArcConfig().clear();

    //! calculate the average difference of length between alternative paths for each specific path
    path_measurement = calMeasurement(vrpsol, all_destroyable_arcpos, all_destroyable_arcconfig);

    //! rank the measurement for all paths from largest to smallest
    vector<int> indices(path_measurement.size());
    iota(indices.begin(), indices.end(), 0);
    sort(indices.begin(), indices.end(), [&](int A, int B) -> bool {return path_measurement[A] > path_measurement[B];});
    for(auto iter = indices.begin(); iter != indices.end(); )
    {
        if(path_measurement[*iter] == -INF)
        {
            iter = indices.erase(iter);
        }
        else
        {
            iter++;
        }
    }
    // indices.erase(remove_if(indices.begin(), indices.end(), [&](int i){return path_measurement[i] == -INF;}), indices.end());
    RandomNumber r;
    int destroyableSize = static_cast<int>(ceil(indices.size() * pathDesSizeRate * r.get_rflt(0,1)));

    while(destroyableSize > 0)
    {
        //! randomly select a destroyable path
        RandomNumber r;
        int select_indpos = static_cast<int>(round(pow(r.get_rflt(0,1), randSel)*(indices.size()-1)));
        tuple<int, int, int> selected_arcpos = all_destroyable_arcpos[indices[select_indpos]]; //! <destroyed_arcpos, destroyed_pathid, routeid>
        vector<int> selected_arcconfig = all_destroyable_arcconfig[indices[select_indpos]]; //! {node1, node2}
        //! destroy the selected path
        vrpsol.getDestroyedArcsPos().push_back(selected_arcpos);
        vrpsol.getDestroyedArcConfig().push_back(selected_arcconfig);
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

/*old version*/
// void Path_Random_Removal::destroySolPath(ISolution& sol)
// {
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     vector<tuple<int, int, int>> all_destroyable_arcpos = vrpsol.getDestroyableArcPos();  //! <destroyed_arcpos, destroyed_pathid, routeid>
//     vector<vector<int>> all_destroyable_arcconfig = vrpsol.getDestroyableArcConfig(); //! {node1, node2}
//     vrpsol.getDestroyedArcsPos().clear();
//     destroyed_arc_config.clear();
//     calPathDestroySize(all_destroyable_arcpos.size()); //! pathDestroySize <= all_destroyable_arcpos.size()
//     int i = 0;
//     while(i < pathDestroySize)
//     {
//         RandomNumber r1;
//         int p = r1.get_rint(0, all_destroyable_arcpos.size()-1);
//         tuple<int, int, int> selected_arcpos = all_destroyable_arcpos[p]; //! <destroyed_arcpos, destroyed_pathid, routeid>
//         vector<int> selected_arcconfig = all_destroyable_arcconfig[p]; //! {node1, node2}
//         if(pathTabu[selected_arcconfig[0]][selected_arcconfig[1]] == 0) //! after the tabu tenure
//         {
//             vrpsol.getDestroyedArcsPos().push_back(selected_arcpos);
//             destroyed_arc_config.insert({selected_arcconfig[0], selected_arcconfig[1]});
//             pathTabu[selected_arcconfig[0]][selected_arcconfig[1]] = tabu_tenure + 1;
//         }
//         all_destroyable_arcpos.erase(all_destroyable_arcpos.begin()+p);
//         all_destroyable_arcconfig.erase(all_destroyable_arcconfig.begin()+p);
//         i++;
//     }
//     // if(destroyed_arc_config.size() == 0) 
//     // {
//     //     setEmpty(true);
//     //     setToSelectNext(false);
//     // }
// }