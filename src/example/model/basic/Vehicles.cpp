#include <iostream>  
#include <vector>  
#include <cmath>
#include <cstdlib>  
#include <algorithm>  
#include <cassert>
#include "example/model/basic/Vehicles.h"
#include "example/model/basic/RawInstance.h"
#include "example/model/basic/config.h"

Vehicles::Vehicles(RawInstance& inputInstance)
{
    this->rawInstance = &inputInstance;
    veh_num = rawInstance->extract_vehnum();
    veh_speed = rawInstance->extract_vehspeed();
    designVehTypes(); 
    setVehCap();
    setWaitLimit();
    veh_range = MAX_DIST;
    veh_plmax = PLEN_MAX;
}

Vehicles::Vehicles(RawInstance& inputInstance, vector<int> input_vehtype)
{
    this->rawInstance = &inputInstance;
    veh_num = rawInstance->extract_vehnum();
    veh_speed = rawInstance->extract_vehspeed();
    veh_types = input_vehtype;
    setVehCap();
    setWaitLimit();
    veh_range = MAX_DIST;
    veh_plmax = PLEN_MAX;
}

Vehicles::~Vehicles()
{
    // delete rawInstance;  //! not creating new memory
}


void Vehicles::designVehTypes(double prob)
{
    veh_types.resize(veh_num);
    int pas_mavs = int(ceil(veh_num * prob));
    if(prob == 0)
    {
        for(int i = 0; i < veh_num; i++)
        {
            veh_types[i] = 1;
        }
    }
    else if(prob == 1)
    {
        for(int i = 0; i < veh_num; i++)
        {
            veh_types[i] = 0;
        }
    }
    else
    {
        for(int i = 0; i < pas_mavs; i++)
        {
            veh_types[i] = 0;
        }
        for(int i = pas_mavs; i < veh_num; i++)
        {
            veh_types[i] = 1;
        }
    }
}

void Vehicles::setVehCap()
{
    veh_cap.resize(veh_num);
    for(int i = 0; i < veh_num; i++)
    {
        veh_cap[i] = (veh_types[i] == 0) ? VCAP_0: VCAP_1;
    }
}

void Vehicles::setWaitLimit()
{
    veh_wlim_pernode.resize(veh_num);
    veh_wlim_max.resize(veh_num);
    for(int i = 0; i < veh_num; i++)
    {
        switch (veh_types[i])
        {
        case 0: //passenger type
            veh_wlim_pernode[i] = WAIT_0;
            veh_wlim_max[i] = WAIT_MAX_0;
            break;
        case 1: //freight type
            veh_wlim_pernode[i] = WAIT_1;
            veh_wlim_max[i] = WAIT_MAX_1;
        }
    }
}