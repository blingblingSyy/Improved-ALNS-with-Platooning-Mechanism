#include "couplingVRP/operators/pathRepairOps/Path_KMaxLen_Insert.h"
#include "couplingVRP/model/establish/VRPSolution.h"
#include "couplingVRP/model/establish/ARoute.h"
#include "couplingVRP/model/basic/Nodes.h"
#include "couplingVRP/model/basic/ADijkstraSol.h"
#include "src/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
#include <numeric>
#include <set>
using namespace std;

Path_KMaxLen_Insert::Path_KMaxLen_Insert(string s, Nodes& nodes, double rand_klen) : Path_Random_Insert(s, nodes)
{
    this->randK = rand_klen;
}

Path_KMaxLen_Insert::~Path_KMaxLen_Insert()
{

}

vector<int> Path_KMaxLen_Insert::sortMeasurement(VRPSolution& vrpsol, vector<int> arc_config, int orig_usedpath)
{
    vector<ADijkstraSol*> avail_path_set_at_arc = nodeset.getAvailPathSet(arc_config[0], arc_config[1]);
    vector<int> indices(avail_path_set_at_arc.size());
    iota(indices.begin(), indices.end(), 0);
    indices.erase(remove(indices.begin(), indices.end(), orig_usedpath));   //!  orig_usedpath = get<1>(*pos_it): the index for the old used path
    sort(indices.begin(), indices.end(), [&](int A, int B) -> bool {return avail_path_set_at_arc[A]->getDist() > avail_path_set_at_arc[B]->getDist();});
}

/*old version*/
// void Path_KMaxLen_Insert::repairSolPath(ISolution& sol)
// {
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     for(auto pos_it = vrpsol.getDestroyedArcsPos().begin(); pos_it != vrpsol.getDestroyedArcsPos().end(); )
//     {
//         auto config_it = find(vrpsol.getDestroyableArcPos().begin(), vrpsol.getDestroyableArcPos().end(), (*pos_it));
//         vector<int> arc_config = vrpsol.getDestroyableArcConfig()[config_it - vrpsol.getDestroyableArcPos().begin()];
//         vector<ADijkstraSol*> avail_path_set_it = nodeset.getAvailPathSet(arc_config[0], arc_config[1]);
//         vector<int> indices(avail_path_set_it.size());
//         iota(indices.begin(), indices.end(), 0);
//         sort(indices.begin(), indices.end(), [&](int A, int B) -> bool {return avail_path_set_it[A]->getDist() > avail_path_set_it[B]->getDist();});
//         int old_usedpath = get<1>(*pos_it); //! the index for the old used path
//         RandomNumber r1;
//         int new_usedpath = indices[int(pow(r1.get_rint(0,1), randK)*(indices.size()-1))];
//         while(new_usedpath == old_usedpath)
//         {
//             new_usedpath = int(pow(r1.get_rint(0,1), randK)*(indices.size()-1));
//         }
//         vrpsol.repairPath(get<0>(*pos_it), new_usedpath, get<2>(*pos_it));
//         pos_it = vrpsol.getDestroyedArcsPos().erase(pos_it);
//     }
//     vrpsol.getDestroyedArcsPos().clear();
// }