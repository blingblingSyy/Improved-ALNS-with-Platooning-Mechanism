#include "couplingVRP/operators/pathRepair/Path_Random_Insert.h"
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

Path_Random_Insert::Path_Random_Insert(string s, Nodes& nodes) : nodeset(nodes), APathRepairOperator(s)
{
    empty = false;
    hasSelectedCur = true;
    toSelectNext = true;
}

Path_Random_Insert::~Path_Random_Insert()
{

}

void Path_Random_Insert::repairSolPath(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    for(auto pos_it = vrpsol.getDestroyedArcsPos().begin(); pos_it != vrpsol.getDestroyedArcsPos().end(); )
    {
        auto config_it = find(vrpsol.getDestroyableArcPos().begin(), vrpsol.getDestroyableArcPos().end(), (*pos_it));
        vector<int> arc_config = vrpsol.getDestroyableArcConfig()[config_it - vrpsol.getDestroyableArcPos().begin()];
        int card = nodeset.getAllAvailPathSize()[arc_config[0]][arc_config[1]];
        int orig_usedpath = get<1>(*pos_it);
        RandomNumber r1;
        int rand_usedpath_id = r1.get_rint(0, card-1);
        while(rand_usedpath_id == orig_usedpath)
        {
            rand_usedpath_id = r1.get_rint(0, card-1);
        }
        vrpsol.repairPath(get<0>(*pos_it), rand_usedpath_id, get<2>(*pos_it));
        pos_it = vrpsol.getDestroyedArcsPos().erase(pos_it);
    }
}