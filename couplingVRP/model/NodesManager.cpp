#include <iostream>  
#include <vector>  
#include <cmath>
#include <cstdlib>  
#include <algorithm>  
#include "couplingVRP/model/NodesManager.h"
#include "utility.h"
#include "config.h"
using namespace std;  

int NodesManager::identify_nodetype(int nodeid, int nodedmd)
{

}


//in larger transportation network, some nodes have type 2, which are intersections without demands. 
vector<int> NodesManager::set_nodetype(int node_num, double prob, bool add_intersects) //prob = PAS_REQ_PROP
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

//generate random demand for each node according to their node type (for self-created data)
vector<int> NodesManager::randdemand(vector<int> nodetype)
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

//modify demands according to node types by setting demands
void NodesManager::modify_demand(vector<int> &initial_dmd, vector<int> nodetype, bool modify_pasdmd)
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

//set the demand type for each node based on the node types and the demands
vector<int> NodesManager::set_demand_type(vector<int> demands, vector<int> nodetype)
{
    vector<int> dmd_type(nodetype.size()); //0: pickup nodes; 1: delivery nodes; 2: depot or intersections
    for(int i = 0; i < nodetype.size(); i++)
    {
        if(nodetype[i] != 2)
        {
            dmd_type[i] = (demands[i] >= 0) ? 0 : 1;
        }
        else //nodetype[i] == 2
        {
            dmd_type[i] = 2;
        }
    }
    return dmd_type;
}

//generate constant service time length for each location (for self-created data)
vector<int> NodesManager::set_servetime(vector<int> node_type)
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

//generate service rate for each customer (for self-created data)
vector<double> NodesManager::set_serverate(vector<int> node_type)
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

//generate available AMV serving time window for each customer according to their node type (for self-created data)
vector<vector<int>> NodesManager::set_sertw(vector<vector<int>> tvl_tw, vector<int> nodetype) //the two parameters should be of the same length
{
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
            ser_tw[i] = tvl_tw[i];
        }
    }
    return ser_tw;
}

//get the combination of customers and amvs of the same type
vector<vector<int>> NodesManager::set_matchmavs(vector<int> nodetype, vector<int> mavtype)
{
    vector<vector<int>> match_comb;
    match_comb.resize(int(nodetype.size()));
    for(int i = 0; i < nodetype.size(); i++)
    {
        if(nodetype[i] < 2)
        {
            for(int v = 0; v < mavtype.size(); v++)
            {
                if(mavtype[v] == nodetype[i])
                {
                    match_comb[i].push_back(v);
                }
            }
        }
        else    //nodetype[i] == 2 -> intersections
        {
            for(int v = 0; v < mavtype.size(); v++)
            {
                match_comb[i].push_back(v);
            }
        }
    }
    return match_comb;
}


//get initial distance matrix given the coordinate of each node
vector<vector<double>> NodesManager::get_init_distmat(vector<vector<double>> coordinates)
{
    int nodenum = coordinates.size();
    vector<vector<double>> init_dist(nodenum);
    for(int i = 0; i < nodenum; i++)
    {
        init_dist[i].resize(nodenum);
        for(int j = i+1; j < nodenum && j > i; j++)
        {
            init_dist[i][j] = cal_euclid_dist(coordinates[i], coordinates[j]);
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

//modify the initial distance matrix by randomly disconnecting some direct links between two nodes
vector<vector<double>> NodesManager::modify_init_distmat(vector<vector<double>> init_dist)
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

//find the neighbours of each node
vector<vector<int>> NodesManager::get_neighbours(vector<vector<double>> init_dist, int node_num)
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

//generate the travel time matrix based on original distance matrix
vector<vector<int>> NodesManager::get_init_tvltime(vector<vector<double>> init_dist, int node_num, double speed)
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

//generate available AMV travelling time windows for each customer: earliest arrival time & latest leaving time
//to model time-space network, transform time into integers
vector<vector<int>> NodesManager::cal_tvltw(vector<double> source_dist, int plan_horizon, double speed)   //source dist is the shortest path distance from the depot
{
    vector<vector<int>> tvl_tw(int(source_dist.size()));
    for(int i = 0; i < source_dist.size(); i++)
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