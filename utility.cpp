#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <cstring>
#include <algorithm>
#include <math.h>
#include "config.h"
#include "utility.h"

using namespace std;


//generate random number with RandomNumber class
RandomNumber::RandomNumber()
{
    srand((unsigned)time(NULL));
}

int RandomNumber::get_rint(int begin, int end)
{
    return rand() % (end - begin + 1) + begin;
}

float RandomNumber::get_rflt(int begin, int end)
{
    return begin + float(rand()) / float(RAND_MAX) * (end - begin);
}


//generate type for each MAV - 0 for passenger and 1 for freight
vector<int> set_mavtype(int mav_num, double prob)   //prob = PAS_MAV_PROP
{
    vector<int> mav_type(mav_num);
    int pas_mavs = int(ceil(mav_num * prob));
    if(prob == 0)
    {
        for(int i = 0; i < mav_num; i++)
        {
            mav_type[i] = 1;
        }
    }
    else if(prob == 1)
    {
        for(int i = 0; i < mav_num; i++)
        {
            mav_type[i] = 0;
        }
    }
    else
    {
        for(int i = 0; i < pas_mavs; i++)
        {
            mav_type[i] = 0;
        }
        for(int i = pas_mavs; i < mav_num; i++)
        {
            mav_type[i] = 1;
        }
    }
    return mav_type;
    // vector<int> amv_type(amv_num);
    // RandomNumber r;
    // for(int i = 0; i < amv_num; i++)
    // {
    //     amv_type[i] = (r.get_rflt() <= prob) ? 0 : 1;
    // }
}

//link vehicle capacity with each vehicle type
vector<int> set_mavcap(vector<int> mav_type)    //0 for passenger and 1 for freight
{
    vector<int> mav_cap(int(mav_type.size()));
    for(int i = 0; i < mav_type.size(); i++)
    {
        mav_cap[i] = (mav_type[i] == 0) ? VCAP_0: VCAP_1;
    }
    return mav_cap;
}

//generate waiting time limit for each AMV
vector<int> set_mavwlim(vector<int> mav_type)
{
    vector<int> mav_wlim(int(mav_type.size()));
    for(int i = 0; i < mav_type.size(); i++)
    {
        mav_wlim[i] = (mav_type[i] == 0) ? WAIT_0: WAIT_1;
    }
    return mav_wlim;
}

//calculate the distance reduction factor for a platoon of specific length
double pl_factor(int length)
{
    if(length <= 0)
    {
        throw "Division by zero or negative condition!";
    }
    return double(1+0.90*(length-1)) / double(length);
}

//generate type for each request - 0 for passenger and 1 for freight
//in larger transportation network, some nodes have type 2, which are intersections without demands. 
vector<int> set_nodetype(int node_num, double prob, bool add_intersects) //prob = PAS_REQ_PROP
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
vector<int> randdemand(vector<int> nodetype)
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
void modify_demand(vector<int> &initial_dmd, vector<int> nodetype)
{
    int nodenum = nodetype.size();
    for(int i = 0; i < nodenum; i++)
    {
        if(nodetype[i] == 2)
        {
            initial_dmd[i] = 0;
        }
    }
}

//set the demand type for each node based on the node types and the demands
vector<int> set_demand_type(vector<int> demands, vector<int> nodetype)
{
    vector<int> dmd_type; //0: pickup nodes; 1: delivery nodes; 2: depot or intersections
    int nodenum = nodetype.size();
    for(int i = 0; i < nodenum; i++)
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
vector<int> set_sertime_const(vector<int> node_type)
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
vector<double> set_serverate(vector<int> node_type)
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
vector<vector<int>> set_sertw(vector<vector<int>> tvl_tw, vector<int> nodetype) //the two parameters should be of the same length
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
vector<vector<int>> set_matchmavs(vector<int> nodetype, vector<int> mavtype)
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

//calculate Euclidean distance between any two nodes
double cal_euclid_dist(vector<double> x1_coord, vector<double> x2_coord)
{
    return sqrt((x1_coord[0] - x2_coord[0])*(x1_coord[0] - x2_coord[0]) + (x1_coord[1] - x2_coord[1])*(x1_coord[1] - x2_coord[1]));
}

//get initial distance matrix given the coordinate of each node
vector<vector<double>> get_init_distmat(vector<vector<double>> coordinates)
{
    int nodenum = coordinates.size();
    vector<vector<double>> init_dist(nodenum);
    for(int i = 0; i < nodenum; i++)
    {
        init_dist[i].resize(nodenum);
        for(int j = 0; j < nodenum && j != i; j++)
        {
            init_dist[i][j] = cal_euclid_dist(coordinates[i], coordinates[j]);
        }
        init_dist[i][i] = 0;
    }
    
}

//modify the initial distance matrix by randomly disconnecting some direct links between two nodes
vector<vector<double>> modify_init_distmat(vector<vector<double>> init_dist)
{
    int nodenum = init_dist.size();
    vector<vector<double>> copy_dist = init_dist;
    RandomNumber r;
    for(int i = 0; i < nodenum; i++)
    {
        for(int j = 0; j < nodenum && j != i; j++)
        {
            copy_dist[i][j] = (r.get_rflt() <= DISCONNECTION_PROB) ? INF : init_dist[i][j];
        }
    }
}

//find the neighbours of each node
vector<vector<int>> get_neighbours(vector<vector<double>> init_dist, int node_num)
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
vector<vector<int>> get_init_tvltime(vector<vector<double>> init_dist, int node_num, double speed)
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
vector<vector<int>> get_tvltw(vector<double> source_dist, int plan_horizon, double speed)   //source dist is the shortest path distance from the depot
{
    vector<vector<int>> tvl_tw(int(source_dist.size()));
    for(int i = 0; i < source_dist.size(); i++)
    {
        int source_time = int(ceil(source_dist[i] / speed));
        if(plan_horizon - source_time <= 0 + source_time)
        {
            cerr << "Infeasible travel time window of node "<< i << endl;
        }
        else
            tvl_tw[i] = {0 + source_time, plan_horizon - source_time};  //by default, the starting time of the planning horizon is 0
    }
	return tvl_tw;
}

//calculate the distance between two nodes in an extended route
double cal_path_dist(vector<int> route, vector<vector<double>> init_dist, int start_id, int end_id)
{
    double dist_path = 0.0;
    for(int i = start_id; i < end_id; i++)
    {
        int arc_start = route[i];
        int arc_end = route[i+1];
        dist_path += init_dist[arc_start][arc_end];
    }
    return dist_path;
}


//calculate the travel time between two nodes in an extended route
int cal_path_tvltime(vector<int> route, vector<vector<int>> init_timemat, int start_id, int end_id)
{
    int tvltime_path = 0.0;
    for(int i = start_id; i < end_id; i++)
    {
        int arc_start = route[i];
        int arc_end = route[i+1];
        tvltime_path += init_timemat[arc_start][arc_end];
    }
    return tvltime_path;
}

//find the indices for the served nodes
vector<int> find_servednode_id(vector<int> labels)
{
    vector<int> serve_idvec;
    auto routelen = labels.size();
    for(int i = 0; i < routelen; i++)
    {
        if(labels[i]) //label == 1: serve
        {
            serve_idvec.push_back(i);
        }
    }
    return serve_idvec;
}

// //remove the intersection part of two vectors
// void remove_intersection(vector<int>& a, vector<int>& b){
//     unordered_multiset<int> st;
//     st.insert(a.begin(), a.end());
//     st.insert(b.begin(), b.end());
//     auto predicate = [&st](const int& k){return st.count(k) > 1; };
//     a.erase(remove_if(a.begin(), a.end(), predicate), a.end());
//     b.erase(remove_if(b.begin(), b.end(), predicate), b.end());
// }

// //find the common path segments of two routes
// vector<vector<vector<int>>> intersect_arcs(vector<vector<vector<int>>> sort_arcset1, vector<vector<vector<int>>> sort_arcset2)
// {
//     vector<vector<vector<int>>> common_set;
//     set_intersection(sort_arcset1.begin(), sort_arcset1.end(), sort_arcset2.begin(), sort_arcset2.end(), back_inserter(common_set));
//     return common_set;
// }


// //find the position of elements of vector 2 in vector 1
// vector<int> intersect_pos(vector<vector<vector<int>>> sort_arcset, vector<vector<vector<int>>> sort_arcsubset)
// {
//     vector<int> pos_in_arc1;
//     vector<vector<vector<int>>>::iterator first1 = sort_arcset.begin();
//     vector<vector<vector<int>>>::iterator last1 = sort_arcset.end();
//     vector<vector<vector<int>>>::iterator first2 = sort_arcsubset.begin();
//     vector<vector<vector<int>>>::iterator last2 = sort_arcsubset.end();
//     while(first1 != last1 && first2 != last2)
//     {
//         if(*first1 < *first2)
//             ++first1;
//         else
//         {
//             if(!(*first2 < *first1))
//             {
//                 pos_in_arc1.push_back(static_cast<int>(first1 - sort_arcset.begin()));
//                 ++first1;
//             }
//             ++first2;
//         }
//     }
//     return pos_in_arc1;
// }