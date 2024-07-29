#ifndef VEHICLES_H_
#define VEHICLES_H_

#include <vector>
#include "couplingVRP/model/RawInstance.h"
#include "couplingVRP/model/VehiclesManager.h"
using namespace std;


/// @brief the class is to define the features of all vehicles and the methods to set, modify and get the vehicles information
class Vehicles
{
    private:
        //! a copy of raw data of input instance
        RawInstance rawInstance;

        //! a manager of processing nodes data
        VehiclesManager VehsMan;
        
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
        int veh_wlim_max;

        //! the maximum distance range of each vehicle for all nodes
        double veh_range;

        //! the maximum platoon length of vehicles 
        int veh_plmax;

        //! build the complete information of all nodes
        void buildVehsStruct();

    public:
        Vehicles(RawInstance& inputInstance, VehiclesManager& VehsMan);
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
        int getVehWaitTimeMax() {return veh_wlim_max;};

        //! a simple getter
        double getVehRange() {return veh_range;};

        //! a simple getter
        int getPlatoonMaxLen() {return veh_plmax;};
};


#endif