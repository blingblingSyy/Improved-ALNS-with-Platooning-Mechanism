// NodesManager.h

#ifndef _NODESMANAGER_H_    //include guards
#define _NODESMANAGER_H_

#include <vector>
#include "couplingVRP/model/config.h"
using namespace std;

/// @brief a class to process the data for vehicles
class VehiclesManager
{
    public:
        VehiclesManager() {};
        ~VehiclesManager() {}; 

        //! set the types for each MAV based on the proprotion of each customer type
        vector<int> set_mavtype(int mav_num, double prob = PAS_MAV_PROP);

        //! set the capaicty of each MAV based on its type
        vector<int> set_mavcap(vector<int> mav_type);

        //! set the waiting time limit for each vehicle
        vector<int> set_mavwlim(vector<int> mav_type);

};


#endif