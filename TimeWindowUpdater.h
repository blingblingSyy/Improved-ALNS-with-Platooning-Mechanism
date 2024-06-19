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
        void set_waiting_lim();
        void set_sertw_route();
        void set_st_route();
        void cal_threshold_nodenum(); //calculate the threshold node number to use the total waiting time per node instead of the waiting time for the whole route
        int cal_nectime(int start_pos, int end_pos, bool backward); //calculate the necessary time between two given nodes in a route
        int cal_maxtime(int start_pos, int end_pos, bool backward); //calculate the maximum allowable time between two given nodes in a route
        int cal_AT1(int node_pos, int lastnode_AT1); //calculate AT1 for a node in a route
        int cal_DT1(int node_pos, int thisnode_AT1, int nextnode_AT1); //calculate DT1 for a node in a route
        int cal_DT2(int node_pos, int nextnode_DT2); //calculate DT2 for a node in a route
        int cal_AT2(int node_pos, int thisnode_DT2, int lastnode_DT2); //calculate AT2 for a node in a route
        int calib_wait_max(int used_waiting, int uncal_nodes); //calibrate the waiting time for a given number of nodes provided that some waiting time has been used in other nodes
    public:
        TimeWindowUpdater(Route &input_route, Nodes &nodes);
        void cal_route_tw();
        void set_route_tw(vector<vector<int>> &route_deptw, vector<vector<int>> &route_arrtw); //set the time windows for the route
        vector<vector<int>> get_route_arrtw();  //a simple getter
        vector<vector<int>> get_route_deptw();  //a simple getter
        vector<int> get_node_arrtw(int node_pos);  //a simple getter
        vector<int> get_node_deptw(int node_pos);  //a simple getter
        bool check_node_feasibility(int node_pos); //a simple getter
        bool check_route_feasibility();     //a simple getter
        vector<int> get_infeasible_nodepos(); //a simple getter
        int cal_waiting_pernode(int nodepos, int arrtime_pos, int deptime_pos); //calculate the actual waiting time spent at a given node in a route
        void modify_route_tw(int arc_start_pos, int arc_end_pos, vector<int> arc_overlap_dep_tw); //modify the arrival and departure time windows for a route given a modified departure time window for a node //..., vector<vector<int>> &route_deptw, vector<vector<int>> &route_arrtw
        // void modify_route_tw_AT1_DT1(int arc_start_pos, int arc_end_pos, vector<int> arc_overlap_dep_tw); //modify the arrival and departure time windows for a route given a modified departure time window for a node //..., vector<vector<int>> &route_deptw, vector<vector<int>> &route_arrtw
        // void modify_route_tw_DT2_AT2(int arc_start_pos, int arc_end_pos, vector<int> arc_overlap_dep_tw); //modify the arrival and departure time windows for a route given a modified departure time window for a node //..., vector<vector<int>> &route_deptw, vector<vector<int>> &route_arrtw
        void clear_route_tw(); //clear the arrival and departure time windows for the route
};

#endif