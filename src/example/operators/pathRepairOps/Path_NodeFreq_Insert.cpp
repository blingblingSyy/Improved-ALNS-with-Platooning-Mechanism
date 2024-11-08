#include "example/operators/pathRepairOps/Path_NodeFreq_Insert.h"
#include "example/model/establish/VRPSolution.h"
#include "example/model/establish/ARoute.h"
#include "example/model/basic/Nodes.h"
#include "example/model/basic/ADijkstraSol.h"
#include "example/operators/Operators_Parameters.h"
#include "alns/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
#include <set>
using namespace std;

Path_NodeFreq_Insert::Path_NodeFreq_Insert(string s, Operators_Parameters& ops_param, Nodes& nodes) : Path_Random_Insert(s, nodes)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
    // this->randF = ops_param.getRandNfreqRep();
    randSel = ops_param.getRandNfreqRep();
}

Path_NodeFreq_Insert::~Path_NodeFreq_Insert()
{

}



double Path_NodeFreq_Insert::calPathAvgNodeFreq(VRPSolution& vrpsol, vector<int> path)
{
    vector<int> avg_freq;
    set<int> unique_middle_nodes = set(path.begin()+1, path.end()-1);
    for(auto it = unique_middle_nodes.begin(); it != unique_middle_nodes.end(); it++)
    {
        avg_freq.push_back(vrpsol.getNodeShowTimes(*it));
    }
    RandomNumber r;
    if(!noise)
    {
        return calAverage(avg_freq);
    }
    else
    {
        return calAverage(avg_freq) + r.get_rflt(-unique_middle_nodes.size(), unique_middle_nodes.size());
    }
}

vector<int> Path_NodeFreq_Insert::sortMeasurement(VRPSolution& vrpsol, vector<int> arc_config, int orig_usedpath)
{
    vector<ADijkstraSol*> avail_path_set_it = nodeset.getAvailPathSet(arc_config[0], arc_config[1]);
    vector<double> path_avg_nodefreq;
    for(int i = 0; i < avail_path_set_it.size(); i++)
    {
        if(i != orig_usedpath)
        {
            path_avg_nodefreq.push_back(calPathAvgNodeFreq(vrpsol, avail_path_set_it[i]->getPath()));
        }
        else
        {
            path_avg_nodefreq.push_back(-INF);
        }
    }
    vector<int> indices(avail_path_set_it.size());
    iota(indices.begin(), indices.end(), 0);
    indices.erase(remove(indices.begin(), indices.end(), orig_usedpath));   //!  orig_usedpath = get<1>(*pos_it): the index for the old used path
    sort(indices.begin(), indices.end(), [&](int A, int B) -> bool {return path_avg_nodefreq[A] > path_avg_nodefreq[B];});
    return indices;
}


/*old version*/
// void Path_NodeFreq_Insert::repairSolPath(ISolution& sol)
// {
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     for(auto pos_it = vrpsol.getDestroyedArcsPos().begin(); pos_it != vrpsol.getDestroyedArcsPos().end(); )
//     {
//         auto config_it = find(vrpsol.getDestroyableArcPos().begin(), vrpsol.getDestroyableArcPos().end(), (*pos_it));
//         vector<int> arc_config = vrpsol.getDestroyableArcConfig()[config_it - vrpsol.getDestroyableArcPos().begin()];
//         vector<ADijkstraSol*> avail_path_set_it = nodeset.getAvailPathSet(arc_config[0], arc_config[1]);
//         int old_usedpath = get<1>(*pos_it);
//         vector<double> path_avg_nodefreq;
//         for(int i = 0; i < avail_path_set_it.size(); i++)
//         {
//             if(i != old_usedpath)
//             {
//                 path_avg_nodefreq.push_back(calPathAvgNodeFreq(vrpsol, avail_path_set_it[i]->getPath()));
//             }
//             else
//             {
//                 path_avg_nodefreq.push_back(0);
//             }
//         }
//         vector<int> indices(avail_path_set_it.size());
//         iota(indices.begin(), indices.end(), 0);
//         indices.erase(remove(indices.begin(), indices.end(), old_usedpath));   //!  old_usedpath = get<1>(*pos_it): the index for the old used path
//         sort(indices.begin(), indices.end(), [&](int A, int B) -> bool {return path_avg_nodefreq[A] > path_avg_nodefreq[B];});
//         RandomNumber r1;
//         int new_usedpath = indices[int(pow(r1.get_rint(0,1), randF)*(indices.size()-1))];
//         vrpsol.repairPath(get<0>(*pos_it), new_usedpath, get<2>(*pos_it));
//         pos_it = vrpsol.getDestroyedArcsPos().erase(pos_it);
//     }
//     vrpsol.getDestroyedArcsPos().clear();
// }