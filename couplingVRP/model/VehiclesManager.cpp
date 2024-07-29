#include <iostream>  
#include <vector>  
#include <cmath>
#include <cstdlib>  
#include <algorithm>  
#include <cassert>
#include "couplingVRP/model/VehiclesManager.h"
#include "couplingVRP/model/config.h"
using namespace std;  

vector<int> VehiclesManager::set_mavtype(int mav_num, double prob = PAS_MAV_PROP)
{
    vector<int> mav_type(mav_num);
    int pas_mavs = int(ceil(mav_num * prob));
    if(prob == 0)
    {
        for(int i = 0; i < mav_num; i++)
        {
            mav_type[i] = 1;
        }
    }
    else if(prob == 1)
    {
        for(int i = 0; i < mav_num; i++)
        {
            mav_type[i] = 0;
        }
    }
    else
    {
        for(int i = 0; i < pas_mavs; i++)
        {
            mav_type[i] = 0;
        }
        for(int i = pas_mavs; i < mav_num; i++)
        {
            mav_type[i] = 1;
        }
    }
    return mav_type;
}

vector<int> VehiclesManager::set_mavcap(vector<int> mav_type)
{
    vector<int> mav_cap(int(mav_type.size()));
    for(int i = 0; i < mav_type.size(); i++)
    {
        mav_cap[i] = (mav_type[i] == 0) ? VCAP_0: VCAP_1;
    }
    return mav_cap;
}

vector<int> VehiclesManager::set_mavwlim(vector<int> mav_type)
{
    vector<int> mav_wlim(int(mav_type.size()));
    for(int i = 0; i < mav_type.size(); i++)
    {
        mav_wlim[i] = (mav_type[i] == 0) ? WAIT_0: WAIT_1;
    }
    return mav_wlim;
}