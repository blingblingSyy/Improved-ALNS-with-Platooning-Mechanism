#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>  
#include <cassert>
#include <tuple>
#include <numeric>
#include "example/model/establish/VRPSolution.h"
#include "example/model/basic/Nodes.h"
#include "example/model/basic/Vehicles.h"
#include "example/model/establish/ARoute.h"
#include "example/model/establish/TimeWindowUpdater.h"
#include "example/model/establish/APlatoon.h"
#include "example/model/establish/PlatoonMaker.h"
#include "utility.h"
#include "example/model/basic/config.h"
#include "alns/improvedALNS/ISolution.h"

VRPSolution::VRPSolution(Nodes& nodeset, Vehicles& vehset)
{
    this->nodeset = &nodeset;
    this->vehset = &vehset;
    this->nonInsertedNodes.resize(2);
    this->nonUsedVehs.resize(2);
    for(int i = 1; i < nodeset.getNodeNum(); i++)
    {
        // if(nodeset.getNodeType(i) != 2)
        // {
        //     nonInsertedNodes.push_back(i);
        // }
        if(nodeset.getNodeType(i) == 0)
        {
            nonInsertedNodes[0].push_back(i);  //{1,2,3,4,5,6,7,8,9}
        }
        else if(nodeset.getNodeType(i) == 1)
        {
            nonInsertedNodes[1].push_back(i);  //{1,2,3,4,5,6,7,8,9}
        }
    }
    for(int v = 0; v < vehset.getVehNum(); v++)
    {
        if(vehset.getVehType(v) == 0)
        {
            nonUsedVehs[0].push_back(v);  //{1,2,3,4,5,6,7,8,9}
        }
        else if(vehset.getVehType(v) == 1)
        {
            nonUsedVehs[1].push_back(v);  //{1,2,3,4,5,6,7,8,9}
        }
        // nonUsedVehs.push_back(v); //{0,1,2,3,4,5}
    }
    getDestroyedArcsPos().clear();
}

VRPSolution::~VRPSolution()
{
    nonInsertedNodes.clear();
    nonUsedVehs.clear();
    destroyedArcPos.clear();
    for(int i = 0; i < getRoutesNum(); i++)
    {
        delete sol_config[i];
    }
    sol_config.clear();
    for(int i = 0; i < getPlatoonNum(); i++)
    {
        delete platoons_config[i];
    }
    platoons_config.clear();
}

void VRPSolution::findDestroyablePaths()
{
    destroyableArcConfig.clear();
    destroyableArcPos.clear();
    for(int i = 0; i < sol_config.size(); i++)
    {
        vector<int> compact_route_i = sol_config[i]->getCompactRoute();
        for(int j = 0; j < compact_route_i.size()-1; j++)
        {
            if(nodeset->getAllAvailPathSize()[compact_route_i[j]][compact_route_i[j+1]] > 1)
            {
                //! destroyableArcPos = {{destroyed_arcpos, destroyed_pathid, routeid}, ...}
                destroyableArcPos.push_back(make_tuple(j, sol_config[i]->getUsedPathsAtArc(j), i));
                destroyableArcConfig.push_back({compact_route_i[j], compact_route_i[j+1]});
            }
        }
    }
}

/*old version*/
// ARoute* VRPSolution::buildNewRoute()
// {
//     ARoute* new_route = new ARoute(*nodeset, *vehset);
//     if(nonInsertedNodes.empty() || nonUsedVehs.empty()) //! if any set is empty, return an empty route
//     {
//         return new_route;
//     }
//     else //! if neither set is empty, do the following
//     {
//         //! randomly pick a customer to be inserted (note that the customer should not have nodetype of 2, which is for the intersection)
//         RandomNumber r;
//         int pick_cus = nonInsertedNodes[r.get_rint(0, nonInsertedNodes.size()-1)]; //0;
//         int cus_type = nodeset->getNodeType(pick_cus);
//         //! after picking the customer, check whether there are lefting vehicles of the same type as the customer type to be inserted
//         vector<int>::iterator veh_it = find_if(nonUsedVehs.begin(), nonUsedVehs.end(), [&](int x) -> bool {return vehset->getVehType(x) == cus_type || cus_type == 2;});
//         if(veh_it != nonUsedVehs.end())  //! there are at least one vehicle of the same type as the chosen customer
//         {
//             new_route->setVehIdType((*veh_it));
//             new_route->evaluateRouteByInsertNode(1, pick_cus);
//             double insert_cost = new_route->getRouteOperatorCosts(ARoute::InsertOneNode);
//             if(insert_cost < INF)
//             {
//                 new_route->setRouteByInsertNode(1, pick_cus);
//                 nonInsertedNodes.erase(remove(nonInsertedNodes.begin(), nonInsertedNodes.end(), pick_cus));
//                 nonUsedVehs.erase(veh_it);
//             }
//             return new_route;
//         }
//         else //! there are no lefting vehicles of the same type as the selected customer type -> change a customer to create a new route
//         {
//             vector<int>::iterator new_cus_it = find_if(nonInsertedNodes.begin(), nonInsertedNodes.end(), [&](int x) -> bool {return nodeset->getNodeType(x) != cus_type;});
//             if(new_cus_it != nonInsertedNodes.end()) //! if not, the remaining customers and vehicles are incompatible
//             {
//                 new_route->setVehIdType(nonUsedVehs[0]);
//                 new_route->evaluateRouteByInsertNode(1, (*new_cus_it));
//                 double insert_cost = new_route->getRouteOperatorCosts(ARoute::InsertOneNode);
//                 if(insert_cost < INF) 
//                 {
//                     new_route->setRouteByInsertNode(1, (*new_cus_it));
//                     nonInsertedNodes.erase(new_cus_it);
//                     nonUsedVehs.erase(nonUsedVehs.begin());
//                 }
//             }
//             return new_route;
//         }
//     }
// }

ARoute* VRPSolution::buildNewRoute(int type)
{
    ARoute* new_route = new ARoute(*nodeset, *vehset);
    if(nonInsertedNodes[type].empty() || nonUsedVehs[type].empty()) //! if any set is empty, return an empty route
    {
        return new_route;
    }
    else //! if neither set is empty, do the following
    {
        //! randomly pick a customer to be inserted (note that the customer should not have nodetype of 2, which is for the intersection)
        RandomNumber r;
        int pick_cus = nonInsertedNodes[type][r.get_rint(0, nonInsertedNodes[type].size()-1)]; //0;
        //! after picking the customer, select the vehicle of the same type for the customer to be inserted
        new_route->setVehIdType(nonUsedVehs[type].front());
        new_route->evaluateRouteByInsertNode(1, pick_cus);
        double insert_cost = new_route->getRouteOperatorCosts(ARoute::InsertOneNode);
        if(insert_cost < INF)
        {
            new_route->setRouteByInsertNode(1, pick_cus);
            nonInsertedNodes[type].erase(remove(nonInsertedNodes[type].begin(), nonInsertedNodes[type].end(), pick_cus));
            nonUsedVehs[type].erase(nonUsedVehs[type].begin());
        }
        return new_route;
    }
}

/*new version -> build based on the cheapest insertion costs*/
// void VRPSolution::buildInitialSol()
// {
//     double start = clock(); // get current time
//     for(int i = 0; i < VTYPE; i++)
//     {
//         while(!nonInsertedNodes[i].empty() && !nonUsedVehs[i].empty())
//         {
//             ARoute* new_route = buildNewRoute(i);  //! this will erase one customer and vehicle from the two sets
//             while(!nonInsertedNodes[i].empty())
//             {
//                 //! for all uninserted customers, insert all possible customers in the current route (start with the customer with the least insertion cost)
//                 vector<pair<double, int>> insert_costs_set;    //to store the cheapest insertion costs of all uninserted customers in the route
//                 for(auto iter = nonInsertedNodes[i].begin(); iter != nonInsertedNodes[i].end(); iter++)
//                 {
//                     pair<double, int> cus_insert_cost = new_route->calCheapestInsertionCosts(*iter);
//                     insert_costs_set.push_back(cus_insert_cost);
//                 }
//                 auto selected_cost_iter = min_element(insert_costs_set.begin(), insert_costs_set.end(), [&](auto x, auto y){return x.first < y.first;});
//                 double selected_cost = (*selected_cost_iter).first;  //! the minimum insertion costs among all customers
//                 int selected_insert_pos = (*selected_cost_iter).second;
//                 int selected_cost_index = selected_cost_iter - insert_costs_set.begin();
//                 int selected_node = nonInsertedNodes[i][selected_cost_index];  //! the customer with the minium insertion cost
//                 if(selected_cost < INF) //selected_cost < INF -> feasible
//                 {
//                     new_route->setRouteByInsertNode(selected_insert_pos, selected_node);
//                     nonInsertedNodes[i].erase(remove(nonInsertedNodes[i].begin(), nonInsertedNodes[i].end(), selected_node)); //! remove the customer from the set of uninserted customers
//                 }
//                 else
//                 {
//                     break;
//                 }
//             }
//             sol_config.push_back(new_route);
//         }
//     }

//     //need to be deleted later
//     //! mip_optimal_solution (integer distance)
//     // sol_config[3]->setRouteByModifyUsedPath(1, 1);
//     // sol_config[3]->setRouteByModifyUsedPath(0, 1);
//     // sol_config[1]->setRouteByModifyUsedPath(0, 1);

//     //! mip_optimal_solution (original distance)
//     // sol_config[1]->setRouteByModifyUsedPath(0, 1);
//     // sol_config[0]->setRouteByModifyUsedPath(1, 1);

//     double cpuBeforePlatooning = clock();
//     cout << "solution building time: " << (cpuBeforePlatooning - start ) / (double) CLOCKS_PER_SEC << "\t";
//     makePlatoons();
//     cout << "Platooning time: " << (clock() - cpuBeforePlatooning ) / (double) CLOCKS_PER_SEC << "\t";
//     recomputeCost();
//     cpuAfterPlatooning = (clock() - start ) / (double) CLOCKS_PER_SEC;
//     cout << "Total time for building initial solution: " << cpuAfterPlatooning << endl;
// }

/*new version -> build randomly*/
void VRPSolution::buildInitialSol()
{
    double start = clock(); // get current time
    cout << "=================Build initial Solution=================" << endl;
    for(int i = 0; i < VTYPE; i++)
    {
        while(!nonInsertedNodes[i].empty() && !nonUsedVehs[i].empty())
        {
            ARoute* new_route = buildNewRoute(i);  //! this will erase one customer and vehicle from the two sets
            if(!new_route->isEmpty())
            {
                vector<int> remaining_noninserted = nonInsertedNodes[i];
                while(!remaining_noninserted.empty())
                {
                    RandomNumber r1;
                    int p = r1.get_rint(0, remaining_noninserted.size()-1);
                    int selected_node = remaining_noninserted[p];
                    pair<double, int> cus_insert_cost = new_route->calCheapestInsertionCosts(selected_node);
                    double selected_cost = cus_insert_cost.first;
                    int selected_insert_pos = cus_insert_cost.second;
                    if(selected_cost < INF) //selected_cost < INF -> feasible
                    {
                        new_route->setRouteByInsertNode(selected_insert_pos, selected_node);
                        nonInsertedNodes[i].erase(remove(nonInsertedNodes[i].begin(), nonInsertedNodes[i].end(), selected_node)); //! remove the customer from the set of uninserted customers
                    }
                    remaining_noninserted.erase(remaining_noninserted.begin()+p);
                }
                sol_config.push_back(new_route);
            }
        }
    }

    //need to be deleted later
    //! mip_optimal_solution (integer distance)
    // sol_config[3]->setRouteByModifyUsedPath(1, 1);
    // sol_config[3]->setRouteByModifyUsedPath(0, 1);
    // sol_config[1]->setRouteByModifyUsedPath(0, 1);

    //! mip_optimal_solution (original distance)
    // sol_config[1]->setRouteByModifyUsedPath(0, 1);
    // sol_config[0]->setRouteByModifyUsedPath(1, 1);

    double cpuBeforePlatooning = clock();
    cout << "solution building time: " << (cpuBeforePlatooning - start ) / (double) CLOCKS_PER_SEC << "\t";
    makePlatoons();
    cout << "Platooning time: " << (clock() - cpuBeforePlatooning ) / (double) CLOCKS_PER_SEC << "\t";
    recomputeCost();
    cpuAfterPlatooning = (clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << "Total time for building initial solution: " << cpuAfterPlatooning << endl;
}

/*old version*/
// void VRPSolution::buildInitialSol()
// {
//     double start = clock(); // get current time
//     while(!nonInsertedNodes.empty() && !nonUsedVehs.empty())
//     {
//         ARoute* new_route = buildNewRoute(); //! this will erase one customer and vehicle from the two sets
//         if(!new_route->isEmpty())
//         {
//             vector<int> unserved_cus_id_same_type;
//             for(auto iter = nonInsertedNodes.begin(); iter != nonInsertedNodes.end(); iter++)
//             {
//                 if(nodeset->getNodeType(*iter) == new_route->getVehType())
//                 {
//                     unserved_cus_id_same_type.push_back(*iter);
//                 }
//             }
//             while(!unserved_cus_id_same_type.empty())
//             {
//                 //! for all uninserted customers, insert all possible customers in the current route (start with the customer with the least insertion cost)
//                 vector<pair<double, int>> insert_costs_set;    //to store the cheapest insertion costs of all uninserted customers in the route
//                 for(auto iter = unserved_cus_id_same_type.begin(); iter != unserved_cus_id_same_type.end(); iter++)
//                 {
//                     pair<double, int> cus_insert_cost = new_route->calCheapestInsertionCosts(*iter);
//                     insert_costs_set.push_back(cus_insert_cost);
//                 }
//                 auto selected_cost_iter = min_element(insert_costs_set.begin(), insert_costs_set.end(), [&](auto x, auto y){return x.first < y.first;});
//                 double selected_cost = (*selected_cost_iter).first;  //! the minimum insertion costs among all customers
//                 int selected_insert_pos = (*selected_cost_iter).second;
//                 int selected_cost_index = selected_cost_iter - insert_costs_set.begin();
//                 int selected_node = unserved_cus_id_same_type[selected_cost_index];  //! the customer with the minium insertion cost
//                 if(selected_cost < INF) //selected_cost < INF -> feasible
//                 {
//                     new_route->setRouteByInsertNode(selected_insert_pos, selected_node);
//                     unserved_cus_id_same_type.erase(unserved_cus_id_same_type.begin() + selected_cost_index);  
//                     nonInsertedNodes.erase(remove(nonInsertedNodes.begin(), nonInsertedNodes.end(), selected_node)); //! remove the customer from the set of uninserted customers
//                 }
//                 else
//                 {
//                     break;
//                 }
//             }
//             sol_config.push_back(new_route);
//         } 
//         else //in case that the remaining customers and vehicles are incompatible
//         {
//             cerr << "No initial solution can be built from the given set of customers and vehicles.";
//             break;
//         }
//     }
//     //need to be deleted later
//     //! mip_optimal_solution (integer distance)
//     // sol_config[3]->setRouteByModifyUsedPath(1, 1);
//     // sol_config[3]->setRouteByModifyUsedPath(0, 1);
//     // sol_config[1]->setRouteByModifyUsedPath(0, 1);
//     //! mip_optimal_solution (original distance)
//     // sol_config[1]->setRouteByModifyUsedPath(0, 1);
//     // sol_config[0]->setRouteByModifyUsedPath(1, 1);
//     // cpuBeforePlatooning = (clock() - start ) / (double) CLOCKS_PER_SEC;
//     makePlatoons();
//     recomputeCost();
//     cpuAfterPlatooning = (clock() - start ) / (double) CLOCKS_PER_SEC;
// }

bool VRPSolution::isFeasible()
{
    for(int i = 0; i < getRoutesNum(); i++)
    {
        if(!sol_config[i]->isRouteFeas())
        {
            cout << "route " << i << " is infeasible." << endl;
            assert(false);
            return false;
        }
    }
    return true;
}

bool VRPSolution::operator<(ISolution& s)
{
	if((isFeasible() && s.isFeasible()) || (!isFeasible() && !s.isFeasible()))
	{
		return static_cast<long long>(1000*getPenalizedObjectiveValue()) < static_cast<long long>(1000*s.getPenalizedObjectiveValue(false));
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

void VRPSolution::resetExpectedTWAllRoutes()
{
    for(int i = 0; i < getRoutesNum(); i++)
    {
        sol_config[i]->calExpectedArrDepTW();
    }
}

void VRPSolution::makePlatoons()
{
    platoons_config.clear();
    if(sol_config.size() > 1)
    {
        PlatoonMaker* plmaker = new PlatoonMaker(*this, *nodeset);
        platoons_config = plmaker->getAllPlatoons();
        plmaker->setArrDepTimeAllRoutes();
        totalEnergySaving = plmaker->calSolTotalEnergySaving();
        // totalDistCostsBeforePlatooning = calTotalDist();
        // totalDistCostsAfterPlatooning = calTotalEnergyDist();
        // totalTripDurationAllRoutes = calTotalTripDuration();
        delete plmaker;
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
    // totalDistCostsAfterPlatooning = calTotalDist() - totalEnergySaving;
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
    // totalUnservedPasRequests = nonInsertedNodes[0].size();
    // totalUnservedFreRequests = nonInsertedNodes[1].size();
    totalUnservedRequests = nonInsertedNodes[0].size() + nonInsertedNodes[1].size();
    // for(int i = 0; i < nonInsertedNodes.size(); i++)
    // {
    //     if(nodeset->getNodeType(nonInsertedNodes[i]) == 0)
    //     {
    //         totalUnservedPasRequests += 1;
    //     }
    //     else if(nodeset->getNodeType(nonInsertedNodes[i]) == 1)
    //     {
    //         totalUnservedFreRequests += 1;
    //     }
    //     totalUnservedRequests += 1;
    // }
    return totalUnservedRequests;
    // return nonInsertedNodes.size();
}

int VRPSolution::calTotalUnservedPasRequests()
{
    totalUnservedPasRequests = nonInsertedNodes[0].size();
    // for(int i = 0; i < nonInsertedNodes.size(); i++)
    // {
    //     if(nodeset->getNodeType(nonInsertedNodes[i]) == 0)
    //     {
    //         totalUnservedPasRequests += 1;
    //     }
    // }
    return totalUnservedPasRequests;
}

int VRPSolution::calTotalUnservedFreRequests()
{
    totalUnservedFreRequests = nonInsertedNodes[1].size();
    // for(int i = 0; i < nonInsertedNodes.size(); i++)
    // {
    //     if(nodeset->getNodeType(nonInsertedNodes[i]) == 0)
    //     {
    //         totalUnservedFreRequests += 1;
    //     }
    // }
    return totalUnservedFreRequests;
}

double VRPSolution::calTotalObjectiveValue()
{
    return obj_w1*totalDistCostsAfterPlatooning + obj_w2*totalTripDurationAllRoutes + obj_w3*(totalUnservedPasRequests+fp*totalUnservedFreRequests);
    // return obj_w1*calTotalEnergyDist() + obj_w2*calTotalTripDuration() + obj_w3*calTotalUnservedRequests();
}

double VRPSolution::getPenalizedObjectiveValue(bool modified)
{
    if(!modified) return calTotalObjectiveValue();
    else return (obj_w1*totalDistCostsAfterPlatooning + obj_w2*totalTripDurationAllRoutes);
}

void VRPSolution::recomputeCost() //bool make_platoon
{
    totalDistCostsBeforePlatooning = 0;
    totalEnergySaving = 0;
    totalDistCostsAfterPlatooning = 0;
    totalTripDurationAllRoutes = 0;
    totalUnservedRequests = 0;
    totalUnservedPasRequests = 0;
    totalUnservedFreRequests = 0;
	totalObjValueAfterPlatooning = 0;
    // if(make_platoon) makePlatoons(); //! make_platoon = true: rebuild platoons again -> get the arrival time and departure time
    for(int r = 0; r < getRoutesNum(); r++)
    {
        totalDistCostsBeforePlatooning += sol_config[r]->getRouteDistance();
        totalTripDurationAllRoutes += sol_config[r]->getRouteDuration();
    }
    for(int p = 0; p < getPlatoonNum(); p++)
    {
        totalEnergySaving += platoons_config[p]->calEnergySaving();
    }
    totalDistCostsAfterPlatooning = totalDistCostsBeforePlatooning - totalEnergySaving;
    totalUnservedPasRequests = nonInsertedNodes[0].size();
    totalUnservedFreRequests = nonInsertedNodes[1].size();
    totalUnservedRequests = totalUnservedPasRequests + totalUnservedFreRequests;
    // for(int i = 0; i < nonInsertedNodes.size(); i++)
    // {
    //     if(nodeset->getNodeType(nonInsertedNodes[i]) == 0)
    //     {
    //         totalUnservedPasRequests += 1;
    //     }
    //     else if(nodeset->getNodeType(nonInsertedNodes[i] == 1))
    //     {
    //         totalUnservedFreRequests += 1;
    //     }
    //     totalUnservedRequests += 1;
    // }
    // totalUnservedRequests = nonInsertedNodes.size();
    totalObjValueAfterPlatooning = obj_w1*totalDistCostsAfterPlatooning + obj_w2*totalTripDurationAllRoutes + obj_w3*(totalUnservedPasRequests+fp*totalUnservedFreRequests);
}

void VRPSolution::updateSol(bool make_platoon)
{
    for(int i = 0; i < getRoutesNum(); i++)
    {
        sol_config[i]->calExpectedArrDepTW();
        // sol_config[i]->calRouteLoad();
        // sol_config[i]->calRouteMileage();
        // if(make_platoon) makePlatoons(); //! make_platoon = true: rebuild platoons again
        // sol_config[i]->calArrDepTime();
    }
}

void VRPSolution::postProcessSol()
{
    for(auto it = sol_config.begin(); it != sol_config.end();)
    {
        if((*it)->isEmpty())
        {
            delete *it;
            it = sol_config.erase(it);
        }
        else
        {
            it++;
        }
    }
}

int& VRPSolution::getTotalUnservedRequests(int type)
{
    if(type == 0)
    {
        return totalUnservedPasRequests;
    }
    else if(type == 1)
    {
        return totalUnservedFreRequests;
    }
    else
    {
        return totalUnservedRequests;
    }
}

ISolution* VRPSolution::getCopy()
{
	VRPSolution* newSol = new VRPSolution(*nodeset, *vehset);
    // newSol->nodeset = nodeset;
    // newSol->vehset = vehset;
    newSol->getNonInsertedNodes().clear();
    newSol->getNonInsertedNodes().resize(VTYPE);
    for(int i = 0; i < VTYPE; i++)
    {
        for(int j = 0; j < getNonInsertedNodes()[i].size(); j++)
        {
            newSol->getNonInsertedNodes()[i].push_back(getNonInsertedNodes()[i][j]);
        }
    }
    newSol->getNonUsedVehs().clear();
    newSol->getNonUsedVehs().resize(VTYPE);
    for(int i = 0; i < VTYPE; i++)
    {
        for(int v = 0; v <getNonUsedVehs()[i].size(); v++)
        {
            newSol->getNonUsedVehs()[i].push_back(getNonUsedVehs()[i][v]);
        }
    }
    newSol->destroyableArcConfig.clear(); //! destroyable
    newSol->destroyableArcPos.clear(); //! destroyable
    // for(int a = 0; a < destroyableArcConfig.size(); a++)
    // {
    //     newSol->destroyableArcConfig.push_back(getDestroyableArcConfig()[a]);
    //     newSol->destroyableArcPos.push_back(getDestroyableArcPos()[a]);
    // }
    newSol->destroyedArcPos.clear(); //! destroyed
    // for(int a = 0; a < destroyedArcPos.size(); a++)
    // {
    //     newSol->destroyedArcPos.push_back(getDestroyedArcsPos()[a]);
    // }
    newSol->getAllRoutes().clear();
    for(int r = 0; r < getRoutesNum(); r++)
    {
        ARoute* newRoute = new ARoute(*(sol_config[r]));
        newSol->getAllRoutes().push_back(newRoute);
    }
    newSol->getAllPlatoons().clear();
    for(int p = 0; p < getPlatoonNum(); p++)
    {
        APlatoon* newPlatoon = new APlatoon(*(platoons_config[p]));
        newSol->getAllPlatoons().push_back(newPlatoon);
    }
    newSol->totalDistCostsBeforePlatooning = totalDistCostsBeforePlatooning;
    newSol->totalEnergySaving = totalEnergySaving;
    newSol->totalDistCostsAfterPlatooning = totalDistCostsAfterPlatooning;
    newSol->totalTripDurationAllRoutes = totalTripDurationAllRoutes;
    newSol->totalUnservedRequests = totalUnservedRequests;
    newSol->totalUnservedPasRequests = totalUnservedPasRequests;
    newSol->totalUnservedFreRequests = totalUnservedFreRequests;
	newSol->totalObjValueAfterPlatooning = totalObjValueAfterPlatooning;
    // newSol->recomputeCost();
	return dynamic_cast<ISolution*>(newSol);
}

long long VRPSolution::getHash()
{
	//recomputeCost();
    double hash = 1;
    hash = 100 * totalDistCostsAfterPlatooning * totalTripDurationAllRoutes + 50 * totalUnservedPasRequests * totalUnservedFreRequests;
    // hash = 100 * calTotalEnergyDist() * calTotalTripDuration() + 50 * calTotalUnservedRequests();
	return static_cast<long long>(hash);
}

void VRPSolution::insertNode(int insert_pos, int insert_nodeid, int routeid)
{
    /* new version */
    ARoute* original_route = sol_config[routeid];
    assert(nodeset->getNodeType(insert_nodeid) == original_route->getVehType());
    int vehtype = original_route->getVehType();
    auto it = find(nonInsertedNodes[vehtype].begin(), nonInsertedNodes[vehtype].end(), insert_nodeid);
    if(it != nonInsertedNodes[vehtype].end()) //! make sure that the inserted node is in the nonInsertedNodes set
    {
        double insertcosts = evaluateInsertNode(insert_pos, insert_nodeid, routeid);
        if(insertcosts < INF) //! only insert when the insertion is feasible
        {
            sol_config[routeid]->setRouteByInsertNode(insert_pos, insert_nodeid);
            nonInsertedNodes[vehtype].erase(it);
            solCurOpt = InsertOneNode;
            //! compute the cost for the solution after the insertion
            totalDistCostsBeforePlatooning += insertcosts;
            // totalUnservedRequests -= 1;
            // if(nodeset->getNodeType(insert_nodeid) == 0)
            // {
            //     totalUnservedPasRequests -= 1;
            // }
            // else if(nodeset->getNodeType(insert_nodeid) == 1)
            // {
            //     totalUnservedFreRequests -= 1;
            // }
        }
        else
        {
            solCurOpt = NoChange;
        }
    }

    /* old version */
    // auto it = find(nonInsertedNodes.begin(), nonInsertedNodes.end(), insert_nodeid);
    // if(it != nonInsertedNodes.end()) //! make sure that the inserted node is in the nonInsertedNodes set
    // {
    //     sol_config[routeid]->setRouteByInsertNode(insert_pos, insert_nodeid);
    //     nonInsertedNodes.erase(it);
    //     totalUnservedRequests -= 1;
    // }
}

void VRPSolution::removeNode(int remove_pos, int routeid)
{
    /* new version */
    ARoute* original_route = sol_config[routeid];
    double removalcosts = evaluateRemoveNode(remove_pos, routeid);
    int remove_nodeid = sol_config[routeid]->getCompactRoute()[remove_pos];
    int nodetype = nodeset->getNodeType(remove_nodeid);
    if(removalcosts < INF) //only remove when the removal is feasible
    {
        sol_config[routeid]->setRouteByRemoveNode(remove_pos);
        nonInsertedNodes[nodetype].push_back(remove_nodeid);
        solCurOpt = RemoveOneNode;
        //! compute the removal costs for the solution after the removal
        totalDistCostsBeforePlatooning += removalcosts;
        // totalUnservedRequests += 1;
        // if(nodeset->getNodeType(remove_nodeid) == 0)
        // {
        //     totalUnservedPasRequests += 1;
        // }
        // else if(nodeset->getNodeType(remove_nodeid) == 1)
        // {
        //     totalUnservedFreRequests += 1;
        // }
    }
    else
    {
        solCurOpt = NoChange;
        // if(getOneRoute(routeid)->getCompactRoute().size() < 3)
        // {
        //     nonUsedVehs[nodetype].push_back(getOneRoute(routeid)->getVehID());
        //     getOneRoute(routeid)->setVehIdType(-1);
        // }
    }


    /* old version */
    // int remove_nodeid = sol_config[routeid]->getCompactRoute()[remove_pos];
    // sol_config[routeid]->setRouteByRemoveNode(remove_pos);
    // nonInsertedNodes.push_back(remove_nodeid);
    // totalUnservedRequests += 1;
}

void VRPSolution::repairPath(int modified_arcpos, int modified_pathid, int routeid)
{
    /* new version */
    ARoute* original_route = sol_config[routeid];
    double modifycosts = evaluateModifyPath(modified_arcpos, modified_pathid, routeid);
    if(modifycosts < INF) //only modify when the path modification is feasible
    {
        sol_config[routeid]->setRouteByModifyUsedPath(modified_arcpos, modified_pathid);
        solCurOpt = ModifyOnePath;
        //! compute the modification costs for the solution after the modification
        totalDistCostsBeforePlatooning += modifycosts;
    }
    else
    {
        solCurOpt = NoChange;
    }

    /* old version */
    // sol_config[routeid]->setRouteByModifyUsedPath(modified_arcpos, modified_pathid);
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


vector<tuple<double, int, int>> VRPSolution::calOrderedInsertCostsAllRoutes(int insert_node_id)
{
    vector<tuple<double, int, int>> min_cost_all_routes; //! <insert_cost, insert_pos, insert_route>
    for(int i = 0; i < getRoutesNum(); i++)
    {
        pair<double, int> cost_pos_route_i = getOneRoute(i)->calCheapestInsertionCosts(insert_node_id);
        min_cost_all_routes.push_back(make_tuple(cost_pos_route_i.first, cost_pos_route_i.second, i));
    }
    sort(min_cost_all_routes.begin(), min_cost_all_routes.end(), [&](auto A, auto B){return get<0>(A) < get<0>(B);});
    return min_cost_all_routes;
}

tuple<double, int, int> VRPSolution::calCheapestInsertCostsAllRoutes(int insert_node_id, int k_cheapest)
{
    return calOrderedInsertCostsAllRoutes(insert_node_id)[k_cheapest];
}

void VRPSolution::deleteOneRoute(int rid)
{
    //! remove nodes in the route
    vector<int> compact_route = getOneRoute(rid)->getCompactRoute();
    int rtype = getOneRoute(rid)->getVehType();
    for(int i = 1; i < compact_route.size()-1; i++)
    {
        nonInsertedNodes[rtype].push_back(compact_route[i]);
    }
    nonUsedVehs[rtype].push_back(getOneRoute(rid)->getVehID());
    delete sol_config[rid];
    sol_config.erase(sol_config.begin()+rid);
    //! remove platoons containing the route
    for(auto it_platoon = platoons_config.begin(); it_platoon != platoons_config.end();)
    {
        vector<int> common_routes = (*it_platoon)->getCommonRoutes();
        if(find(common_routes.begin(), common_routes.end(), rid) != common_routes.end())
        {
            (*it_platoon)->removeOneVeh(rid);
            if((*it_platoon)->calPLen() < 2)
            {
                delete (*it_platoon);
                it_platoon = platoons_config.erase(it_platoon);
            }
        }
        else
        {
            it_platoon++;
        }
    }
    // recomputeCost();
    solCurOpt = DeleteOneRoute;
}

/* old version */
// int VRPSolution::getMaxCusArrTime()
// {
//     vector<int> maxArrTimeVec;
//     for(int i = 0; i < getRoutesNum(); i++)
//     {
//         ARoute* route_i = getOneRoute(i);
//         int cusnum_i = route_i->getCompactRoute().size()-2;
//         maxArrTimeVec.push_back(route_i->getFinalArrTime()[cusnum_i]);
//     }
//     auto max_it = max_element(maxArrTimeVec.begin(), maxArrTimeVec.end());
//     return *max_it;
// }

/* old version */
// int VRPSolution::getMinCusArrTime()
// {
//     vector<int> minArrTimeVec;
//     for(int i = 0; i < getRoutesNum(); i++)
//     {
//         minArrTimeVec.push_back(getOneRoute(i)->getFinalArrTime()[1]);
//     }
//     auto min_it = min_element(minArrTimeVec.begin(), minArrTimeVec.end());
//     return *min_it;
// }

int VRPSolution::getNodeShowTimes(int nodeid)
{
    int count_sum = 0;
    vector<int> allnodes;
    for(int i = 0; i < getRoutesNum(); i++)
    {
        vector<int> extended_route_i = getOneRoute(i)->getExtendedRoute();
        count_sum += count(extended_route_i.begin(), extended_route_i.end(), nodeid);
    }
    return count_sum;
}

/*old version*/
vector<pair<int, int>> VRPSolution::getAllCusPos(int type)
{
    vector<pair<int, int>> all_cus_pos;
    if(type == 0 || type == 1)
    {
        for(int i = 0; i < getRoutesNum(); i++)
        {
            if(getOneRoute(i)->getVehType() == type)
            {
                for(int j = 1; j < sol_config[i]->getCompactRoute().size()-1; j++)
                {
                    all_cus_pos.push_back(make_pair(i, j)); //! <routeid, arcpos>
                }
            }
        }
    }
    else
    {
        for(int i = 0; i < getRoutesNum(); i++)
        {
            for(int j = 1; j < sol_config[i]->getCompactRoute().size()-1; j++)
            {
                all_cus_pos.push_back(make_pair(i, j)); //! <routeid, arcpos>
            }
        }
    }
    return all_cus_pos;
}

/*new version*/
// vector<vector<int>> VRPSolution::getAllCusCopy()
// {
//     vector<vector<int>> all_cus_pos(getRoutesNum());
//     for(int i = 0; i < getRoutesNum(); i++)
//     {
//         for(int j = 0; j < sol_config[i]->getCompactRoute().size(); j++)
//         {
//             all_cus_pos[i].push_back(sol_config[i]->getCompactRoute()[j]); //! a set of compact routes
//         }
//     }
//     return all_cus_pos;
// }