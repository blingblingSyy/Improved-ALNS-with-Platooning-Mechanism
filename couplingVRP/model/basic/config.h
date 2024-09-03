// Config.h //for model config

#ifndef _CONFIG_H_    //include guards
#define _CONFIG_H_

#include <string> 
#include <vector>
using namespace std;

/*file paths*/
// const string ins100_dir = "C:\\Users\\SYY\\Improved_ALNS_Git\\Li_Lim_Benchmark\\pdp_100";  // the directory for benchmark instances
// const string result_dir = "C:\\Users\\SYY\\Improved_ALNS_Git\\results";   // the directory for result files

/*special numbers*/
#define INF 0x3f3f3f3f   // define a very large number
#define SMALL 1e-4    // define a very small number

/*network information*/
const int PLEN_MAX = 5;  //the maximum length of platoons
const int SPEED = 1;      //MAV speed (km/h)
const int MAX_DIST = 100;   //MAV maximum range (km)
const int VTYPE = 2;        //the number of MAV types
const int VCAP_0 = 1;       //MAV capacity for passenger
const int VCAP_1 = 2;      //MAV capacity for freight
const int WAIT_0 = 1;       //waiting time limit for passenger-type AMV at each location
const int WAIT_1 = 2;       //waiting time limit for freight-type AMV at each location
const int WAIT_MAX_0 = 5;    //waiting time limit for passenger-type AMV at all locations
const int WAIT_MAX_1 = 7;    //waiting time limit for freight-type AMV at all locations
const int SERT_0 = 1;       //constant service time for passenger node (for self-designed data)
const int SERT_1 = 3;       //constant service time for freight node (for self-designed data)
const double SER_RATE_0 = 1;      //service rate for passenger node (for self-designed data)
const double SER_RATE_1 = 1;      //service rate for freight node (for self-designed data)
const int SER_PRD_0 = 10;   //service period for passenger node (for self-designed data)
const int SER_PRD_1 = 20;   //service period for freight node (for self-designed data)
const vector<int> DMR_0 = {1,12};   //demand distribution range for passenger node (for self-designed data)
const vector<int> DMR_1 = {1,60};   //demand distribution range for freight node (for self-designed data)
const double PAS_MAV_PROP = 0.5;    //ratio of passenger MAVs (same as PAS_REQ_PROB)
const double PAS_REQ_PROP = 0.5;    //ratio of passenger requests
const double INTERSECTIONS_PROB = 0.3; //probability of changing a node to be an intersection
const double DISCONNECTION_PROB = 0.3; //probability of disconnecting the direct link between a given pair of nodes
const int modify_pasdmd_amount = 10; //shrink the passenger demand by a factor of 10

/*model parameters*/
const int AVAIL_PATHSET_SIZE_K = 5;  //the limit of the size of alternative paths
const double obj_w1 = 1;    //weight of the energy-related total distance in the objective
const double obj_w2 = 1;    //weight of the total trip duration in the objective
const double obj_w3 = 10;   //weight of the total number of unserved requests


#endif