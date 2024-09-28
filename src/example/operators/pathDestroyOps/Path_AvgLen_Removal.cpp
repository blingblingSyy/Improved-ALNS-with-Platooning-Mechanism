#include "example/operators/pathDestroyOps/Path_AvgLen_Removal.h"
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

Path_AvgLen_Removal::Path_AvgLen_Removal(string s, Operators_Parameters& ops_param, PathTabu& tabuObj, Nodes& nodes) : Path_Random_Removal(s, ops_param, tabuObj, nodes)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
    randSel = ops_param.getRandAvgLenDes();
}

vector<double> Path_AvgLen_Removal::calMeasurement(VRPSolution& vrpsol, vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig)
{
    vector<double> destroyable_arc_len;
    for(int i = 0; i < all_destroyable_arcpos.size(); i++)
    {
        if(pathTabuObj->getPathTenure(all_destroyable_arcconfig[i]) != 0)
        {
            destroyable_arc_len.push_back(-INF);  //! if the path is tabu, cannot destroy it
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
                    dist_diff_i.push_back(avail_path_set_i[p]->getDist());
                }
            }
            // destroyable_arc_len.push_back(calAverage(dist_diff_i));
            RandomNumber r;
            if(!noise)
            {
                destroyable_arc_len.push_back(calAverage(dist_diff_i));
            }
            else
            {
                destroyable_arc_len.push_back(calAverage(dist_diff_i) * r.get_rflt(0.9, 1));
            }
        }
    }
    return destroyable_arc_len;
}