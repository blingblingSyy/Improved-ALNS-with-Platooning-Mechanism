// Config.h //for model config

#ifndef _CONFIG_H_    //include guards
#define _CONFIG_H_

#include <string> 
#include <vector>
using namespace std;

/*special numbers*/
#define INF 0x3f3f3f3f   // define a very large number
#define SMALL 1e-4    // define a very small number

/*network information*/
const double AREA_SIZE = 20; //km^2
const int SPEED = 0.5;      //MAV speed (km/h)
const int PLEN_MAX = 10;  //the maximum length of platoons
const int MAX_DIST = 200;   //MAV maximum range (km)
const int VTYPE = 2;        //the number of MAV types
const int VCAP_0 = 6;       //MAV capacity for passenger
const int VCAP_1 = 15;      //MAV capacity for freight
const int WAIT_0 = 1;       //waiting time limit for passenger-type AMV at each location
const int WAIT_1 = 3;       //waiting time limit for freight-type AMV at each location
const int WAIT_MAX_0 = 5;    //waiting time limit for passenger-type AMV at all locations
const int WAIT_MAX_1 = 8;    //waiting time limit for freight-type AMV at all locations
const int SERT_0 = 1;       //constant service time for passenger node (for self-designed data)
const int SERT_1 = 3;       //constant service time for freight node (for self-designed data)
const double SER_RATE_0 = 0.5;      //service rate for passenger node (for self-designed data)
const double SER_RATE_1 = 0.3;      //service rate for freight node (for self-designed data)
const int MIN_SER_PRD_0 = 5;   //minimum service period for passenger node (for self-designed data)
const int MAX_SER_PRD_0 = 20;   //maximum service period for passenger node (for self-designed data)
const int MIN_SER_PRD_1 = 30;   //minimum service period for freight node (for self-designed data)
const int MAX_SER_PRD_1 = 60;   //maximum service period for freight node (for self-designed data)
const vector<int> DMD_RANGE_0 = {1,VCAP_0};   //demand distribution range for passenger node (for self-designed data)
const vector<int> DMD_RANGE_1 = {1,VCAP_1};   //demand distribution range for freight node (for self-designed data)
const double PAS_CUS_PROP = 0.5;    //ratio of passenger requests
const double FRE_CUS_PROP = 0.4;    //ratio of freight requests (the remaining belongs to intersections)
const double INTERSECTIONS_PROB = 0.1; //probability of changing a node to be an intersection
const double DISCONNECTION_PROB = 0.8; //probability of disconnecting the direct link between a given pair of nodes
const vector<int> PLAN_HORIZON = {0, 960}; //the planning horizon is from 6 am to 10 pm
const vector<int> PAS_DMD_PEAK = {180, 720}; //the peak period for the passenger demands is at 9 am and 6 pm
const vector<int> FRE_DMD_PEAK = {240, 540}; //the peak period for the passenger demands is at 10 am and 3 pm
const double PICKUP_DROPOFF_PROB = 0.4; // the probability of having pickup requests overall all customers

/*model parameters*/
const int AVAIL_PATHSET_SIZE_K = 5;  //the limit of the size of alternative paths
const double obj_w1 = 20;    //weight of the energy-related total distance in the objective
const double obj_w2 = 1;    //weight of the total trip duration in the objective
const double obj_w3 = 10000;   //weight of the total number of unserved requests
const double fp = 0.8;   //Freight to passenger weight ratio of total unserved request

#endif