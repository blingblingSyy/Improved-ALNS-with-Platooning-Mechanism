#include "couplingVRP/operators/pathDestroyOps/Path_AvgLenDiff_Removal.h"
#include "couplingVRP/model/establish/VRPSolution.h"
#include "couplingVRP/model/establish/ARoute.h"
#include "couplingVRP/model/basic/Nodes.h"
#include "couplingVRP/model/basic/ADijkstraSol.h"
#include "couplingVRP/operators/tabuSearch/pathTabu.h"
#include "couplingVRP/operators/Operators_Parameters.h"
#include "src/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
#include <set>
using namespace std;

Path_AvgLenDiff_Removal::Path_AvgLenDiff_Removal(string s, Operators_Parameters& ops_param, PathTabu& tabuObj, Nodes& nodes) : nodeset(nodes), Path_Random_Removal(s, ops_param, tabuObj)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
    randSel = ops_param.getRandAvgLenDes();
}

vector<double> Path_AvgLenDiff_Removal::calMeasurement(VRPSolution& vrpsol, vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig)
{
    vector<double> destroyable_arc_avglen;
    for(int i = 0; i < all_destroyable_arcpos.size(); i++)
    {
        if(pathTabuObj->getPathTenure(all_destroyable_arcconfig[i]) <= 0)
        {
            destroyable_arc_avglen.push_back(-INF);  //! if the path is tabu, cannot destroy it
        }
        else
        {
            vector<ADijkstraSol*> avail_path_set_i = nodeset.getAvailPathSet(all_destroyable_arcconfig[i][0], all_destroyable_arcconfig[i][1]);
            int selected_usedpath_id = get<1>(all_destroyable_arcpos[i]);
            vector<double> dist_diff_i;
            for(int p = 0; p < avail_path_set_i.size(); p++)
            {
                if(p != selected_usedpath_id)
                {
                    dist_diff_i.push_back(avail_path_set_i[p]->getDist() - avail_path_set_i[selected_usedpath_id]->getDist());
                }
            }
            destroyable_arc_avglen.push_back(calAverage(dist_diff_i));
        }
    }
    return destroyable_arc_avglen;
}

/*old version*/
// void Path_AvgLenDiff_Removal::destroySolPath(ISolution& sol)
// {
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     vector<tuple<int, int, int>> all_destroyable_arcpos = vrpsol.getDestroyableArcPos();  //! <destroyed_arcpos, destroyed_pathid, routeid>
//     vector<vector<int>> all_destroyable_arcconfig = vrpsol.getDestroyableArcConfig(); //! {node1, node2}
//     calPathDestroySize(all_destroyable_arcpos.size()); //! pathDestroySize <= all_destroyable_arcpos.size()
//     vrpsol.getDestroyedArcsPos().clear();
//     destroyed_arc_config.clear();
//     vector<int> destroyable_arc_avglen;
//     for(int i = 0; i < all_destroyable_arcpos.size(); i++)
//     {
//         vector<ADijkstraSol*> avail_path_set_i = nodeset.getAvailPathSet(all_destroyable_arcconfig[i][0], all_destroyable_arcconfig[i][1]);
//         int selected_usedpath_id = get<1>(all_destroyable_arcpos[i]);
//         vector<double> dist_diff_i;
//         for(int p = 0; p < avail_path_set_i.size(); p++)
//         {
//             if(p != selected_usedpath_id)
//             {
//                 dist_diff_i.push_back(avail_path_set_i[p]->getDist() - avail_path_set_i[selected_usedpath_id]->getDist());
//             }
//         }
//         destroyable_arc_avglen.push_back(calAverage(dist_diff_i));
//     }
//     vector<int> indices(destroyable_arc_avglen.size());
//     iota(indices.begin(), indices.end(), 0);
//     sort(indices.begin(), indices.end(), [&](int A, int B) -> bool {return destroyable_arc_avglen[A] < destroyable_arc_avglen[B];});
//     int i = 0;
//     while(i < pathDestroySize)
//     {
//         tuple<int, int, int> selected_arcpos = all_destroyable_arcpos[indices[0]]; //! <destroyed_arcpos, destroyed_pathid, routeid>
//         vector<int> selected_arcconfig = all_destroyable_arcconfig[indices[0]]; //! {node1, node2}
//         if(pathTabu[selected_arcconfig[0]][selected_arcconfig[1]] == 0) //! after the tabu tenure
//         {
//             vrpsol.getDestroyedArcsPos().push_back(selected_arcpos);
//             destroyed_arc_config.insert({selected_arcconfig[0], selected_arcconfig[1]});
//             pathTabu[selected_arcconfig[0]][selected_arcconfig[1]] = tabu_tenure + 1; //! in the update process, it will minus 1 again
//         }
// //         all_destroyable_arcpos.erase(all_destroyable_arcpos.begin()+indices[0]);
// //         all_destroyable_arcconfig.erase(all_destroyable_arcconfig.begin()+indices[0]);
//         indices.erase(indices.begin());
//         i++;
//     }
//     // if(destroyed_arc_config.size() == 0) 
//     // {
//     //     setEmpty(true);
//     //     setToSelectNext(false);
//     // }
// }