#include "example/operators/pathRepairOps/Path_Random_Insert.h"
#include "example/model/establish/VRPSolution.h"
#include "example/model/establish/ARoute.h"
#include "example/model/basic/Nodes.h"
#include "alns/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
#include <set>
using namespace std;

Path_Random_Insert::Path_Random_Insert(string s, Nodes& nodes) : nodeset(nodes), APathRepairOperator(s)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
    randSel = 1;
}

Path_Random_Insert::~Path_Random_Insert()
{

}

void Path_Random_Insert::repairSolPath(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    // vrpsol.getRepairedArcsPos().clear();
    vrpsol.findDestroyablePaths();
    for(auto pos_it = vrpsol.getDestroyedArcsPos().begin(); pos_it != vrpsol.getDestroyedArcsPos().end(); pos_it++)
    {
        // vector<tuple<int, int, int>> destroyable_arcpos = vrpsol.getDestroyableArcPos();
        // auto config_it = find(destroyable_arcpos.begin(), destroyable_arcpos.end(), (*pos_it));
        // vector<int> arc_config = vrpsol.getDestroyableArcConfig()[config_it - destroyable_arcpos.begin()];
        vector<int> arc_config = vrpsol.getDestroyedArcConfig()[pos_it - vrpsol.getDestroyedArcsPos().begin()];
        vector<int> indices = sortMeasurement(vrpsol, arc_config, get<1>(*pos_it));
        RandomNumber r1;
        int new_usedpath = indices[static_cast<int>(round(pow(r1.get_rflt(0,1), randSel)*(indices.size()-1)))];
        vrpsol.repairPath(get<0>(*pos_it), new_usedpath, get<2>(*pos_it));
        // vrpsol.getRepairedArcsPos().push_back(*pos_it);
        // pos_it = vrpsol.getDestroyedArcsPos().erase(pos_it);
    }
    // vrpsol.getDestroyedArcsPos().clear();
}

vector<int> Path_Random_Insert::sortMeasurement(VRPSolution& vrpsol, vector<int> arc_config, int orig_usedpath)
{
    vector<int> indices(static_cast<int>(nodeset.getAllAvailPathSize()[arc_config[0]][arc_config[1]]));
    iota(indices.begin(), indices.end(), 0);
    indices.erase(remove(indices.begin(), indices.end(), orig_usedpath));   //! orig_usedpath = get<1>(*pos_it): the index for the old used path
    return indices;
}

/*old version*/
// void Path_Random_Insert::repairSolPath(ISolution& sol)
// {
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     for(auto pos_it = vrpsol.getDestroyedArcsPos().begin(); pos_it != vrpsol.getDestroyedArcsPos().end(); )
//     {
//         auto config_it = find(vrpsol.getDestroyableArcPos().begin(), vrpsol.getDestroyableArcPos().end(), (*pos_it));
//         vector<int> arc_config = vrpsol.getDestroyableArcConfig()[config_it - vrpsol.getDestroyableArcPos().begin()];
//         int card = nodeset.getAllAvailPathSize()[arc_config[0]][arc_config[1]];
//         vector<int> indices(card);
//         iota(indices.begin(), indices.end(), 0);
//         indices.erase(remove(indices.begin(), indices.end(), get<1>(*pos_it)));   //!  get<1>(*pos_it): the index for the old used path
//         RandomNumber r1;
//         int new_usedpath = indices[int(pow(r1.get_rint(0,1), randSel)*(indices.size()-1))];
//         vrpsol.repairPath(get<0>(*pos_it), new_usedpath, get<2>(*pos_it));
//         pos_it = vrpsol.getDestroyedArcsPos().erase(pos_it);
//     }
//     vrpsol.getDestroyedArcsPos().clear();
// }