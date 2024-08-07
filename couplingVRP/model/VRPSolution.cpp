#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>  
#include <cassert>
#include <tuple>
#include "couplingVRP/model/VRPSolution.h"
#include "couplingVRP/model/Nodes.h"
#include "couplingVRP/model/Vehicles.h"
#include "couplingVRP/model/ARoute.h"
#include "couplingVRP/model/TimeWindowUpdater.h"
#include "couplingVRP/model/APlatoon.h"
#include "couplingVRP/model/PlatoonMaker.h"
#include "couplingVRP/model/utility.h"
#include "couplingVRP/model/config.h"
#include "src/alns/ISolution.h"

VRPSolution::VRPSolution(Nodes& nodeset, Vehicles& vehset)
{
    this->nodeset = &nodeset;
    this->vehset = &vehset;
    for(int i = 1; i < nodeset.getNodeNum(); i++)
    {
        nonInsertedNodes.push_back(i);  //{1,2,3,4,5,6,7,8,9}
    }
    for(int v = 0; v < vehset.getVehNum(); v++)
    {
        nonUsedVehs.push_back(v); //{0,1,2,3,4,5}
    }
}

VRPSolution::~VRPSolution()
{
    nonInsertedNodes.clear();
    nonUsedVehs.clear();
}

ARoute* VRPSolution::buildNewRoute()
{
    ARoute* new_route = new ARoute(*nodeset, *vehset);
    if(nonInsertedNodes.empty() || nonUsedVehs.empty()) //! if any set is empty, return an empty route
    {
        return new_route;
    }
    else //! if neither set is empty, do the following
    {
        //! randomly pick a customer to be inserted (note that the customer should not have nodetype of 2, which is for the intersection)
        RandomNumber r;
        int pick_cus = r.get_rint(1, nonInsertedNodes.size()); //0;
        int cus_type = nodeset->getNodeType(pick_cus);
        //! after picking the customer, check whether there are lefting vehicles of the same type as the customer type to be inserted
        vector<int>::iterator veh_it = find_if(nonUsedVehs.begin(), nonUsedVehs.end(), [&](int x) -> bool {return vehset->getVehType(x) == cus_type;});
        if(veh_it != nonUsedVehs.end())  //! there are at least one vehicle of the same type as the chosen customer
        {
            new_route->setVehIdType((*veh_it));
            new_route->evaluateRouteByInsertNode(1, pick_cus);
            double insert_cost = new_route->getRouteOperatorCosts(ARoute::InsertOneNode);
            if(insert_cost < INF)
            {
                new_route->setRouteByInsertNode(1, pick_cus);
                nonInsertedNodes.erase(remove(nonInsertedNodes.begin(), nonInsertedNodes.end(), pick_cus));
                nonUsedVehs.erase(veh_it);
            }
            return new_route;
        }
        else //! there are no lefting vehicles of the same type as the selected customer type -> change a customer to create a new route
        {
            vector<int>::iterator new_cus_it = find_if(nonInsertedNodes.begin(), nonInsertedNodes.end(), [&](int x) -> bool {return nodeset->getNodeType(x) != cus_type;});
            if(new_cus_it != nonInsertedNodes.end()) //! if not, the remaining customers and vehicles are incompatible
            {
                new_route->setVehIdType(nonUsedVehs[0]);
                new_route->evaluateRouteByInsertNode(1, (*new_cus_it));
                double insert_cost = new_route->getRouteOperatorCosts(ARoute::InsertOneNode);
                if(insert_cost < INF) 
                {
                    new_route->setRouteByInsertNode(1, (*new_cus_it));
                    nonInsertedNodes.erase(new_cus_it);
                    nonUsedVehs.erase(nonUsedVehs.begin());
                }
            }
            return new_route;
        }
    }
}

void VRPSolution::buildInitialSol()
{
    double start = clock(); // get current time
    
    while(!nonInsertedNodes.empty() && !nonUsedVehs.empty())
    {
        ARoute* new_route = buildNewRoute(); //! this will erase one customer and vehicle from the two sets
        if(!new_route->isEmpty())
        {
            vector<int> unserved_cus_id_same_type;
            for(auto iter = nonInsertedNodes.begin(); iter != nonInsertedNodes.end(); iter++)
            {
                if(nodeset->getNodeType(*iter) == new_route->getVehType())
                {
                    unserved_cus_id_same_type.push_back(*iter);
                }
            }
            while(!unserved_cus_id_same_type.empty())
            {
                //! for all uninserted customers, insert all possible customers in the current route (start with the customer with the least insertion cost)
                vector<pair<double, int>> insert_costs_set;    //to store the cheapest insertion costs of all uninserted customers in the route
                for(auto iter = unserved_cus_id_same_type.begin(); iter != unserved_cus_id_same_type.end(); iter++)
                {
                    pair<double, int> cus_insert_cost = new_route->calCheapestInsertionCosts(*iter);
                    insert_costs_set.push_back(cus_insert_cost);
                }
                auto selected_cost_iter = min_element(insert_costs_set.begin(), insert_costs_set.end(), [&](auto x, auto y){return x.first < y.first;});
                double selected_cost = (*selected_cost_iter).first;  //! the minimum insertion costs among all customers
                int selected_insert_pos = (*selected_cost_iter).second;
                int selected_cost_index = selected_cost_iter - insert_costs_set.begin();
                int selected_node = unserved_cus_id_same_type[selected_cost_index];  //! the customer with the minium insertion cost
                if(selected_cost < INF) //selected_cost < INF -> feasible
                {
                    new_route->setRouteByInsertNode(selected_insert_pos, selected_node);
                    unserved_cus_id_same_type.erase(unserved_cus_id_same_type.begin() + selected_cost_index);  
                    nonInsertedNodes.erase(remove(nonInsertedNodes.begin(), nonInsertedNodes.end(), selected_node)); //! remove the customer from the set of uninserted customers
                }
                else
                {
                    break;
                }
            }
            sol_config.push_back(new_route);
        } 
        else //in case that the remaining customers and vehicles are incompatible
        {
            cerr << "No initial solution can be built from the given set of customers and vehicles.";
            break;
        }
    }

    //need to be deleted later
    sol_config[1]->setRouteByModifyUsedPath(0, 1);
    sol_config[1]->setRouteByModifyUsedPath(0, 1);

    cpuBeforePlatooning = (clock() - start ) / (double) CLOCKS_PER_SEC;
    makePlatoons();
    recomputeCost();
    cpuAfterPlatooning = (clock() - start ) / (double) CLOCKS_PER_SEC;
}

bool VRPSolution::isFeasible()
{
    for(int i = 0; i < getRoutesNum(); i++)
    {
        if(!sol_config[i]->isRouteFeas()) return false;
    }
    return true;
}

bool VRPSolution::operator<(ISolution& s)
{
	if((isFeasible() && s.isFeasible()) || (!isFeasible() && !s.isFeasible()))
	{
		return static_cast<long long>(1000*getObjectiveValue()) < static_cast<long long>(1000*s.getObjectiveValue());
	}
	else
	{
		return isFeasible();
	}
}

int VRPSolution::distance(ISolution& s)
{
    return getObjectiveValue() - s.getObjectiveValue();
}

bool VRPSolution::isSolEmpty()
{
    return (sol_config.size() == 0);
}

void VRPSolution::makePlatoons()
{
    if(sol_config.size() > 1)
    {
        PlatoonMaker* plmaker = new PlatoonMaker(*this, *nodeset);
        platoons_config = plmaker->getAllPlatoons();
        plmaker->setArrDepTimeAllRoutes();
        totalEnergySaving = plmaker->calSolTotalEnergySaving();
    }
}

void VRPSolution::setFinalArrDepTime()
{
    for(int i = 0; i < getRoutesNum(); i++)
    {
        sol_config[i]->calArrDepTime();
    }
}

double VRPSolution::calTotalDist()
{
    totalDistCostsBeforePlatooning = 0;
    for(int i = 0; i < getRoutesNum(); i++)
    {
        totalDistCostsBeforePlatooning += sol_config[i]->getRouteDistance();
    }
    return totalDistCostsBeforePlatooning;
}

double VRPSolution::calTotalEnergyDist()
{
    totalDistCostsAfterPlatooning = totalDistCostsBeforePlatooning - totalEnergySaving;
    return totalDistCostsAfterPlatooning;
}

int VRPSolution::calTotalTripDuration()
{
    totalTripDurationAllRoutes = 0;
    for(int i = 0; i < getRoutesNum(); i++)
    {
        totalTripDurationAllRoutes += sol_config[i]->getRouteDuration();
    }
    return totalTripDurationAllRoutes;
}

int VRPSolution::calTotalUnservedRequests()
{
    return nonInsertedNodes.size();
}

double VRPSolution::calTotalObjectiveValue()
{
    return obj_w1*totalDistCostsAfterPlatooning + obj_w2*totalTripDurationAllRoutes + obj_w3*totalUnservedRequests;
    // return obj_w1*calTotalEnergyDist() + obj_w2*calTotalTripDuration() + obj_w3*calTotalUnservedRequests();
}

double VRPSolution::getPenalizedObjectiveValue(bool modified = false)
{
    if(!modified) return totalObjValueAfterPlatooning;
    else return (obj_w1*totalDistCostsAfterPlatooning + obj_w2*totalTripDurationAllRoutes);
}

void VRPSolution::recomputeCost()
{
    totalDistCostsBeforePlatooning = 0;
    totalEnergySaving = 0;
    totalDistCostsAfterPlatooning = 0;
    totalTripDurationAllRoutes = 0;
    totalUnservedRequests = 0;
	totalObjValueAfterPlatooning = 0;
    for(int i = 0; i < getRoutesNum(); i++)
    {
        totalDistCostsBeforePlatooning += sol_config[i]->getRouteDistance();
        totalTripDurationAllRoutes += sol_config[i]->getRouteDuration();
    }
    for(int p = 0; p < getPlatoonNum(); p++)
    {
        totalEnergySaving += platoons_config[p]->calEnergySaving();
    }
    totalDistCostsAfterPlatooning = totalDistCostsBeforePlatooning - totalEnergySaving;
    totalUnservedRequests = nonInsertedNodes.size();
    totalObjValueAfterPlatooning = obj_w1*totalDistCostsAfterPlatooning + obj_w2*totalTripDurationAllRoutes + obj_w3*totalUnservedRequests;
}

ISolution* VRPSolution::getCopy()
{
	VRPSolution* newSol = new VRPSolution(*nodeset, *vehset);
    newSol->getNonInsertedNodes().clear();
    for(int i = 0; i < getNonInsertedNodes().size(); i++)
    {
        newSol->getNonInsertedNodes().push_back(getNonInsertedNodes()[i]);
    }
    newSol->getNonUsedVehs().clear();
    for(int v = 0; v <getNonUsedVehs().size(); v++)
    {
        newSol->getNonUsedVehs().push_back(getNonUsedVehs()[v]);
    }
    newSol->getAllRoutes().clear();
    for(int r = 0; r < getRoutesNum(); r++)
    {
        newSol->getAllRoutes().push_back(sol_config[r]);
    }
    newSol->getAllPlatoons().clear();
    for(int p = 0; p < getPlatoonNum(); p++)
    {
        newSol->getAllPlatoons().push_back(platoons_config[p]);
    }
	newSol->recomputeCost();
	return dynamic_cast<ISolution*>(newSol);
}

long long VRPSolution::getHash()
{
	//recomputeCost();
    double hash = 1;
    hash = 100 * calTotalEnergyDist() * calTotalTripDuration() + 50 * calTotalUnservedRequests();
	return static_cast<long long>(hash);
}

void VRPSolution::insertNode(int insert_pos, int insert_nodeid, int routeid)
{
    ARoute* original_route = sol_config[routeid];
    auto it = find(nonInsertedNodes.begin(), nonInsertedNodes.end(), insert_nodeid);
    if(it != nonInsertedNodes.end()) //! make sure that the inserted node is in the nonInsertedNodes set
    {
        double insertcosts = evaluateInsertNode(insert_pos, insert_nodeid, routeid);
        if(insertcosts < INF) //! only insert when the insertion is feasible
        {
            sol_config[routeid]->setRouteByInsertNode(insert_pos, insert_nodeid);
            nonInsertedNodes.erase(it);
            //! compute the cost for the solution after the insertion
            totalDistCostsBeforePlatooning += insertcosts;
            totalUnservedRequests -= 1;
            totalTripDurationAllRoutes = totalTripDurationAllRoutes - original_route->getRouteDuration() + sol_config[routeid]->getRouteDuration();
        }
    }
}

void VRPSolution::removeNode(int remove_pos, int routeid)
{
    ARoute* original_route = sol_config[routeid];
    double removalcosts = evaluateRemoveNode(remove_pos, routeid);
    int remove_nodeid = sol_config[routeid]->getCompactRoute()[remove_pos];
    if(removalcosts < INF) //only remove when the removal is feasible
    {
        sol_config[routeid]->setRouteByRemoveNode(remove_pos);
        nonInsertedNodes.push_back(remove_nodeid);
        //! compute the removal costs for the solution after the removal
        totalDistCostsBeforePlatooning += removalcosts;
        totalUnservedRequests += 1;
        totalTripDurationAllRoutes = totalTripDurationAllRoutes - original_route->getRouteDuration() + sol_config[routeid]->getRouteDuration();
    }
}

void VRPSolution::modifyPath(int modified_arcpos, int modified_pathid, int routeid)
{
    ARoute* original_route = sol_config[routeid];
    double modifycosts = evaluateModifyPath(modified_arcpos, modified_pathid, routeid);
    if(modifycosts < INF) //only modify when the path modification is feasible
    {
        sol_config[routeid]->setRouteByModifyUsedPath(modified_arcpos, modified_pathid);
        //! compute the modification costs for the solution after the modification
        totalDistCostsBeforePlatooning += modifycosts;
        totalTripDurationAllRoutes = totalTripDurationAllRoutes - original_route->getRouteDuration() + sol_config[routeid]->getRouteDuration();
    }
}

double VRPSolution::evaluateInsertNode(int insert_pos, int insert_nodeid, int routeid)
{
    sol_config[routeid]->evaluateRouteByInsertNode(insert_pos, insert_nodeid);
    return sol_config[routeid]->getRouteOperatorCosts(ARoute::InsertOneNode);
}

double VRPSolution::evaluateRemoveNode(int remove_pos, int routeid)
{
    sol_config[routeid]->evaluateRouteByRemoveNode(remove_pos);
    return sol_config[routeid]->getRouteOperatorCosts(ARoute::RemoveOneNode);
}

double VRPSolution::evaluateModifyPath(int modified_arcpos, int modified_pathid, int routeid)
{
    sol_config[routeid]->evaluateRouteByModifyUsedPath(modified_arcpos, modified_pathid);
    return sol_config[routeid]->getRouteOperatorCosts(ARoute::ModifyOnePath);
}