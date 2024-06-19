// PlatoonMaker.h

#ifndef _PLATOONMAKER_H_    //include guards
#define _PLATOONMAKER_H_

#include <vector>
#include <string>
#include "config.h"
using namespace std;

//update the arrival and departure time windows within a route
class PlatoonMaker
{
    private: 
        Route route;
        Nodes nodeset;
        int routelen;
    public:
        PlatoonMaker(Route &input_route, Nodes &nodes);
};

#endif