#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include <assert.h>
#include "couplingVRP/model/APlatoon.h"
#include "couplingVRP/model/Nodes.h"
using namespace std;

APlatoon::APlatoon(Nodes& nodeset)
{
    this->nodeset = &nodeset;
}

APlatoon::APlatoon(APlatoon& p)
{
    this->arc = p.arc;
    this->config = p.config;
    this->overlap_deptw = p.overlap_deptw;
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

vector<int> APlatoon::getRouteidsInPlatoon()
{
    vector<int> commonVehs;
    for(int i = 0; i < config.size(); i++)
    {
        commonVehs.push_back(config[i].first);
    }
}

double APlatoon::calArcLen()
{
    return nodeset->getInitialDist()[arc[0]][arc[1]];;
}

int APlatoon::calPLen()
{
    return config.size();
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
    return 0.1*(plen-1)*arclen;
}