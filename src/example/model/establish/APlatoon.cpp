#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include <assert.h>
#include "example/model/establish/APlatoon.h"
#include "example/model/basic/Nodes.h"
using namespace std;

APlatoon::APlatoon(Nodes& nodeset)
{
    this->nodeset = &nodeset;
}

APlatoon::APlatoon(APlatoon& p)
{
    this->nodeset = p.nodeset;
    this->arc = p.arc;
    this->config = p.config;
    this->overlap_deptw = p.overlap_deptw;
    this->arclen = p.arclen;
    this->plen = p.plen;
    this->energy_saving = p.energy_saving;
}

APlatoon::~APlatoon() 
{
    // delete nodeset;
}

bool APlatoon::operator ==(const APlatoon& other) const
{
    if (this->arc != other.arc) return false; 
    return this->config == other.config;
}

bool APlatoon::operator <(const APlatoon& other) const
{
    return this->energy_saving < other.energy_saving;
} 

bool APlatoon::operator >(const APlatoon& other) const
{
    return this->energy_saving >= other.energy_saving;
} 

vector<int> APlatoon::getCommonRoutes()
{
    vector<int> commonVehs;
    for(int i = 0; i < config.size(); i++)
    {
        commonVehs.push_back(config[i].first);
    }
    return commonVehs;
}

double APlatoon::calArcLen()
{
    return nodeset->getInitialDist()[arc[0]][arc[1]];;
}

int APlatoon::calPLen()
{
    plen = config.size();
    return plen;
}

void APlatoon::setArc(vector<int> inputArc)
{
    this->arc = inputArc;
    this->arclen = calArcLen();
}

void APlatoon::setConfig(vector<pair<int, int>> inputConfig)
{
    this->config = inputConfig;
    this->plen = calPLen();
}

double APlatoon::calEnergySaving()
{
    //(n - (1+0.9(n-1)))d = 0.1(n-1)d
    energy_saving = 0.1*(plen-1)*arclen;
    return energy_saving;
}

void APlatoon::removeOneVeh(int vehid)
{
    auto it = find_if(config.begin(), config.end(), [&](pair<int, int> vehpos) -> bool {return vehpos.first == vehid;});
    if(it != config.end()) 
    {
        config.erase(it);
    }
}