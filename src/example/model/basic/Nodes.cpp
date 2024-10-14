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

// #define NDEBUG

Nodes::Nodes(RawInstance& inputInstance)
{
    this->rawInstance = &inputInstance;
    nodenum = rawInstance->getAllNodesNum();
    nodetype = rawInstance->getNodeTypes();
    demands = rawInstance->getDemands();
    coordinates = rawInstance->getCoordinates();
    initial_distmat = calInitialDistMat(coordinates);
    modified_distmat = modifyInitialDistMat(initial_distmat);
    initial_timemat = calInitialTravelTime(modified_distmat, nodenum, rawInstance->getVehSpeed());
    neighbours = findAdjacentNodes(modified_distmat, nodenum);
    service_time = rawInstance->getSerTime();
    KSPBuilder altpathsets_obj(modified_distmat, nodenum, AVAIL_PATHSET_SIZE_K);
    SP_distmat = altpathsets_obj.getAllShortestPathDistance();
    SP_timemat = altpathsets_obj.getAllShortestPathTimeMat(rawInstance->getVehSpeed());
    avail_path_set = altpathsets_obj.getAllAvailablePathSet();
    travel_tw = calTravelTW(SP_timemat[0], rawInstance->getPlanHorizon(), rawInstance->getVehSpeed());
    service_tw = rawInstance->getSerTW();
    calibrateServiceTW(service_tw, service_time, travel_tw);//get modified service time windows for each node
}

Nodes::Nodes(RawInstance& inputInstance, vector<vector<int>> disconnected_links)
{
    this->rawInstance = &inputInstance;
    nodenum = rawInstance->getAllNodesNum();
    nodetype = rawInstance->getNodeTypes();
    demands = rawInstance->getDemands();
    coordinates = rawInstance->getCoordinates();
    initial_distmat = calInitialDistMat(coordinates);
    modified_distmat = initial_distmat;
    for(int i = 0; i < disconnected_links.size(); i++)
    {
        disconnectLinks(disconnected_links[i][0], disconnected_links[i][1]); //modified_distmat
    }
    initial_timemat = calInitialTravelTime(modified_distmat, nodenum, rawInstance->getVehSpeed());
    neighbours = findAdjacentNodes(modified_distmat, nodenum);
    service_time = rawInstance->getSerTime();
    KSPBuilder altpathsets_obj(modified_distmat, nodenum, AVAIL_PATHSET_SIZE_K);
    SP_distmat = altpathsets_obj.getAllShortestPathDistance();
    SP_timemat = altpathsets_obj.getAllShortestPathTimeMat(rawInstance->getVehSpeed());
    avail_path_set = altpathsets_obj.getAllAvailablePathSet();
    travel_tw = calTravelTW(SP_timemat[0], rawInstance->getPlanHorizon(), rawInstance->getVehSpeed());
    service_tw = rawInstance->getSerTW();
    calibrateServiceTW(service_tw, service_time, travel_tw);//get modified service time windows for each node
}

Nodes::~Nodes()
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

vector<vector<int>> Nodes::calTravelTW(vector<int> source_timemat, vector<int> plan_horizon, double speed)
{
    vector<vector<int>> tvl_tw(int(source_timemat.size()));
    for(int i = 0; i < source_timemat.size(); i++)   //source dist is the shortest path distance from the depot
    {
        if(plan_horizon[1] - source_timemat[i] <= plan_horizon[0] + source_timemat[i])
        {
            cerr << "Infeasible travel time window of node "<< i << endl;
            tvl_tw[i] = {0, 0};
        }
        else
            tvl_tw[i] = {plan_horizon[0] + source_timemat[i], plan_horizon[1] - source_timemat[i]};  //by default, the starting time of the planning horizon is 0
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
            init_dist[i][j] = calEuclidDist(coordinates[i], coordinates[j]);  // round(calEuclidDist(coordinates[i], coordinates[j]))
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

void Nodes::disconnectLinks(int node1, int node2)
{
    modified_distmat[node1][node2] = INF;
    modified_distmat[node2][node1] = INF;
}

// //self-defined data for test2.txt (delete later)
// disconnectLinks(0,1);
// disconnectLinks(0,3);
// disconnectLinks(1,4);
// disconnectLinks(2,4);
// disconnectLinks(2,5);
// disconnectLinks(3,5);
// disconnectLinks(4,5);

// //self-defined data for test1.txt (delete later)
// disconnectLinks(0,2) = INF;
// disconnectLinks(0,3) = INF;
// disconnectLinks(0,6) = INF;
// disconnectLinks(1,3) = INF;
// disconnectLinks(1,4) = INF;
// disconnectLinks(1,5) = INF;
// disconnectLinks(1,6) = INF;
// disconnectLinks(2,5) = INF;
// disconnectLinks(2,6) = INF;
// disconnectLinks(3,4) = INF;
// disconnectLinks(3,5) = INF;
// disconnectLinks(3,6) = INF;
// disconnectLinks(4,5) = INF;
// disconnectLinks(4,6) = INF;


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