// Config.h

#ifndef _CONFIG_H_    //include guards
#define _CONFIG_H_

#include <string> 
#include <vector>
using namespace std;

const string ins100_dir = "C:\\Users\\SYY\\Improved_ALNS_Git\\Li_Lim_Benchmark\\pdp_100";
const string result_dir = "C:\\Users\\SYY\\Improved_ALNS_Git\\results";

#define INF 0x3f3f3f3f     //infinite large number
#define SMALL 1e-4         //a small number

/*network information*/
const int PLEN_MAX = 5;  //the maximum length of platoons
const int SPEED = 30;      //MAV speed (km/h)
const int MAX_DIST = 200000;   //MAV maximum range (km)
const int VCAP_0 = 10;       //MAV capacity for passenger
const int VCAP_1 = 90;      //MAV capacity for freight
const int WAIT_0 = 2;       //waiting time limit for passenger-type AMV at each location
const int WAIT_1 = 5;       //waiting time limit for freight-type AMV at each location
const int WAIT_MAX = 10;    //waiting time limit for passenger-type AMV at all locations
const int SERT_0 = 3;       //constant service time for passenger node (for self-designed data)
const int SERT_1 = 5;       //constant service time for freight node (for self-designed data)
const double SER_RATE_0 = 1;      //service rate for passenger node (for self-designed data)
const double SER_RATE_1 = 1;      //service rate for freight node (for self-designed data)
const int SER_PRD_0 = 10;   //service period for passenger node (for self-designed data)
const int SER_PRD_1 = 15;   //service period for freight node (for self-designed data)
const vector<int> DMR_0 = {1,12};   //demand distribution range for passenger node (for self-designed data)
const vector<int> DMR_1 = {1,60};   //demand distribution range for freight node (for self-designed data)
const double PAS_MAV_PROP = 0.5;    //ratio of passenger AMVs (same as PAS_REQ_PROB)
const double PAS_REQ_PROP = 0.5;    //ratio of passenger requests
const double INTERSECTIONS_PROB = 0.3; //probability of changing a node to be an intersection
const double DISCONNECTION_PROB = 0.3; //probability of disconnecting the direct link between a given pair of nodes
const int VTYPE = 2;


//store one Dijkstra Solution
struct DijkstraOneSol
{
    double SP_Dist;
    vector<int> SP_Path;  //from start node to every other nodes
};

// //store Dijkstra Solution from a source node to all other nodes
// struct DijkstraSolVecs
// {
//     vector<double> SP_Dist_FromStart;
//     vector<vector<int>> SP_Paths_FromStart;  //from start node to every other nodes
// };

//store nodes' information
struct Nodes    
{
    int nodenum;
    vector<int> nodetype;   //0 - passenger; 1 - freight
    vector<int> demands;    //demands for each node, negative demands represent delivery requests
    vector<int> demand_type;    //0 - pickup; 1 - delivery
    // vector<vector<int>> demand_pair; //no noed to distinguish pickup demands and delivery demands of the same node
    vector<vector<int>> neighbours; //ajacent nodes of a given node
    vector<vector<double>> initial_distmat; //initial distance matrix between any two nodes
    vector<vector<int>> initial_timemat; //initial travel time matrix between any two nodes
    vector<vector<double>> SP_distmat; //shortest path distance matrix between any two nodes
    vector<vector<int>> SP_timemat;  //shortest path travel time matrix between any two nodes
    vector<vector<vector<DijkstraOneSol>>> avail_path_set; //available path set consists of all possible paths including shortest path and detours between any two nodes
    vector<vector<int>> service_tw;  //possible time window to serve a node; for depot, the time window is the planning horizon
    vector<vector<int>> travel_tw;  //possible time window to bypass a node
    vector<int> service_time; //service time is constant if 1) a node is only served by one vehicle; or 2) the service time is independent on the service amount
    vector<double> service_rate; //service rate is different between passenger-type request and freight-type request
    vector<vector<int>> match_mav; //the matching MAVs (of the same type as the served request) of a given request
};

//store AMVs' information
struct Vehicles     
{
    int veh_num;
    double veh_speed;
    int max_range;
    int max_plen;   //maximum platoon length
    vector<int> veh_type;
    vector<int> veh_cap;
    vector<int> wait_lim;
};

struct CouplingSol
{
    vector<int> coupling_path_seg; //coupling path segments
    vector<int> coupling_arrtime; //arrival time at each coupling node
    vector<int> coupling_vehs_id; //indices for coupling vehicles
};

struct Route 
{
    int veh_id; //the index of vehicle
    int veh_type; //type of the vehicle
    vector<int> compact_route; //the sequence of serving requests in this route
    vector<int> extended_route; //the sequence of serving and bypassing nodes in this route
    vector<int> node_labels; //label: serve or bypass
    vector<vector<int>> route_arrtw; //the arrival time window for each node in this route
    vector<vector<int>> route_deptw; //the departure time window for each node in this route
    vector<int> route_arrtime; //the arrival time at each node in this route
    vector<int> route_load;  //the change of vehicle load in this route
    vector<int> route_mileage;  //the change of travelling distance for this route
};

struct Solution 
{
    // vector<vector<int>> sol_routes;
    // vector<vector<vector<int>>> sol_arrival_tw;
    // vector<vector<vector<int>>> sol_departure_tw;
    // vector<vector<int>> sol_arrtime;
    // vector<vector<int>> sol_loads;
    // vector<vector<double>> sol_miles; 
    vector<Route> sol_config;
    vector<CouplingSol> sol_platoons;
    double total_obj_val;
    double cpu_time;
};

#endif