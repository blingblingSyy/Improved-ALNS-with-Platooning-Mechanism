// NodesManager.h

#ifndef _NODESMANAGER_H_    //include guards
#define _NODESMANAGER_H_

#include <vector>
#include "couplingVRP/model/config.h"
using namespace std;

/// @brief a class to process the data for nodes
class NodesManager
{
    public:
        NodesManager() {};
        ~NodesManager() {}; 

        //! set the types of all nodes: 0 for passenger and 1 for freight
        vector<int> set_nodetype(int node_num, bool add_intersects, double prob = PAS_REQ_PROP);
        
        //! randomly generate demands based on the node types for all nodes
        vector<int> randdemand(vector<int> nodetype);
        
        //! modify the demands for intersections and passengers
        void modify_demands(vector<int> &initial_dmd, vector<int> nodetype, bool shrink_pasdmd);
        
        //! set different constant service time for different types of nodes
        vector<int> set_servetime(vector<int> node_type);
        
        //! set different service rate for different types of nodes
        vector<double> set_serverate(vector<int> node_type);
        
        //! set the travelable time windows for all nodes
        vector<vector<int>> cal_tvltw(vector<double> source_dist, int plan_horizon, double speed);
        
        //! calibrate the service time windows for all nodes based on the travel time windows
        vector<vector<int>> calibrate_sertw(vector<vector<int>> initial_sertw, vector<int> servetime, vector<vector<int>> tvl_tw);
        
        //! set the service time windows for all nodes from top given their different node types
        vector<vector<int>> set_sertw(vector<vector<int>> tvl_tw, vector<int> nodetype);

        //! get the set of adjacent nodes for all nodes given the initial distance matrix
        vector<vector<int>> find_adjacent_nodes(vector<vector<double>> init_dist, int node_num);
        
        //! calculate the initial distance matrix for all pairs of nodes
        vector<vector<double>> cal_init_distmat(vector<vector<double>> coordinates);
        
        //! modify the initial distance matrix by changing connectivity between nodes
        vector<vector<double>> modify_init_distmat(vector<vector<double>> init_dist);
        
        //! get the initial travel time matrix based on the (modified) initial distance matrix
        vector<vector<int>> get_init_tvltime(vector<vector<double>> init_dist, int node_num, double speed);

};


#endif