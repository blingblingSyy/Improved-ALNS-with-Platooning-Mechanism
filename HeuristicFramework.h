// HeuristicFramework.h

#ifndef _HEURISTICFRAMEWORK_H_    //include guards
#define _HEURISTICFRAMEWORK_H_

#include <vector>
#include <string>
#include "ALNS_Parameters.h"
#include "config.h"
using namespace std;

//set up the framework for ALNS heuristics
class HeuristicFramework  
{
    protected: 
        Solution sol;

    public:
        HeuristicFramework();
        void find_vehicles_set_in_sol();
        void set_extend_route(Route &route);
        void set_route_labels(Route &route);
        void set_route_loads(Route &route);
        void set_route_mileage(Route &route);
        void set_route_arrtime(Route &route);
};

#endif
