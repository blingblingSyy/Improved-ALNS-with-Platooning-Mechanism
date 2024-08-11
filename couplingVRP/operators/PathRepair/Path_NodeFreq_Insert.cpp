#include "couplingVRP/operators/PathRepair/Path_NodeFreq_Insert.h"
#include "couplingVRP/model/VRPSolution.h"
#include "couplingVRP/model/ARoute.h"
#include "couplingVRP/model/Nodes.h"
#include "couplingVRP/model/ADijkstraSol.h"
#include "src/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
#include <set>
using namespace std;

Path_NodeFreq_Insert::Path_NodeFreq_Insert(string s, Nodes& nodes, double rand_nFreq) : nodeset(nodes), APathRepairOperator(s)
{
    empty = false;
    hasSelectedCurOp = true;
    toSelectNextOp = true;
    this->randF = rand_nFreq;
}

Path_NodeFreq_Insert::~Path_NodeFreq_Insert()
{

}

void Path_NodeFreq_Insert::repairSolPath(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    for(auto pos_it = vrpsol.getDestroyedArcsPos().begin(); pos_it != vrpsol.getDestroyedArcsPos().end(); )
    {
        auto config_it = find(vrpsol.getDestroyableArcPos().begin(), vrpsol.getDestroyableArcPos().end(), (*pos_it));
        vector<int> arc_config = vrpsol.getDestroyableArcConfig()[config_it - vrpsol.getDestroyableArcPos().begin()];
        vector<ADijkstraSol*> avail_path_set_it = nodeset.getAvailPathSet(arc_config[0], arc_config[1]);
        int old_usedpath = get<1>(*pos_it);
        vector<double> path_avg_nodefreq;
        for(int i = 0; i < avail_path_set_it.size(); i++)
        {
            if(i != old_usedpath)
            {
                path_avg_nodefreq.push_back(calPathAvgNodeFreq(vrpsol, avail_path_set_it[i]->getPath()));
            }
            else
            {
                path_avg_nodefreq.push_back(0);
            }
        }
        vector<int> indices(avail_path_set_it.size());
        iota(indices.begin(), indices.end(), 0);
        sort(indices.begin(), indices.end(), [&](int A, int B) -> bool {return path_avg_nodefreq[A] > path_avg_nodefreq[B];});
        RandomNumber r1;
        int new_usedpath = indices[int(pow(r1.get_rint(0,1), randF)*(indices.size()-1))];
        while(new_usedpath == old_usedpath)
        {
            new_usedpath = int(pow(r1.get_rint(0,1), randF)*(indices.size()-1));
        }
        vrpsol.repairPath(get<0>(*pos_it), new_usedpath, get<2>(*pos_it));
        vrpsol.getDestroyedArcsPos().erase(pos_it);
    }
}

double Path_NodeFreq_Insert::calPathAvgNodeFreq(VRPSolution& vrpsol, vector<int> path)
{
    vector<int> avg_freq;
    set<int> unique_middle_nodes = set(path.begin()+1, path.end()-1);
    for(auto it = unique_middle_nodes.begin(); it != unique_middle_nodes.end(); it++)
    {
        avg_freq.push_back(vrpsol.getNodeShowTimes(*it));
    }
    return calAverage(avg_freq);
}