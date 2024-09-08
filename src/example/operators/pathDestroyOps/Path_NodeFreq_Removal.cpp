#include "example/operators/pathDestroyOps/Path_NodeFreq_Removal.h"
#include "example/model/establish/VRPSolution.h"
#include "example/model/establish/ARoute.h"
#include "example/model/basic/Nodes.h"
#include "example/model/basic/ADijkstraSol.h"
#include "example/operators/tabuSearch/pathTabu.h"
#include "example/operators/Operators_Parameters.h"
#include "alns/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
#include <set>
using namespace std;

Path_NodeFreq_Removal::Path_NodeFreq_Removal(string s, Operators_Parameters& ops_param, PathTabu& tabuObj, Nodes& nodes) : nodeset(nodes), Path_Random_Removal(s, ops_param, tabuObj)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
    randSel = ops_param.getRandNfreqDes();
}

double Path_NodeFreq_Removal::calPathAvgNodeFreq(VRPSolution& vrpsol, vector<int> path)
{
    vector<int> avg_freq;
    set<int> unique_middle_nodes = set(path.begin()+1, path.end()-1);
    for(auto it = unique_middle_nodes.begin(); it != unique_middle_nodes.end(); it++)
    {
        avg_freq.push_back(vrpsol.getNodeShowTimes(*it));
    }
    return calAverage(avg_freq);
}

vector<double> Path_NodeFreq_Removal::calMeasurement(VRPSolution& vrpsol, vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig)
{
    vector<double> destroyable_arc_avg_nodefreq;
    for(int i = 0; i < all_destroyable_arcpos.size(); i++)
    {
        if(pathTabuObj->getPathTenure(all_destroyable_arcconfig[i]) <= 0)
        {
            destroyable_arc_avg_nodefreq.push_back(-INF);
        }
        else
        {
            int selected_usedpath_id = get<1>(all_destroyable_arcpos[i]);
            vector<int> destroyable_path_i = nodeset.getOnePath(all_destroyable_arcconfig[i][0], all_destroyable_arcconfig[i][1], selected_usedpath_id)->getPath();
            destroyable_arc_avg_nodefreq.push_back(calPathAvgNodeFreq(vrpsol, destroyable_path_i));
        }
    }
    return destroyable_arc_avg_nodefreq;
}

/*old version*/
// void Path_NodeFreq_Removal::destroySolPath(ISolution& sol)
// {
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     vector<tuple<int, int, int>> all_destroyable_arcpos = vrpsol.getDestroyableArcPos();  //! <destroyed_arcpos, destroyed_pathid, routeid>
//     vector<vector<int>> all_destroyable_arcconfig = vrpsol.getDestroyableArcConfig(); //! {node1, node2}
//     calPathDestroySize(all_destroyable_arcpos.size()); //! pathDestroySize <= all_destroyable_arcpos.size()
//     vrpsol.getDestroyedArcsPos().clear();
//     destroyed_arc_config.clear();
//     vector<double> destroyable_arc_avg_nodefreq;
//     for(int i = 0; i < all_destroyable_arcpos.size(); i++)
//     {
//         int selected_usedpath_id = get<1>(all_destroyable_arcpos[i]);
//         vector<int> destroyable_path_i = nodeset.getOnePath(all_destroyable_arcconfig[i][0], all_destroyable_arcconfig[i][1], selected_usedpath_id)->getPath();
//         destroyable_arc_avg_nodefreq.push_back(calPathAvgNodeFreq(vrpsol, destroyable_path_i));
//     }
//     vector<int> indices(destroyable_arc_avg_nodefreq.size());
//     iota(indices.begin(), indices.end(), 0);
//     sort(indices.begin(), indices.end(), [&](int A, int B) -> bool {return destroyable_arc_avg_nodefreq[A] < destroyable_arc_avg_nodefreq[B];});
//     int i = 0;
//     while(i < pathDestroySize)
//     {
//         tuple<int, int, int> selected_arcpos = all_destroyable_arcpos[indices[0]]; //! <destroyed_arcpos, destroyed_pathid, routeid>
//         vector<int> selected_arcconfig = all_destroyable_arcconfig[indices[0]]; //! {node1, node2}
//         if(pathTabu[selected_arcconfig[0]][selected_arcconfig[1]] == 0) //! after the tabu tenure
//         {
//             vrpsol.getDestroyedArcsPos().push_back(selected_arcpos);
//             destroyed_arc_config.insert({selected_arcconfig[0], selected_arcconfig[1]});
//             pathTabu[selected_arcconfig[0]][selected_arcconfig[1]] = tabu_tenure + 1;
//         }
//         all_destroyable_arcpos.erase(all_destroyable_arcpos.begin()+indices[0]);
//         all_destroyable_arcconfig.erase(all_destroyable_arcconfig.begin()+indices[0]);
//         indices.erase(indices.begin());
//         i++;
//     }
//     // if(destroyed_arc_config.size() == 0) 
//     // {
//     //     setEmpty(true);
//     //     setToSelectNext(false);
//     // }
// }
