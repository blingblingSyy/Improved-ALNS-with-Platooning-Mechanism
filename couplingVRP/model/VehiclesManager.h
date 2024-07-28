// NodesManager.h

#ifndef _NODESMANAGER_H_    //include guards
#define _NODESMANAGER_H_

#include <vector>
#include "config.h"
using namespace std;

/// @brief a class to process the data for nodes
class VehiclesManager
{
    protected:
    //setting mavs
    vector<int> set_mavtype(int mav_num, double prob);
    vector<int> set_mavcap(vector<int> mav_type);
    vector<int> set_mavwlim(vector<int> mav_type);

    //setting nodes
    vector<int> set_nodetype(int node_num, double prob, bool add_intersects);
    vector<int> randdemand(vector<int> nodetype);
    void modify_demand(vector<int> &initial_dmd, vector<int> nodetype, bool modify_pasdmd);
    vector<int> set_demand_type(vector<int> demands, vector<int> nodetype);
    vector<int> set_servetime(vector<int> node_type);
    vector<double> set_serverate(vector<int> node_type);
    vector<vector<int>> set_sertw(vector<vector<int>> tvl_tw, vector<int> nodetype);
    vector<vector<int>> set_matchmavs(vector<int> nodetype, vector<int> mavtype);
    vector<vector<int>> cal_tvltw(vector<double> source_dist, int plan_horizon, double speed);
    vector<vector<int>> get_neighbours(vector<vector<double>> init_dist, int node_num);
    
    //setting matrices
    vector<vector<double>> get_init_distmat(vector<vector<double>> coordinates);
    vector<vector<double>> modify_init_distmat(vector<vector<double>> init_dist);
    vector<vector<int>> get_init_tvltime(vector<vector<double>> init_dist, int node_num, double speed);
    
    public:
        VehiclesManager();

};


#endif