#include <iostream>  
#include <vector>  
#include <cmath>
#include <cstdlib>  
#include <algorithm>  
#include <cassert>
#include "example/model/basic/Nodes.h"
#include "example/model/basic/RawInstance.h"
#include "example/model/basic/ADijkstraSol.h"
#include "example/model/basic/KSPBuilder.h"
#include "example/model/basic/config.h"
#include "utility.h"

#define NDEBUG

Nodes::Nodes(RawInstance& inputInstance)
{
    this->rawInstance = &inputInstance;
    this->design_demands = false; //! default = false;
    this->design_sertime = false; //! default = false;
    this->design_sertw = false;  //! default = false;
    this->add_intersects = false;  //! default = false;
    this->shrink_pasdmd = false;  //! default = false;
    this->modify_connectivity = false;  //! default = false;
    veh_speed = rawInstance->extract_vehspeed();
    nodenum = rawInstance->getNodesNum();
    nodetype = designNodeTypes(nodenum, add_intersects, PAS_REQ_PROP); //{2, 2, 2, 0, 1, 2, 0} for test1.txt; {2,1,2,0,1,0} for test2.txt
    buildNodesStruct();
}

Nodes::Nodes(RawInstance& inputInstance, vector<int> input_nodetype)
{
    this->rawInstance = &inputInstance;
    this->design_demands = false; //! default = false;
    this->design_sertime = false; //! default = false;
    this->design_sertw = false;  //! default = false;
    this->add_intersects = false;  //! default = false;
    this->shrink_pasdmd = false;  //! default = false;
    this->modify_connectivity = false;  //! default = false;
    veh_speed = rawInstance->extract_vehspeed();
    nodenum = rawInstance->getNodesNum();
    setNodeTypes(input_nodetype); //{2, 2, 2, 0, 1, 2, 0} for test1.txt; {2,1,2,0,1,0} for test2.txt
    buildNodesStruct();
}

Nodes::Nodes(RawInstance& inputInstance, vector<int> input_nodetype, vector<vector<int>> disconnected_links)
{
    this->rawInstance = &inputInstance;
    this->design_demands = false; //! default = false;
    this->design_sertime = false; //! default = false;
    this->design_sertw = false;  //! default = false;
    this->add_intersects = false;  //! default = false;
    this->shrink_pasdmd = false;  //! default = false;
    this->modify_connectivity = false;  //! default = false;
    veh_speed = rawInstance->extract_vehspeed();
    nodenum = rawInstance->getNodesNum();
    setNodeTypes(input_nodetype); //{2, 2, 2, 0, 1, 2, 0} for test1.txt; {2,1,2,0,1,0} for test2.txt
    this->disconnected_links = disconnected_links;
    buildNodesStruct();
}

Nodes::Nodes()
{
    this->design_demands = false; //! default = false;
    this->design_sertime = false; //! default = false;
    this->design_sertw = false;  //! default = false;
    this->add_intersects = false;  //! default = false;
    this->shrink_pasdmd = false;  //! default = false;
    this->modify_connectivity = false;  //! default = false;
}

Nodes::~Nodes()
{
    // delete rawInstance; //! this is deleting scalar (existing inputInstance), not deleting new memory
    for(int i = 0; i < avail_path_set.size(); i++)
    {
        for(int j = 0; j < avail_path_set[i].size(); j++)
        {
            if(i != j)
            {
                for(int z = 0; z < avail_path_set[i][j].size(); z++)
                {
                    delete avail_path_set[i][j][z];
                }
            }
        }
    }
    delete avail_path_set[0][0][0];
}

void Nodes::end()
{
    for(int i = 0; i < avail_path_set.size(); i++)
    {
        for(int j = 0; j < avail_path_set[i].size(); j++)
        {
            if(i != j)
            {
                for(int z = 0; z < avail_path_set[i][j].size(); z++)
                {
                    delete avail_path_set[i][j][z];
                }
            }
        }
    }
    delete avail_path_set[0][0][0];
}

/*old version*/
void Nodes::buildNodesStruct()
{
    // nodetype = designNodeTypes(nodenum, add_intersects, PAS_REQ_PROP); //{2, 2, 2, 0, 1, 2, 0} for test1.txt; {2,1,2,0,1,0} for test2.txt
    demands = (design_demands) ? designRandDemands(nodetype) : rawInstance->extract_demands();
    designModifiedDemands(demands, nodetype, shrink_pasdmd); //! default: shrink_pasdmd = false;
    coordinates = rawInstance->extract_coordinates();
    initial_distmat = calInitialDistMat(coordinates);
    modified_distmat = (modify_connectivity) ? modifyInitialDistMat(initial_distmat) : initial_distmat;
    for(int i = 0; i < disconnected_links.size(); i++)
    {
        setLinkConnectivity(disconnected_links[i][0], disconnected_links[i][1]);
    }
    initial_timemat = calInitialTravelTime(modified_distmat, nodenum, veh_speed);

    // //self-defined data for test2.txt (delete later)
    // setLinkConnectivity(0,1);
    // setLinkConnectivity(0,3);
    // setLinkConnectivity(1,4);
    // setLinkConnectivity(2,4);
    // setLinkConnectivity(2,5);
    // setLinkConnectivity(3,5);
    // setLinkConnectivity(4,5);

    // //self-defined data for test1.txt (delete later)
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

    neighbours = findAdjacentNodes(modified_distmat, nodenum);
    service_time = (design_sertime) ? designServiceTimes(nodetype) : rawInstance->extract_sertime();
    KSPBuilder altpathsets_obj(modified_distmat, nodenum, AVAIL_PATHSET_SIZE_K);
    SP_distmat = altpathsets_obj.getAllShortestPathDistance();
    SP_timemat = altpathsets_obj.getAllShortestPathTimeMat(veh_speed);
    avail_path_set = altpathsets_obj.getAllAvailablePathSet();
    travel_tw = calTravelTW(SP_timemat[0], rawInstance->getPlanHorizon(), veh_speed);
    service_tw = (design_sertw) ? designServiceTW(travel_tw, nodetype) : rawInstance->extract_sertw();
    calibrateServiceTW(service_tw, service_time, travel_tw);//get modified service time windows for each node
}

vector<int> Nodes::designNodeTypes(int node_num, bool add_intersects, double prob)
{
    vector<int> node_type(node_num);
    int pas_reqs = int(ceil(node_num * prob));
    if(prob == 0)
    {
        for(int i = 0; i < node_num; i++)
        {
            node_type[i] = 1;
        }
    }
    else if(prob == 1)
    {
        for(int i = 0; i < node_num; i++)
        {
            node_type[i] = 0;
        }
    }
    else
    {
        for(int i = 0; i < pas_reqs; i++)
        {
            node_type[i] = 0;
        }
        for(int i = pas_reqs; i < node_num; i++)
        {
            node_type[i] = 1;
        }
    }
    if(add_intersects) //if intersections are added, some nodes will be changed to intersections with type 2
    {
        RandomNumber r;
        for(int i = 0; i < node_num; i++)
        {
            node_type[i] = (r.get_rflt() <= INTERSECTIONS_PROB) ? 2 : node_type[i];
        }
    }
    return node_type;
}

vector<int> Nodes::designRandDemands(vector<int> nodetype)
{
    vector<int> demand;
    demand.resize(int(nodetype.size()));
    RandomNumber r;
    for(int i = 0; i < nodetype.size(); i++)
    {
        switch(nodetype[i])
        {
            case 0:
                demand[i] = r.get_rint(DMR_0[0], DMR_0[1]);
                break;
            case 1:
                demand[i] = r.get_rint(DMR_1[0], DMR_1[1]);
                break;
            case 2:
                demand[i] = 0;
                break;
        }
    }
    return demand;
}

void Nodes::designModifiedDemands(vector<int> &initial_dmd, vector<int> nodetype, bool modify_pasdmd)
{
    int nodenum = nodetype.size();
    for(int i = 0; i < nodenum; i++)
    {
        if(nodetype[i] == 2)
        {
            initial_dmd[i] = 0;
        }
    }
    if(modify_pasdmd)
    {
        for(int i = 0; i < nodenum; i++)
        {
            if(nodetype[i] == 0)
            {
                initial_dmd[i] = int(ceil(initial_dmd[i]*1.0 / 10));
            }
        }
    }
}

vector<int> Nodes::designServiceTimes(vector<int> node_type)
{
    vector<int> serve_time(int(node_type.size()));
    for(int i = 0; i < node_type.size(); i++)
    {
        switch(node_type[i])
        {
            case 0:
                serve_time[i] = SERT_0;
                break;
            case 1:
                serve_time[i] = SERT_1;
                break;
            case 2:
                serve_time[i] = 0;
                break;
        }
    }
    return serve_time;
}

vector<double> Nodes::designServiceRates(vector<int> node_type)
{
    vector<double> serve_rate(int(node_type.size()));
    for(int i = 0; i < node_type.size(); i++)
    {
        switch(node_type[i])
        {
            case 0:
                serve_rate[i] = SER_RATE_0;
                break;
            case 1:
                serve_rate[i] = SER_RATE_1;
                break;
            case 2:
                serve_rate[i] = 0;
                break;
        }
    }
    return serve_rate;
}

vector<vector<int>> Nodes::calTravelTW(vector<int> source_timemat, int plan_horizon, double speed)
{
    vector<vector<int>> tvl_tw(int(source_timemat.size()));
    for(int i = 0; i < source_timemat.size(); i++)   //source dist is the shortest path distance from the depot
    {
        if(plan_horizon - source_timemat[i] <= 0 + source_timemat[i])
        {
            cerr << "Infeasible travel time window of node "<< i << endl;
            tvl_tw[i] = {0, 0};
        }
        else
            tvl_tw[i] = {0 + source_timemat[i], plan_horizon - source_timemat[i]};  //by default, the starting time of the planning horizon is 0
    }
	return tvl_tw;
}

void Nodes::calibrateServiceTW(vector<vector<int>>& initial_sertw, vector<int> servetime, vector<vector<int>> tvl_tw)
{
    // assert(initial_sertw.size() == tvl_tw.size());
    for(int i = 0; i < initial_sertw.size(); i++)
    {
        initial_sertw[i][0] = max(initial_sertw[i][0], tvl_tw[i][0]);
        initial_sertw[i][1] = min(initial_sertw[i][1], tvl_tw[i][1] - servetime[i]);
    }
}


vector<vector<int>> Nodes::designServiceTW(vector<vector<int>> tvl_tw, vector<int> nodetype)
{
    // assert(tvl_tw.size() == nodetype.size());
    RandomNumber r;
    vector<vector<int>> ser_tw(int(tvl_tw.size()));
    for(int i = 0; i < tvl_tw.size(); i++)
    {
        if(nodetype[i] != 2)
        {
            int serve_period = (nodetype[i] == 0) ? SER_PRD_0 : SER_PRD_1;
            if(tvl_tw[i][1] - serve_period <= tvl_tw[i][0])
            {
                ser_tw[i] = tvl_tw[i];
            }
            else
            {
                int ser_start = r.get_rint(tvl_tw[i][0], tvl_tw[i][1] - serve_period);
                int ser_end = ser_start + serve_period;
                ser_tw[i] = {max(ser_start, tvl_tw[i][0]), min(ser_end, tvl_tw[i][1])};
            }
        }
        else
        {
            ser_tw[i] = tvl_tw[i]; //for depot and intersections
        }
    }
    return ser_tw;
}

vector<vector<int>> Nodes::findAdjacentNodes(vector<vector<double>> init_dist, int node_num)
{
    vector<vector<int>> adjacent_nodes(node_num);
    for(int i = 0; i < node_num; i++)
    {
        for(int j = 0; j < node_num; j++)
        {
            if((i != j) && (init_dist[i][j] != INF))
            {
                adjacent_nodes[i].push_back(j);
            }
        }
    }
    return adjacent_nodes;
}

vector<vector<double>> Nodes::calInitialDistMat(vector<vector<double>> coordinates)
{
    int nodenum = coordinates.size();
    vector<vector<double>> init_dist(nodenum);
    for(int i = 0; i < nodenum; i++)
    {
        init_dist[i].resize(nodenum);
        for(int j = i+1; j < nodenum && j > i; j++)
        {
            init_dist[i][j] = calEuclidDist(coordinates[i], coordinates[j]);  //round(calEuclidDist(coordinates[i], coordinates[j]))
        }
        init_dist[i][i] = 0;
    }
    for(int i = 0; i < nodenum; i++)
    {
        for(int j = 0; j < i; j++)
        {
            init_dist[i][j] = init_dist[j][i];
        }
    }
    return init_dist;
}

vector<vector<double>> Nodes::modifyInitialDistMat(vector<vector<double>> init_dist)
{
    int nodenum = init_dist.size();
    vector<vector<double>> copy_dist = init_dist;
    RandomNumber r;
    for(int i = 0; i < nodenum-1; i++)
    {
        for(int j = i+1; j < nodenum; j++)
        {
            copy_dist[i][j] = (r.get_rflt() <= DISCONNECTION_PROB) ? INF : init_dist[i][j];
            copy_dist[j][i] = copy_dist[i][j];
        }
    }
    return copy_dist;
}


vector<vector<int>> Nodes::calInitialTravelTime(vector<vector<double>> init_dist, int node_num, double speed)
{
    vector<vector<int>> travel_time(node_num);
    for(int i = 0; i < node_num; i++)
    {
        travel_time[i].resize(node_num);
        for(int j = 0; j < node_num; j++)
        {
            travel_time[i][j] = (init_dist[i][j] < INF) ? int(round(init_dist[i][j] / speed)) : INF; //rounding to integer
        }
    }
    return travel_time;
}

ADijkstraSol* Nodes::getOnePath(int nodeid1, int nodeid2, int pathid)
{
    // ADijkstraSol* initDijkstraSol = new ADijkstraSol;
    // if(pathid < avail_path_set[nodeid1][nodeid2].size())
    // {
    //     initDijkstraSol = avail_path_set[nodeid1][nodeid2][pathid];
    // }
    // return initDijkstraSol;
    assert(pathid < avail_path_set[nodeid1][nodeid2].size());
    return avail_path_set[nodeid1][nodeid2][pathid];
}


vector<vector<int>> Nodes::getAllAvailPathSize()
{
    vector<vector<int>> all_path_size(avail_path_set.size());
    for(int i = 0; i < avail_path_set.size(); i++)
    {
        all_path_size[i].resize(avail_path_set[i].size());
        for(int j = 0; j < avail_path_set[i].size(); j++)
        {
            all_path_size[i][j] = avail_path_set[i][j].size();
        }
    }
    return all_path_size;
}

// void Nodes::calMaxMinSPDist()
// {
//     vector<double> spDistVec;
//     for(int i = 0; i < nodenum-1; i++)
//     {
//         for(int j = i+1; j < nodenum; j++)
//         {
//             spDistVec.push_back(SP_distmat[i][j]);
//         }
//     }
//     auto maxit = max_element(spDistVec.begin(), spDistVec.end());
//     auto minit = min_element(spDistVec.begin(), spDistVec.end());
//     maxSPdist = *maxit;
//     minSPdist = *minit;
// }

// void Nodes::calMaxMinDmd()
// {
//     auto maxit = max_element(demands.begin(), demands.end());
//     auto minit = min_element(demands.begin(), demands.end());
//     maxDmd = *maxit;
//     minDmd = *minit;
// }

void Nodes::setLinkConnectivity(int start_node, int end_node)
{
    modified_distmat[start_node][end_node] = INF;
    modified_distmat[end_node][start_node] = INF;
}