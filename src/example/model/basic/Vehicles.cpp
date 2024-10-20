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
    pas_vehnum = rawInstance->getVehNum(0);
    fre_vehnum = rawInstance->getVehNum(1);
    vehnum = rawInstance->getVehNum();
    veh_speed = rawInstance->getVehSpeed();
    veh_types.resize(vehnum);
    veh_cap.resize(vehnum);
    veh_wlim_pernode.resize(vehnum);
    veh_wlim_max.resize(vehnum);
    for(int i = 0; i < vehnum; i++)
    {
        veh_types[i] = (i < pas_vehnum) ? 0 : 1;
        veh_cap[i] = (i < pas_vehnum) ? rawInstance->getVehCap(0) : rawInstance->getVehCap(1);
        veh_wlim_pernode[i] = (i < pas_vehnum) ? WAIT_0 : WAIT_1;
        veh_wlim_max[i] = (i < pas_vehnum) ? WAIT_MAX_0 : WAIT_MAX_1;
    }
    veh_range = MAX_DIST;
    veh_plmax = PLEN_MAX;
}

Vehicles::~Vehicles()
{

}

void Vehicles::setVehNum(int type, int vehnum)
{
    if(type == 0)
    {
        this->pas_vehnum = vehnum;
    }
    else if(type == 1)
    {
        this->fre_vehnum = vehnum;
    }
    assert(false);
}

void Vehicles::setVehCap(int type, int vehcap)
{
    if(type == 0)
    {
        for(int i = 0; i < pas_vehnum; i++)
        {
            veh_cap[i] = vehcap;
        }
    }
    else if(type == 1)
    {
        for(int i = pas_vehnum; i < vehnum; i++)
        {
            veh_cap[i] = vehcap;
        }
    }
    assert(false);
}

void Vehicles::setWaitLimitPerNode(int type, int waitlim)
{
    if(type == 0)
    {
        for(int i = 0; i < pas_vehnum; i++)
        {
            veh_wlim_pernode[i] = waitlim;
        }
    }
    else if(type == 1)
    {
        for(int i = pas_vehnum; i < vehnum; i++)
        {
            veh_wlim_pernode[i] = waitlim;
        }
    }
    assert(false);
}

void Vehicles::setWaitLimitMax(int type, int waitlim)
{
    if(type == 0)
    {
        for(int i = 0; i < pas_vehnum; i++)
        {
            veh_wlim_max[i] = waitlim;
        }
    }
    else if(type == 1)
    {
        for(int i = pas_vehnum; i < vehnum; i++)
        {
            veh_wlim_max[i] = waitlim;
        }
    }
    assert(false);
}