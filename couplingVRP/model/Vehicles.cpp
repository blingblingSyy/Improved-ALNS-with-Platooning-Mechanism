#include <iostream>  
#include <vector>  
#include <cmath>
#include <cstdlib>  
#include <algorithm>  
#include <cassert>
#include "couplingVRP/model/Vehicles.h"
#include "couplingVRP/model/RawInstance.h"
#include "couplingVRP/model/config.h"

Vehicles::Vehicles(RawInstance& inputInstance)
{
    this->rawInstance = &inputInstance;
    veh_num = rawInstance->extract_vehnum();
    buildVehsStruct();
}

void Vehicles::buildVehsStruct()
{
    veh_speed = rawInstance->extract_vehspeed();
    set_veh_types(veh_num); 
    set_veh_cap();
    set_wait_limit();
    veh_range = MAX_DIST;
    veh_plmax = PLEN_MAX;
}

void Vehicles::set_veh_types(double prob = PAS_MAV_PROP)
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

void Vehicles::set_veh_cap()
{
    veh_cap.resize(veh_num);
    for(int i = 0; i < veh_num; i++)
    {
        veh_cap[i] = (veh_types[i] == 0) ? VCAP_0: VCAP_1;
    }
}

void Vehicles::set_wait_limit()
{
    veh_wlim_pernode.resize(veh_num);
    veh_wlim_max.resize(veh_num);
    for(int i = 0; i < veh_num; i++)
    {
        switch (veh_types[i])
        {
        case 0: //passenger type
            veh_wlim_pernode[i] = WAIT_0;
            veh_wlim_max[i] = WAIT_MAX;
            break;
        case 1: //freight type
            veh_wlim_pernode[i] = WAIT_1;
            veh_wlim_max[i] = WAIT_1 * veh_num;
        }
    }
}