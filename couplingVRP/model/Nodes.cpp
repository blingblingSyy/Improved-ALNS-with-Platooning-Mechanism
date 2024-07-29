#include "couplingVRP/model/Nodes.h"
#include "couplingVRP/model/config.h"
#include "couplingVRP/model/RawInstance.h"
#include "couplingVRP/model/NodesManager.h"
#include "couplingVRP/model/ADijkstraSol.h"
#include "couplingVRP/model/AvailablePaths.h"

class RawInstance;
class NodesManager;
class ADijkstraSol;
class AvailablePaths;

Nodes::Nodes(RawInstance& inputInstance, NodesManager& NodesMan, bool add_intersects, bool shrink_pasdmd, bool modify_connectivity, int veh_speed)
{
    this->rawInstance = inputInstance;
    this->NodesMan = NodesMan;
    this->add_intersects = add_intersects;
    this->shrink_pasdmd = shrink_pasdmd;
    this->modify_connectivity = modify_connectivity;
    nodenum = rawInstance.getRowNum()-1;
    buildNodesStruct(veh_speed);
}

void Nodes::buildNodesStruct(int veh_speed)
{
    nodetype = NodesMan.set_nodetype(nodenum, add_intersects, PAS_REQ_PROP); //{2, 2, 2, 0, 1, 2, 0} for test1.txt
    demands = rawInstance.extract_demands();
    NodesMan.modify_demands(demands, nodetype, shrink_pasdmd);
    coordinates = rawInstance.extract_coordinates();
    initial_distmat = NodesMan.cal_init_distmat(coordinates);
    modified_distmat = (modify_connectivity) ? NodesMan.modify_init_distmat(initial_distmat) : initial_distmat;
    initial_timemat = NodesMan.get_init_tvltime(modified_distmat, nodenum, veh_speed);

    // //self-defined data for test1.txt (deleted later)
    // modified_distmat[0][2] = INF;
    // modified_distmat[0][3] = INF;
    // modified_distmat[0][6] = INF;
    // modified_distmat[1][3] = INF;
    // modified_distmat[1][4] = INF;
    // modified_distmat[1][5] = INF;
    // modified_distmat[1][6] = INF;
    // modified_distmat[2][0] = INF;
    // modified_distmat[2][5] = INF;
    // modified_distmat[2][6] = INF;
    // modified_distmat[3][0] = INF;
    // modified_distmat[3][1] = INF;
    // modified_distmat[3][4] = INF;
    // modified_distmat[3][5] = INF;
    // modified_distmat[3][6] = INF;
    // modified_distmat[4][1] = INF;
    // modified_distmat[4][3] = INF;
    // modified_distmat[4][5] = INF;
    // modified_distmat[4][6] = INF;
    // modified_distmat[5][1] = INF;
    // modified_distmat[5][2] = INF;
    // modified_distmat[5][3] = INF;
    // modified_distmat[5][4] = INF;
    // modified_distmat[6][0] = INF;
    // modified_distmat[6][1] = INF;
    // modified_distmat[6][2] = INF;
    // modified_distmat[6][3] = INF;
    // modified_distmat[6][4] = INF;    

    neighbours = NodesMan.find_adjacent_nodes(modified_distmat, nodenum);
    service_time = rawInstance.extract_sertime();
    AvailablePaths altpathsets_obj(modified_distmat, nodenum, AVAIL_PATHSET_SIZE_K);
    SP_distmat = altpathsets_obj.getAllShortestPathDistance();
    avail_path_set = altpathsets_obj.getAllAvailablePathSet();
    travel_tw = NodesMan.cal_tvltw(SP_distmat[0], rawInstance.getPlanHorizon(), veh_speed);
    service_tw = NodesMan.calibrate_sertw(rawInstance.extract_sertw(), service_time, travel_tw);//get modified service time windows for each node
}