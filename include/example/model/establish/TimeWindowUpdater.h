// TimeWidowUpdater.h

#ifndef _TIMEWINDOWUPDATER_H_    //include guards
#define _TIMEWINDOWUPDATER_H_

#include <vector>
#include <string>
using namespace std;

class Nodes;

/// @brief a class to calculate and store the arrival and departure time windows for a given route
class TimeWindowUpdater
{
    private: 
        //! a pointer to the input nodeset
        Nodes* nodeset;

        //! the initial travel time matrix between any pair of nodes
        vector<vector<int>> initial_timemat;

        //! the input extended route
        vector<int> extended_route;

        //! the input node labels
        vector<int> node_labels;

        //! the length of the extended route
        int extendroutelen;

        //! the service time window of all nodes in the extended route
        vector<vector<int>> sertw_inroute;

        //! the service time of all nodes in the extended route
        vector<int> st_inroute;

        //! the updated waiting time limit of the route for each node
        int wait_pernode;

        //! the updated maximum waiting time limit for the whole route
        int wait_max;

        //! the arrival time window of the input route
        vector<vector<int>> route_arrtw;

        //! the departure time window of the input route
        vector<vector<int>> route_deptw;

        //! the arrival time of the input route
        vector<int> route_arrtime;

        //! the departure time of the input route
        vector<int> route_deptime;

        //! initialize time
        void initTimeElements();

        //! set the service time window for each node in the extended route
        void setServiceTW();

        //! set the service time for each node in the extended route
        void setServiceTime();

        //! calculate the waiting time budget for given numbers of invovled nodes
        int calWaitTimeBudget(int nodes_involved, int used_waiting);

        //! calculate the necessary time from a start node to an end node
        int calNecessaryTime(int start_pos, int end_pos, bool serve_start_node, bool serve_end_node); 
        
        //! calculate the maximum time from a start node to an end node (necessary time + buffer time)
        int calMaximumTime(int start_pos, int end_pos, bool serve_start_node, bool serve_end_node, int used_waiting);
        
        //! calculate the expected AT1 for all nodes in a route
        int calExpectedAT1(int node_pos, int lastnode_AT1);

        //! calculate the expected DT1 for all nodes in a route
        int calExpectedDT1(int node_pos, int thisnode_AT1, int nextnode_AT1);

        //! calculate the expected DT2 for all nodes in a route
        int calExpectedDT2(int node_pos, int nextnode_DT2);

        //! calculate the expected AT2 for all nodes in a route
        int calExpectedAT2(int node_pos, int thisnode_DT2, int lastnode_DT2);

        //! calibrate the AT1 and DT1 for all nodes in a route given the specified departure time window of an arc
        void Calib_AT1_DT1(int fixed_deptw_arcpos, vector<vector<int>> original_arrtw);
        
        //! calibrate the DT2 and AT2 for all nodes in a route given the specified departure time window of an arc
        void Calib_DT2_AT2(int fixed_deptw_arcpos, vector<vector<int>> original_deptw_input); //calibrate the DT2 and AT2 for a route given a modified departure time window for a node

        //! check the time feasibility of a node position
        bool checkNodeTimeFeas(int node_pos);

    public:
        //! constructor
        TimeWindowUpdater(vector<int> extended_route, vector<int> node_labels, int WaitTimePerNode, int WaitMaxRoute, Nodes& nodes);
        
        //! a default constructor
        TimeWindowUpdater() = default;
        
        //! destructor
        ~TimeWindowUpdater();

        //! calculate the expected arrival and departure time windows of a route
        bool calRouteExpectedTW();

        //! calibrate the time windows of a route given a modfied departure time window of an arc
        void CalibRouteTW(int fixed_deptw_arcpos, vector<int> overlap_dep_tw);

        //! set the final arrival and departure time for a given route with the route's final arrival and departure time windows
        void setFinalArrDepTime();

        //! a simple setter
        void setRouteArrTW(vector<vector<int>> input_arrtw) {route_arrtw = input_arrtw;};

        //! a simple setter
        void setRouteDepTW(vector<vector<int>> input_deptw) {route_deptw = input_deptw;};

        //! a simple setter
        void setNodeArrTW(int nodepos, vector<int> node_arrtw) {route_arrtw[nodepos] = node_arrtw;};
        
        //! a simple setter
        void setNodeDepTW(int nodepos, vector<int> node_deptw) {route_deptw[nodepos] = node_deptw;};

        //! a simple setter
        void setNodeArrTime(int nodepos, int node_arrtime) {route_arrtime[nodepos] = node_arrtime;};

        //! a simple setter
        void setNodeDepTime(int nodepos, int node_deptime) {route_deptime[nodepos] = node_deptime;};

        //! a simple getter
        vector<vector<int>> getRouteArrTW() {return route_arrtw;};

        //! a simple getter
        vector<vector<int>> getRouteDepTW() {return route_deptw;};

        //! a simple getter
        vector<int> getNodeArrTW(int node_pos) {return route_arrtw[node_pos];};

        //! a simple getter
        vector<int> getNodeDepTW(int node_pos) {return route_deptw[node_pos];};

        //! a simple getter
        vector<int> getRouteArrTime() {return route_arrtime;};

        //! a simple getter
        vector<int> getRouteDepTime() {return route_deptime;};

        //! a simple getter
        int getNodeArrTime(int node_pos) {return route_arrtime[node_pos];};

        //! a simple getter
        int getNodeDepTime(int node_pos) {return route_deptime[node_pos];};

        //! check the time feasibility of a route
        bool isRouteTimeFeas(); 

        //! get the infeasible nodes' positions
        vector<int> getTimeInfeasNodePos();
        
        //! calculate the actual waiting time spent at a given node in a route
        int calActualWaitTimePerNode(int nodepos, int arrtime_pos, int deptime_pos); 

        //! calculate the actual waiting time spent at each node in an extended route
        vector<int> getActualWaitTimeAllNodes();

        //! calculate the total trip duration for the input route
        int calRouteTotalTripDuration();

        //! calculate the total waiting time for the input route
        int calTotalWaitTime();

        //! clear the arrival and departure time windows for the route
        void clear_route_tw(); 

        //! clear the arrival and departure time for the route
        void clear_route_time();
};

#endif