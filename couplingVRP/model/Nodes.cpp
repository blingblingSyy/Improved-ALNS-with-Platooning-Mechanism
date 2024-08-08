#include <iostream>  
#include <vector>  
#include <cmath>
#include <cstdlib>  
#include <algorithm>  
#include <cassert>
#include "couplingVRP/model/Nodes.h"
#include "couplingVRP/model/RawInstance.h"
#include "couplingVRP/model/ADijkstraSol.h"
#include "couplingVRP/model/KSPBuilder.h"
#include "couplingVRP/model/utility.h"
#include "couplingVRP/model/config.h"

Nodes::Nodes(RawInstance& inputInstance, bool reset_demands = false, bool reset_sertime = false, bool reset_sertw = false, bool add_intersects, bool shrink_pasdmd, bool modify_connectivity, int veh_speed)
{
    this->rawInstance = &inputInstance;
    this->reset_demands = reset_demands;
    this->reset_sertime = reset_sertime;
    this->reset_sertw = reset_sertw;
    this->add_intersects = add_intersects;
    this->shrink_pasdmd = shrink_pasdmd;
    this->modify_connectivity = modify_connectivity;
    nodenum = rawInstance->getRowNum()-1;
    buildNodesStruct(veh_speed);
}

Nodes::~Nodes()
{
    delete rawInstance;
    for(int i = 0; i < avail_path_set.size(); i++)
    {
        for(int j = 0; j < avail_path_set[i].size(); j++)
        {
            for(int z = 0; z < avail_path_set[i][j].size(); z++)
            {
                delete avail_path_set[i][j][z];
            }
        }
    }
}

void Nodes::buildNodesStruct(int veh_speed)
{
    nodetype = set_nodetype(nodenum, add_intersects, PAS_REQ_PROP); //{2, 2, 2, 0, 1, 2, 0} for test1.txt
    demands = (reset_demands) ? randdemand(nodetype) : rawInstance->extract_demands();
    modify_demands(demands, nodetype, shrink_pasdmd);
    coordinates = rawInstance->extract_coordinates();
    initial_distmat = cal_init_distmat(coordinates);
    modified_distmat = (modify_connectivity) ? modify_init_distmat(initial_distmat) : initial_distmat;
    initial_timemat = cal_init_tvltime(modified_distmat, nodenum, veh_speed);

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

    neighbours = find_adjacent_nodes(modified_distmat, nodenum);
    service_time = (reset_sertime) ? set_servetime(nodetype) : rawInstance->extract_sertime();
    KSPBuilder altpathsets_obj(modified_distmat, nodenum, AVAIL_PATHSET_SIZE_K);
    SP_distmat = altpathsets_obj.getAllShortestPathDistance();
    avail_path_set = altpathsets_obj.getAllAvailablePathSet();
    travel_tw = cal_tvltw(SP_distmat[0], rawInstance->getPlanHorizon(), veh_speed);
    service_tw = (reset_sertw) ? set_sertw(travel_tw, nodetype) : rawInstance->extract_sertw();
    calibrate_sertw(service_tw, service_time, travel_tw);//get modified service time windows for each node
}

vector<int> Nodes::set_nodetype(int node_num, bool add_intersects, double prob)
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

vector<int> Nodes::randdemand(vector<int> nodetype)
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

void Nodes::modify_demands(vector<int> &initial_dmd, vector<int> nodetype, bool modify_pasdmd)
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

vector<int> Nodes::set_servetime(vector<int> node_type)
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

vector<double> Nodes::set_serverate(vector<int> node_type)
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

vector<vector<int>> Nodes::cal_tvltw(vector<double> source_dist, int plan_horizon, double speed)
{
    vector<vector<int>> tvl_tw(int(source_dist.size()));
    for(int i = 0; i < source_dist.size(); i++)   //source dist is the shortest path distance from the depot
    {
        int source_time = int(ceil(source_dist[i] / speed));
        if(plan_horizon - source_time <= 0 + source_time)
        {
            cerr << "Infeasible travel time window of node "<< i << endl;
            tvl_tw[i] = {0, 0};
        }
        else
            tvl_tw[i] = {0 + source_time, plan_horizon - source_time};  //by default, the starting time of the planning horizon is 0
    }
	return tvl_tw;
}

void Nodes::calibrate_sertw(vector<vector<int>>& initial_sertw, vector<int> servetime, vector<vector<int>> tvl_tw)
{
    // assert(initial_sertw.size() == tvl_tw.size());
    for(int i = 0; i < initial_sertw.size(); i++)
    {
        initial_sertw[i][0] = max(initial_sertw[i][0], tvl_tw[i][0]);
        initial_sertw[i][1] = min(initial_sertw[i][1], tvl_tw[i][1] - servetime[i]);
    }
}


vector<vector<int>> Nodes::set_sertw(vector<vector<int>> tvl_tw, vector<int> nodetype)
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

vector<vector<int>> Nodes::find_adjacent_nodes(vector<vector<double>> init_dist, int node_num)
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

vector<vector<double>> Nodes::cal_init_distmat(vector<vector<double>> coordinates)
{
    int nodenum = coordinates.size();
    vector<vector<double>> init_dist(nodenum);
    for(int i = 0; i < nodenum; i++)
    {
        init_dist[i].resize(nodenum);
        for(int j = i+1; j < nodenum && j > i; j++)
        {
            init_dist[i][j] = calEuclidDist(coordinates[i], coordinates[j]);
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

vector<vector<double>> Nodes::modify_init_distmat(vector<vector<double>> init_dist)
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


vector<vector<int>> Nodes::cal_init_tvltime(vector<vector<double>> init_dist, int node_num, double speed)
{
    vector<vector<int>> travel_time(node_num);
    for(int i = 0; i < node_num; i++)
    {
        travel_time[i].resize(node_num);
        for(int j = 0; j < node_num; j++)
        {
            travel_time[i][j] = (init_dist[i][j] < INF) ? int(ceil(init_dist[i][j] / speed)) : INF; //rounding up to integer
        }
    }
    return travel_time;
}

ADijkstraSol* Nodes::getOnePath(int nodeid1, int nodeid2, int pathid)
{
    if(pathid < avail_path_set[nodeid1][nodeid2].size())
    {
        return avail_path_set[nodeid1][nodeid2][pathid];
    }
    else
    {
        ADijkstraSol* emptyDijkstraSol;
        return emptyDijkstraSol;
    }
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


int Nodes::getMaxAvailPathSize()
{
    vector<vector<int>> all_path_size = getAllAvailPathSize();
    vector<int> onedim_maxsize;
    for(int i = 0; i < all_path_size.size(); i++)
    {
        auto it = max_element(all_path_size[i].begin(), all_path_size[i].end());
        onedim_maxsize.push_back(*it);
    }
    auto it = max_element(onedim_maxsize.begin(), onedim_maxsize.end());
    return *it;
}