#ifndef VEHICLES_H_
#define VEHICLES_H_

#include <vector>
#include "couplingVRP/model/config.h"
using namespace std;

class RawInstance;

/// @brief the class is to define the features of all vehicles and the methods to set, modify and get the vehicles information
class Vehicles
{
    private:
        //! a copy of raw data of input instance
        RawInstance* rawInstance;

        //! the total number of vehicles
        int veh_num;

        //! the vehicle speed in the system
        int veh_speed;

        //! the type of each vehicle
        vector<int> veh_types;

        //! the capacity of each vehicle
        vector<int> veh_cap;

        //! the waiting time limit per node of each vehicle
        vector<int> veh_wlim_pernode;

        //! the maximum waiting time limit of each vehicle for all nodes
        vector<int> veh_wlim_max;

        //! the maximum distance range of each vehicle for all nodes
        double veh_range;

        //! the maximum platoon length of vehicles 
        int veh_plmax;

        //! set the types for each MAV based on the proprotion of each customer type
        void set_veh_types(double prob = PAS_MAV_PROP);

        //! set the capaicty of each MAV based on its type
        void set_veh_cap();

        //! set the waiting time limit for each vehicle
        void set_wait_limit();

        //! build the complete information of all nodes
        void buildVehsStruct();

    public:
        //! constructor
        Vehicles(RawInstance& inputInstance);

        //! destructor
        ~Vehicles() {};

        //! a simple getter
        int getVehNum() {return veh_num;}; 

        //! a simple getter
        int getVehSpeed() {return veh_speed;};

        //! a simple getter
        int getVehType(int vehid) {return veh_types[vehid];};

        //! a simple getter
        int getVehCap(int vehid) {return veh_cap[vehid];};

        //! a simple getter
        int getVehWaitTimePerNode(int vehid) {return veh_wlim_pernode[vehid];};

        //! a simple getter
        int getVehWaitTimeMax(int vehid) {return veh_wlim_max[vehid];};

        //! a simple getter
        double getVehRange() {return veh_range;};

        //! a simple getter
        int getPlatoonMaxLen() {return veh_plmax;};

        //! a simple setter
        void setVehSpeed(int input_speed) {veh_speed = input_speed;};
};


#endif