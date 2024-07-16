// InitFeasSol.h

#ifndef _INSERTION_HEURISTIC_H_    //include guards
#define _INSERTION_HEURISTIC_H_

#include <vector>
#include <string>
#include "HeuristicFramework.h"
using namespace std;


class InitFeasSol: public HeuristicFramework
{
    private: 
        vector<int> cus_id_set;
        vector<int> veh_id_set;
        //vector<OneCus> new_compact_route;

        // vector<int> build_new_compact_route(vector<int> &remain_cus_id, vector<int> &remain_vehs_id, int &route_veh_id, int &route_veh_type); //build a new route with randomly selected node from the set of uninserted customers
        Route build_new_compact_route(vector<int> &remain_cus_id, vector<int> &remain_vehs_id); //build a new route with randomly selected node from the set of uninserted customers
        void build_initial_sol(vector<int> &unserved_cus_id, vector<int> &unused_vehs_id); //create an initial solution for all customers

    public:
        InitFeasSol(Nodes &nodes, Vehicles &vehs);
        void run_algorithm(); //run the heuristic and record the cpu time
};

#endif

// struct Route 
// {
//     int veh_id; //the index of vehicle
//     int veh_type; //type of the vehicle
//     vector<int> compact_route; //the sequence of serving requests in this route
//     vector<int> used_paths_in_compact_route; //the path index used between each adjacent nodes
//     vector<int> extended_route; //the sequence of serving and bypassing nodes in this route
//     vector<int> node_labels; //label == 1: served; label == 0: bypass
//     vector<vector<int>> route_arrtw; //the arrival time window for each node in the extended route
//     vector<vector<int>> route_deptw; //the departure time window for each node in the extended route
//     vector<int> route_arrtime; //the arrival time at each node in this route
//     vector<int> route_deptime; //the departure time at each node in this route
//     vector<int> route_load;  //the change of vehicle load in this route
//     vector<int> route_mileage;  //the change of travelling distance for this route
// };