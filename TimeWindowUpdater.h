// TimeWidowUpdater.h

#ifndef _TIMEWINDOWUPDATER_H_    //include guards
#define _TIMEWINDOWUPDATER_H_

#include <vector>
#include <string>
#include "config.h"
using namespace std;

//update the arrival and departure time windows within a route
class TimeWindowUpdater
{
    private: 
        Route route;
        Nodes nodeset;
        int routelen;
        vector<vector<int>> sertw_inroute;
        vector<int> st_inroute;
        int wait_pernode;
        int wait_max;
        int threshold_nodenum;
        
        /*set and calculate necessary elements for the time windows*/
        void set_waiting_lim();
        void set_sertw_route();
        void set_st_route();
        void cal_threshold_nodenum(); //calculate the threshold node number to use the total waiting time per node instead of the waiting time for the whole route
        int cal_nectime(int start_pos, int end_pos, bool not_serve_end_node); //calculate the necessary time between two given nodes in a route
        int cal_maxtime(int start_pos, int end_pos, bool not_serve_end_node); //calculate the maximum allowable time between two given nodes in a route
        
        /*calculate the original arrival and departure time windows of a route*/
        int cal_AT1(int node_pos, int lastnode_AT1); //calculate AT1 for a node in a route
        int cal_DT1(int node_pos, int thisnode_AT1, int nextnode_AT1); //calculate DT1 for a node in a route
        int cal_DT2(int node_pos, int nextnode_DT2); //calculate DT2 for a node in a route
        int cal_AT2(int node_pos, int thisnode_DT2, int lastnode_DT2); //calculate AT2 for a node in a route
        
        /*modify the time windows of a route given a modified time window of an arc*/
        int calib_wait_max(int used_waiting, int uncal_nodes); //calibrate the waiting time for a given number of nodes provided that some waiting time has been used in other nodes
        void modify_route_tw_AT1_DT1(int arc_start_pos_input, int arc_end_pos_input, vector<vector<int>> original_arrtw_input); //modify the AT1 and DT1 for a route given a modified departure time window for a node //..., vector<vector<int>> &route_deptw, vector<vector<int>> &route_arrtw
        void modify_route_tw_DT2_AT2(int arc_start_pos_input, int arc_end_pos_input, vector<vector<int>> original_deptw_input); //modify the DT2 and AT2 for a route given a modified departure time window for a node //..., vector<vector<int>> &route_deptw, vector<vector<int>> &route_arrtw
    
    public:
        TimeWindowUpdater(Route &input_route, Nodes &nodes);

        /*calculate the arrival and departure time windows for a route*/
        void cal_route_tw();
        void set_route_tw(vector<vector<int>> &route_deptw, vector<vector<int>> &route_arrtw); //set the time windows for the route
        
        /*get the arrival and departure time windows of a route and check feasibility for the route*/
        vector<vector<int>> get_route_arrtw();  //a simple getter
        vector<vector<int>> get_route_deptw();  //a simple getter
        vector<int> get_node_arrtw(int node_pos);  //a simple getter
        vector<int> get_node_deptw(int node_pos);  //a simple getter
        bool check_node_feasibility(int node_pos); //a simple getter
        bool check_route_feasibility();     //a simple getter
        vector<int> get_infeasible_nodepos(); //a simple getter
        
        /*modify the time windows based on a modified time window of a given arc*/
        void modify_route_tw(int arc_start_pos, vector<int> overlap_dep_tw); //modify the arrival and departure time windows for a route given a modified departure time window for a node //..., vector<vector<int>> &route_deptw, vector<vector<int>> &route_arrtw
        
        /*calculate the actual arrival, departure, waiting time at a node*/
        int cal_waiting_pernode(int nodepos, int arrtime_pos, int deptime_pos); //calculate the actual waiting time spent at a given node in a route
        void set_arrdep_time_one_route(vector<vector<int>> final_arrtw, vector<vector<int>> final_deptw, vector<int> &arrtime_vec, vector<int> &deptime_vec); //calculate the actual arrival and departure time for a route
        int route_total_trip_duration(int arrtime_lastnode, int deptime_firstnode);
        int route_total_wait_time(int arrtime_lastnode, int deptime_firstnode);

        /*clear all time windows of a route*/
        void clear_route_tw(); //clear the arrival and departure time windows for the route
};

#endif