#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>  
#include <cassert>
#include <tuple>
#include "example/model/establish/ARoute.h"
#include "example/model/basic/Nodes.h"
#include "example/model/basic/Vehicles.h"
#include "example/model/basic/config.h"
#include "example/model/establish/TimeWindowUpdater.h"
#include "example/model/basic/ADijkstraSol.h"
// #define NDEBUG
using namespace std;

ARoute::ARoute(Nodes& nodeset, Vehicles& vehset)
{
    this->nodeset = &nodeset;
    this->vehset = &vehset;
    initEmptyRoute();
}

ARoute::ARoute(ARoute& route)
{
    this->nodeset = route.nodeset;
    this->vehset = route.vehset;
    this->vehid = route.vehid;
    this->vehtype = route.vehtype;
    this->compact_route = route.compact_route;
    this->used_paths_in_compact_route = route.used_paths_in_compact_route;
    this->extended_route = route.extended_route;
    this->node_labels = route.node_labels;
    TimeWindowUpdater twupdater(extended_route, node_labels, getRouteWaitTimeLimitPerNode(), getRouteWaitMaxLimit(), *nodeset);
    twupdater.calRouteExpectedTW();
    // this->expected_arrtw = twupdater.getRouteArrTW();
    // this->expected_deptw = twupdater.getRouteDepTW();
    this->expected_arrtw = route.expected_arrtw;
    this->expected_deptw = route.expected_deptw;
    // this->actual_arrtime.clear();
    // this->actual_deptime.clear();
    // this->actual_waittime.clear();
    // this->total_waittime = 0;
    this->actual_arrtime = route.actual_arrtime;
    this->actual_deptime = route.actual_deptime;
    this->actual_waittime = route.actual_waittime;
    this->total_waittime = route.total_waittime;
    this->route_load = route.route_load;
    this->route_mileage = route.route_mileage;

}

ARoute::~ARoute()
{
    // delete nodeset;
    // delete vehset;
}

void ARoute::setVehIdType(int input_vehid)
{
    vehid = input_vehid;
    vehtype = vehset->getVehType(input_vehid);
}

int ARoute::getRouteWaitTimeLimitPerNode()
{
    return vehset->getVehWaitTimePerNode(vehid);
}

int ARoute::getRouteWaitMaxLimit()
{
    return vehset->getVehWaitTimeMax(vehid);
}

int ARoute::convert_nodepos_extend_to_compact(int node_pos_extend)
{
    if(node_pos_extend == 0)
    {
        return 0;
    }
    else if (node_pos_extend == extended_route.size()-1)
    {
        return compact_route.size()-1;
    }
    else if(node_labels[node_pos_extend] == 1)
    {
        vector<int>::iterator compact_it = find(compact_route.begin(), compact_route.end(), extended_route[node_pos_extend]);
        if(compact_it != compact_route.end())
        {
            return (compact_it - compact_route.begin());
        }
    }
    else //route.node_labels[node_pos_extend] != 1 -> find the previous served node
    {
        for(auto iter = extended_route.begin() + node_pos_extend; iter != extended_route.end(); iter++)
        {
            if(node_labels[iter - extended_route.begin()] == 1)
            {
                vector<int>::iterator compact_it = find(compact_route.begin(), compact_route.end(), (*iter));
                if(compact_it != compact_route.end())
                {
                    return (compact_it - compact_route.begin() - 1);
                    break;
                }
            }
        }
    }
    return -1;
}

int ARoute::convert_nodepos_compact_to_extend(int node_pos_compact)
{
    if(node_pos_compact == 0)
    {
        return 0;
    }
    else if(node_pos_compact == compact_route.size()-1)
    {
        return extended_route.size()-1;
    }
    else
    {
        for(int i = 1; i < extended_route.size()-1; i++)
        {
            if(node_labels[i] == 1 && extended_route[i] == compact_route[node_pos_compact])
            {
                return i;
            }
        }
    }
    return -1;
}

vector<int> ARoute::findExtendPath(int node1, int node2, int used_path_in_between)
{
    vector<int> path = nodeset->getOnePath(node1, node2, used_path_in_between)->getPath();
    return path;
}

void ARoute::initEmptyRoute()
{
    vehid = -1;
    vehtype = -1;
    compact_route.resize(2, 0);
    used_paths_in_compact_route.resize(1, 0);
    extended_route.resize(2, 0);
    node_labels.resize(2, 0);
    route_load.resize(2, 0);
    route_mileage.resize(2, 0);
}

void ARoute::initUsedPaths()
{
    int path_size = compact_route.size()-1;
    if(path_size > 0)
    {
        vector<int> used_paths_vec(path_size, 0);
        used_paths_in_compact_route = used_paths_vec;        
    }
    else
    {
        used_paths_in_compact_route = {};
    }
}

void ARoute::buildExtendedRoute()
{
    extended_route.clear();
    for(int i = 0; i < compact_route.size()-1; i++)
    {
        int node1 = compact_route[i];
        int node2 = compact_route[i+1];
        int used_path_id = used_paths_in_compact_route[i];
        vector<int> ij_used_path = nodeset->getOnePath(node1, node2, used_path_id)->getPath();
        if(i == 0)
        {
            extended_route.insert(extended_route.end(), ij_used_path.begin(), ij_used_path.end());
        }
        else
        {
            extended_route.insert(extended_route.end(), ij_used_path.begin()+1, ij_used_path.end());
        }
    }
}

void ARoute::buildNodeLabels()
{
    node_labels.clear();
    node_labels.push_back(0);
    for(int i = 0; i < compact_route.size()-1; i++)
    {
        int node1 = compact_route[i];
        int node2 = compact_route[i+1];
        int used_path_id = used_paths_in_compact_route[i];
        int ij_used_path_size = nodeset->getOnePath(node1, node2, used_path_id)->getPath().size();
        vector<int> node_labels_vec(ij_used_path_size);
        node_labels_vec[ij_used_path_size-1] = 1;
        node_labels.insert(node_labels.end(), node_labels_vec.begin()+1, node_labels_vec.end());
    }
    node_labels[node_labels.size()-1] = 0;
}

void ARoute::calExpectedArrDepTW()
{
    TimeWindowUpdater twupdater(extended_route, node_labels, getRouteWaitTimeLimitPerNode(), getRouteWaitMaxLimit(), *nodeset);
    twupdater.calRouteExpectedTW();
    expected_arrtw = twupdater.getRouteArrTW();
    expected_deptw = twupdater.getRouteDepTW();
    // this->actual_arrtime.clear();
    // this->actual_deptime.clear();
    // this->actual_waittime.clear();
    // this->total_waittime = 0;
}

void ARoute::calArrDepTime()
{
    TimeWindowUpdater twupdater(extended_route, node_labels, getRouteWaitTimeLimitPerNode(), getRouteWaitMaxLimit(), *nodeset);
    twupdater.setRouteArrTW(expected_arrtw);
    twupdater.setRouteDepTW(expected_deptw);
    twupdater.setFinalArrDepTime();
    this->actual_arrtime = twupdater.getRouteArrTime();
    this->actual_deptime = twupdater.getRouteDepTime();
    this->actual_waittime = twupdater.getActualWaitTimeAllNodes();
    this->total_waittime = twupdater.calTotalWaitTime();
}

void ARoute::calRouteLoad()
{
    route_load.resize(compact_route.size());
    for(int i = 1; i < compact_route.size()-1; i++)
    {
        int node_id = compact_route[i];
        route_load[0] += (nodeset->getNodeDemand(node_id) < 0) ? -nodeset->getNodeDemand(node_id) : 0;
    }
    for(int k = 1; k < compact_route.size(); k++)
    {
        route_load[k] = route_load[k-1] + nodeset->getNodeDemand(compact_route[k]);
    }
}

void ARoute::calRouteMileage()
{
    route_mileage.resize(compact_route.size());
    route_mileage[0] = 0;
    for(int i = 0; i < compact_route.size()-1; i++)
    {
        double dist_from_i = nodeset->getOnePath(compact_route[i], compact_route[i+1], used_paths_in_compact_route[i])->getDist();
        route_mileage[i+1] += route_mileage[i] + dist_from_i;
    }
}

vector<int> ARoute::insertNodeIntoCompactRoute(int insert_pos, int insert_nodeid)
{
    vector<int> compact_route_copy = compact_route;
    compact_route_copy.insert(compact_route_copy.begin()+insert_pos, insert_nodeid);
    return compact_route_copy;
}

vector<int> ARoute::setUsedPathsByInsertNode(int insert_pos, int insert_nodeid)
{
    vector<int> used_paths_copy = used_paths_in_compact_route;
    used_paths_copy[insert_pos-1] = 0;
    used_paths_copy.insert(used_paths_copy.begin()+insert_pos, 0);
    return used_paths_copy;
}

vector<int> ARoute::setExtendRouteByInsertNode(int insert_pos, int insert_nodeid)
{
    vector<int> extended_route_copy;
    int segnode1 = convert_nodepos_compact_to_extend(insert_pos-1);
    int segnode2 = convert_nodepos_compact_to_extend(insert_pos);
    extended_route_copy.insert(extended_route_copy.end(), extended_route.begin(), extended_route.begin()+segnode1+1);
    vector<int> extendpath1 = findExtendPath(compact_route[insert_pos-1], insert_nodeid, 0);
    vector<int> extendpath2 = findExtendPath(insert_nodeid, compact_route[insert_pos], 0);
    extended_route_copy.insert(extended_route_copy.end(), extendpath1.begin()+1, extendpath1.end());
    extended_route_copy.insert(extended_route_copy.end(), extendpath2.begin()+1, extendpath2.end());
    extended_route_copy.insert(extended_route_copy.end(), extended_route.begin()+segnode2+1, extended_route.end());
    return extended_route_copy;
}

vector<int> ARoute::setNodeLabelsByInsertNode(int insert_pos, int insert_nodeid)
{
    vector<int> node_labels_copy;
    int segnode1 = convert_nodepos_compact_to_extend(insert_pos-1);
    int segnode2 = convert_nodepos_compact_to_extend(insert_pos);
    node_labels_copy.insert(node_labels_copy.end(), node_labels.begin(), node_labels.begin()+segnode1+1);
    int extendpath1_size = findExtendPath(compact_route[insert_pos-1], insert_nodeid, 0).size();
    int extendpath2_size = findExtendPath(insert_nodeid, compact_route[insert_pos], 0).size();
    vector<int> nodelabels_vec1(extendpath1_size);
    vector<int> nodelabels_vec2(extendpath2_size);
    nodelabels_vec1[extendpath1_size-1] = 1;
    nodelabels_vec2[extendpath2_size-1] = 1;
    node_labels_copy.insert(node_labels_copy.end(), nodelabels_vec1.begin()+1, nodelabels_vec1.end());
    node_labels_copy.insert(node_labels_copy.end(), nodelabels_vec2.begin()+1, nodelabels_vec2.end());
    node_labels_copy.insert(node_labels_copy.end(), node_labels.begin()+segnode2+1, node_labels.end());
    node_labels_copy[node_labels_copy.size()-1] = 0; //in case the last node is the ending depot
    return node_labels_copy;
}

vector<int> ARoute::setRouteLoadByInsertNode(int insert_pos, int insert_nodeid)
{
    vector<int> route_load_copy = route_load;
    route_load_copy.insert(route_load_copy.begin()+insert_pos, route_load[insert_pos-1]);
    int insert_nodedmd = nodeset->getNodeDemand(insert_nodeid);
    if(insert_nodedmd < 0) //delivery node
    {
        route_load_copy[0] = route_load[0] - insert_nodedmd;
        for(int i = 1; i < insert_pos; i++)
        {
            route_load_copy[i] = route_load_copy[i-1] + nodeset->getNodeDemand(route_load_copy[i]);
        }
    }
    else //pickup node
    {
        route_load_copy[insert_pos] += insert_nodedmd;
        for(int i = insert_pos+1; i < route_load_copy.size(); i++)
        {
            route_load_copy[i] = route_load_copy[i-1] + nodeset->getNodeDemand(route_load_copy[i]);
        }
    }
    return route_load_copy;
}

vector<double> ARoute::setRouteMileageByInsertNode(int insert_pos, int insert_nodeid)
{
    vector<double> route_mileage_copy = route_mileage;
    route_mileage_copy.insert(route_mileage_copy.begin()+insert_pos, route_mileage[insert_pos-1]);
    route_mileage_copy[insert_pos] += nodeset->getOnePath(compact_route[insert_pos-1], insert_nodeid, 0)->getDist();
    double increDist = calInsertionCosts(insert_pos, insert_nodeid);
    for(int i = insert_pos+1; i < route_mileage_copy.size(); i++)
    {
        route_mileage_copy[i] += increDist;
    }
    return route_mileage_copy;
}

vector<int> ARoute::removeNodeFromCompactRoute(int remove_pos)
{
    vector<int> compact_route_copy = compact_route;
    compact_route_copy.erase(compact_route_copy.begin() + remove_pos);
    return compact_route_copy;
}

vector<int> ARoute::setUsedPathsByRemoveNode(int remove_pos)
{
    vector<int> used_paths_copy = used_paths_in_compact_route;
    used_paths_copy.erase(used_paths_copy.begin()+remove_pos);
    used_paths_copy[remove_pos-1] = 0;
    return used_paths_copy;
}

vector<int> ARoute::setExtendRouteByRemoveNode(int remove_pos)
{
    vector<int> extended_route_copy;
    vector<int> extendpath = findExtendPath(compact_route[remove_pos-1], compact_route[remove_pos+1], 0);
    if(!extendpath.empty())
    {
        int segnode1 = convert_nodepos_compact_to_extend(remove_pos-1);
        int segnode2 = convert_nodepos_compact_to_extend(remove_pos+1);
        extended_route_copy.insert(extended_route_copy.end(), extended_route.begin(), extended_route.begin()+segnode1+1);
        extended_route_copy.insert(extended_route_copy.end(), extendpath.begin()+1, extendpath.end());
        extended_route_copy.insert(extended_route_copy.end(), extended_route.begin()+segnode2+1, extended_route.end());
    }
    else
    {
        extended_route_copy = {0,0};
    }
    return extended_route_copy;
}

vector<int> ARoute::setNodeLabelsByRemoveNode(int remove_pos)
{
    vector<int> node_labels_copy;
    int extendpath_size = findExtendPath(compact_route[remove_pos-1], compact_route[remove_pos+1], 0).size();
    if(extendpath_size > 1) // >0
    {
        int segnode1 = convert_nodepos_compact_to_extend(remove_pos-1);
        int segnode2 = convert_nodepos_compact_to_extend(remove_pos+1);
        node_labels_copy.insert(node_labels_copy.end(), node_labels.begin(), node_labels.begin()+segnode1+1);
        vector<int> nodelabels_vec(extendpath_size);
        nodelabels_vec[extendpath_size-1] = 1;
        node_labels_copy.insert(node_labels_copy.end(), nodelabels_vec.begin()+1, nodelabels_vec.end());
        node_labels_copy.insert(node_labels_copy.end(), node_labels.begin()+segnode2+1, node_labels.end());
        node_labels_copy[node_labels_copy.size()-1] = 0; //in case the last node is the ending depot
    }
    else
    {
        node_labels_copy = {1,1};
    }
    return node_labels_copy;
}

vector<int> ARoute::setRouteLoadByRemoveNode(int remove_pos)
{
    vector<int> route_load_copy = route_load;
    route_load_copy.erase(route_load_copy.begin()+remove_pos);
    int remove_nodedmd = nodeset->getNodeDemand(compact_route[remove_pos]);
    if(remove_nodedmd < 0) //delivery node
    {
        route_load_copy[0] = route_load[0] + remove_nodedmd;
        for(int i = 1; i < remove_pos; i++)
        {
            route_load_copy[i] = route_load_copy[i-1] + nodeset->getNodeDemand(route_load_copy[i]);
        }
    }
    else //pickup node
    {
        route_load_copy[remove_pos] -= remove_nodedmd;
        for(int i = remove_pos+1; i < route_load_copy.size(); i++)
        {
            route_load_copy[i] = route_load_copy[i-1] + nodeset->getNodeDemand(route_load_copy[i]);
        }
    }
    return route_load_copy;
}

vector<double> ARoute::setRouteMileageByRemoveNode(int remove_pos)
{
    vector<double> route_mileage_copy = route_mileage;
    route_mileage_copy.erase(route_mileage_copy.begin()+remove_pos);
    double decreDist = calRemovalCosts(remove_pos);
    for(int i = remove_pos; i < route_mileage_copy.size(); i++)
    {
        route_mileage_copy[i] -= decreDist;
    }
    return route_mileage_copy;
}

vector<int> ARoute::modifyUsedPath(int modified_arcpos, int used_path_id)
{
    vector<int> used_paths_copy = used_paths_in_compact_route;
    used_paths_copy[modified_arcpos] = used_path_id;
    return used_paths_copy;
}

vector<int> ARoute::setExtendRouteByModifyUsedPath(int modified_arcpos, int used_path_id)
{
    vector<int> extended_route_copy;
    vector<int> extendpath = findExtendPath(compact_route[modified_arcpos], compact_route[modified_arcpos+1], used_path_id);
    if(!extendpath.empty())
    {
        int segnode1 = convert_nodepos_compact_to_extend(modified_arcpos);
        int segnode2 = convert_nodepos_compact_to_extend(modified_arcpos+1);
        extended_route_copy.insert(extended_route_copy.end(), extended_route.begin(), extended_route.begin()+segnode1+1);
        extended_route_copy.insert(extended_route_copy.end(), extendpath.begin()+1, extendpath.end());
        extended_route_copy.insert(extended_route_copy.end(), extended_route.begin()+segnode2+1, extended_route.end());
    }
    else
    {
        extended_route_copy = {0,0};
    }
    return extended_route_copy;
}

vector<int> ARoute::setNodeLabelsByModifyUsedPath(int modified_arcpos, int used_path_id)
{
    vector<int> node_labels_copy;
    int extendpath_size = findExtendPath(compact_route[modified_arcpos], compact_route[modified_arcpos+1], used_path_id).size();

    if(extendpath_size > 1)
    {
        int segnode1 = convert_nodepos_compact_to_extend(modified_arcpos);
        int segnode2 = convert_nodepos_compact_to_extend(modified_arcpos+1);
        node_labels_copy.insert(node_labels_copy.end(), node_labels.begin(), node_labels.begin()+segnode1+1);
        vector<int> nodelabels_vec(extendpath_size);
        nodelabels_vec[extendpath_size-1] = 1;
        node_labels_copy.insert(node_labels_copy.end(), nodelabels_vec.begin()+1, nodelabels_vec.end());
        node_labels_copy.insert(node_labels_copy.end(), node_labels.begin()+segnode2+1, node_labels.end());
        node_labels_copy[node_labels_copy.size()-1] = 0; //in case the last node is the ending depot
    }
    else
    {
        node_labels_copy = {1,1};
    }
    return node_labels_copy;
}

vector<double> ARoute::setRouteMileageByModifyUsedPath(int modified_arcpos, int used_path_id)
{
    vector<double> route_mileage_copy = route_mileage;
    double increDist = nodeset->getOnePath(compact_route[modified_arcpos], compact_route[modified_arcpos+1], used_path_id)->getDist()
                    - nodeset->getOnePath(compact_route[modified_arcpos], compact_route[modified_arcpos+1], used_paths_in_compact_route[modified_arcpos])->getDist();
    for(int i = modified_arcpos+1; i < route_mileage_copy.size(); i++)
    {
        route_mileage_copy[i] += increDist;
    }
    return route_mileage_copy;
}

bool ARoute::isEmpty()
{
    vector<int> initroute = {0,0};
    if(vehid == -1) return true;
    return (compact_route == initroute);
}

void ARoute::resetRoute(int input_vehid, vector<int> input_compact_route)
{
    vehid = input_vehid;
    vehtype = vehset->getVehType(vehid);
    compact_route = input_compact_route;
    initUsedPaths();
    buildExtendedRoute();
    buildNodeLabels();
    calRouteLoad();
    calRouteMileage();
    TimeWindowUpdater twupdater(extended_route, node_labels, getRouteWaitTimeLimitPerNode(), getRouteWaitMaxLimit(), *nodeset);
    twupdater.calRouteExpectedTW();
    expected_arrtw = twupdater.getRouteArrTW();
    expected_deptw = twupdater.getRouteDepTW();
}

void ARoute::copyFromOtherRoute(ARoute& input_route)
{
    this->vehid = input_route.vehid;
    this->vehtype = input_route.vehtype;
    this->compact_route = input_route.compact_route;
    this->used_paths_in_compact_route = input_route.used_paths_in_compact_route;
    this->extended_route = input_route.extended_route;
    this->node_labels = input_route.node_labels;
    this->expected_arrtw = input_route.expected_arrtw;
    this->expected_deptw = input_route.expected_deptw;
    this->actual_arrtime = input_route.actual_arrtime;
    this->actual_deptime = input_route.actual_deptime;
    this->route_load = input_route.route_load;
    this->route_mileage = input_route.route_mileage;
}

void ARoute::evaluateRouteByInsertNode(int insert_pos, int insert_nodeid)
{
    assert(vehid != -1);
    if(nodeset->getNodeType(insert_nodeid) == vehtype || nodeset->getNodeType(insert_nodeid) == 2)
    {
        //veh_id and veh_type unchanged
        vector<int> compact_route1 = insertNodeIntoCompactRoute(insert_pos, insert_nodeid);
        vector<int> extended_route1 = setExtendRouteByInsertNode(insert_pos, insert_nodeid);
        vector<int> node_labels1 = setNodeLabelsByInsertNode(insert_pos, insert_nodeid);
        vector<int> route_load1 = setRouteLoadByInsertNode(insert_pos, insert_nodeid);
        vector<double> route_mileage1 = setRouteMileageByInsertNode(insert_pos, insert_nodeid);
        
        TimeWindowUpdater twupdater(extended_route1, node_labels1, getRouteWaitTimeLimitPerNode(), getRouteWaitMaxLimit(), *nodeset);
        twupdater.calRouteExpectedTW();
        
        bool TimeFeas = twupdater.isRouteTimeFeas();
        bool LoadFeas = isLoadFeas(route_load1);
        bool DistFeas = isDistFeas(route_mileage1[route_mileage1.size()-1]);
        bool LinkFeas = isLinkFeas(insert_pos-1, compact_route1) && isLinkFeas(insert_pos, compact_route1);

        if(TimeFeas && LoadFeas && DistFeas&& LinkFeas)
        {
            nodeInsertionCosts = route_mileage1[route_mileage1.size()-1] - this->route_mileage[route_mileage.size()-1];
        }
        else
        {
            nodeInsertionCosts = INF;
        }   
    }
    else
    {
        nodeInsertionCosts = INF;
    }

}

void ARoute::setRouteByInsertNode(int insert_pos, int insert_nodeid)
{
    assert(vehid != -1);
    //veh_id and veh_type unchanged
    vector<int> compact_route1 = insertNodeIntoCompactRoute(insert_pos, insert_nodeid);
    vector<int> extended_route1 = setExtendRouteByInsertNode(insert_pos, insert_nodeid);
    vector<int> node_labels1 = setNodeLabelsByInsertNode(insert_pos, insert_nodeid);
    vector<int> route_load1 = setRouteLoadByInsertNode(insert_pos, insert_nodeid);
    vector<double> route_mileage1 = setRouteMileageByInsertNode(insert_pos, insert_nodeid);
    this->compact_route = compact_route1;
    this->extended_route = extended_route1;
    this->node_labels = node_labels1;
    this->route_load = route_load1;
    this->route_mileage = route_mileage1;
    this->used_paths_in_compact_route = setUsedPathsByInsertNode(insert_pos, insert_nodeid);
    // this->extended_route = setExtendRouteByInsertNode(insert_pos, insert_nodeid); //! modify the extend route with the original compact route
    // this->node_labels = setNodeLabelsByInsertNode(insert_pos, insert_nodeid); //! used the original extend route and compact route
    // this->compact_route = insertNodeIntoCompactRoute(insert_pos, insert_nodeid); //! modify the compact route
    // this->route_load = setRouteLoadByInsertNode(insert_pos, insert_nodeid);
    // this->route_mileage = setRouteMileageByInsertNode(insert_pos, insert_nodeid);    
    TimeWindowUpdater twupdater(extended_route, node_labels, getRouteWaitTimeLimitPerNode(), getRouteWaitMaxLimit(), *nodeset);
    twupdater.calRouteExpectedTW();
    this->expected_arrtw = twupdater.getRouteArrTW();
    this->expected_deptw = twupdater.getRouteDepTW();
    this->actual_arrtime.clear();
    this->actual_deptime.clear();
}

void ARoute::evaluateRouteByRemoveNode(int remove_pos)
{
    assert(vehid != -1);
    //veh_id and veh_type unchanged
    vector<int> compact_route1 = removeNodeFromCompactRoute(remove_pos);
    vector<int> extended_route1 = setExtendRouteByRemoveNode(remove_pos);
    vector<int> node_labels1 = setNodeLabelsByRemoveNode(remove_pos);
    vector<int> route_load1 = setRouteLoadByRemoveNode(remove_pos);
    vector<double> route_mileage1 = setRouteMileageByRemoveNode(remove_pos);

    TimeWindowUpdater twupdater(extended_route1, node_labels1, getRouteWaitTimeLimitPerNode(), getRouteWaitMaxLimit(), *nodeset);
    twupdater.calRouteExpectedTW();
    bool TimeFeas = twupdater.isRouteTimeFeas();
    bool LoadFeas = true; // isLoadFeas(route_load1);
    bool DistFeas = true; // isDistFeas(route_mileage1[route_mileage1.size()-1]);
    bool LinkFeas = isLinkFeas(remove_pos-1, compact_route1);

    if(TimeFeas && LoadFeas && DistFeas&& LinkFeas)
    {
        nodeRemovalCosts = route_mileage1[route_mileage1.size()-1] - this->route_mileage[route_mileage.size()-1];
    } //the nodeRemovalCosts is negative: after removal costs - before removal costs
    else
    {
        nodeRemovalCosts = INF;
    }
}

void ARoute::setRouteByRemoveNode(int remove_pos)
{
    assert(vehid != -1);
    //veh_id and veh_type unchanged
    vector<int> compact_route1 = removeNodeFromCompactRoute(remove_pos);
    vector<int> extended_route1 = setExtendRouteByRemoveNode(remove_pos);
    vector<int> node_labels1 = setNodeLabelsByRemoveNode(remove_pos);
    vector<int> route_load1 = setRouteLoadByRemoveNode(remove_pos);
    vector<double> route_mileage1 = setRouteMileageByRemoveNode(remove_pos);
    this->compact_route = compact_route1;
    this->extended_route = extended_route1;
    this->node_labels = node_labels1;
    this->route_load = route_load1;
    this->route_mileage = route_mileage1;
    this->used_paths_in_compact_route = setUsedPathsByRemoveNode(remove_pos);

    // this->compact_route = removeNodeFromCompactRoute(remove_pos);
    // this->node_labels = setNodeLabelsByRemoveNode(remove_pos);
    // this->extended_route = setExtendRouteByRemoveNode(remove_pos);
    // this->route_load = setRouteLoadByRemoveNode(remove_pos);
    // this->route_mileage = setRouteMileageByRemoveNode(remove_pos);
    TimeWindowUpdater twupdater(extended_route, node_labels, getRouteWaitTimeLimitPerNode(), getRouteWaitMaxLimit(), *nodeset);
    twupdater.calRouteExpectedTW();
    this->expected_arrtw = twupdater.getRouteArrTW();
    this->expected_deptw = twupdater.getRouteDepTW();
    this->actual_arrtime.clear();
    this->actual_deptime.clear();
}

void ARoute::evaluateRouteByModifyUsedPath(int modified_arcpos, int used_path_id)
{
    assert(vehid != -1);
    //veh_id and veh_type unchanged
    vector<int> extended_route1 = setExtendRouteByModifyUsedPath(modified_arcpos, used_path_id);
    vector<int> node_labels1 = setNodeLabelsByModifyUsedPath(modified_arcpos, used_path_id);
    vector<double> route_mileage1 = setRouteMileageByModifyUsedPath(modified_arcpos, used_path_id);

    TimeWindowUpdater twupdater(extended_route1, node_labels1, getRouteWaitTimeLimitPerNode(), getRouteWaitMaxLimit(), *nodeset);
    twupdater.calRouteExpectedTW();
    bool TimeFeas = twupdater.isRouteTimeFeas();
    bool LoadFeas = true; // isLoadFeas(route_load);
    bool DistFeas = isDistFeas(route_mileage1[route_mileage1.size()-1]);
    bool LinkFeas = isLinkFeas(modified_arcpos, compact_route);

    if(TimeFeas && LoadFeas && DistFeas&& LinkFeas)
    {
        pathModificationCosts = route_mileage1[route_mileage1.size()-1] - this->route_mileage[route_mileage.size()-1];
    } //the pathModificationCosts may be positive or negative
    else
    {
        pathModificationCosts = INF;
    }
}

void ARoute::setRouteByModifyUsedPath(int modified_arcpos, int used_path_id)
{
    assert(vehid != -1);
    //veh_id and veh_type unchanged
    vector<int> extended_route1 = setExtendRouteByModifyUsedPath(modified_arcpos, used_path_id);
    vector<int> node_labels1 = setNodeLabelsByModifyUsedPath(modified_arcpos, used_path_id);
    vector<double> route_mileage1 = setRouteMileageByModifyUsedPath(modified_arcpos, used_path_id);
    this->node_labels = node_labels1;
    this->extended_route = extended_route1;
    this->route_mileage = route_mileage1;
    this->used_paths_in_compact_route = modifyUsedPath(modified_arcpos, used_path_id);

    // this->node_labels = setNodeLabelsByModifyUsedPath(modified_arcpos, used_path_id); //! used the original extend route and compact route
    // this->extended_route = setExtendRouteByModifyUsedPath(modified_arcpos, used_path_id); //! modify the extend route with the original compact route
    // this->route_mileage = setRouteMileageByModifyUsedPath(modified_arcpos, used_path_id);
    TimeWindowUpdater twupdater(extended_route, node_labels, getRouteWaitTimeLimitPerNode(), getRouteWaitMaxLimit(), *nodeset);
    twupdater.calRouteExpectedTW();
    this->expected_arrtw = twupdater.getRouteArrTW();
    this->expected_deptw = twupdater.getRouteDepTW();
    this->actual_arrtime.clear();
    this->actual_deptime.clear();
}

bool ARoute::isTimeFeas()
{
    TimeWindowUpdater twupdater(extended_route, node_labels, getRouteWaitTimeLimitPerNode(), getRouteWaitMaxLimit(), *nodeset);
    // twupdater.calRouteExpectedTW();
    // TimeWindowUpdater twupdater;
    twupdater.setRouteArrTW(expected_arrtw);
    twupdater.setRouteDepTW(expected_deptw);
    return twupdater.isRouteTimeFeas();
}

bool ARoute::isLoadFeas(vector<int> input_load_vec)
{
    auto isNodeLoadFeas = [&](int node_load) -> bool {return node_load >= 0 && node_load <= vehset->getVehCap(vehid);};
    for(int i = 0; i < route_load.size(); i++)
    {
        if(!isNodeLoadFeas(route_load[i]))
        {
            return false;
        }
    }
    return true;
}

bool ARoute::isDistFeas(int total_dist)
{
    return total_dist <= vehset->getVehRange();
}

bool ARoute::isLinkFeas(int arcpos, vector<int> compactRoute)
{
    bool linkfeas1 = !nodeset->getOnePath(compactRoute[arcpos], compactRoute[arcpos+1], 0)->getPath().empty();
    bool linkfeas2 = nodeset->getOnePath(compactRoute[arcpos], compactRoute[arcpos+1], 0)->getDist() < INF;
    // bool linkfeas3 = nodeset->getOnePath(compactRoute[arcpos], compactRoute[arcpos+1], 0)->getDist() > 0;
    return linkfeas1 && linkfeas2; //! if == INF -> contains depots in the middle of the path
    // return !nodeset->getAvailPathSet(compactRoute[arcpos], compactRoute[arcpos+1]).empty(); 
}

bool ARoute::isRouteFeas()
{
    bool timefeas = isTimeFeas();
    bool loadfeas = isLoadFeas(route_load);
    bool distfeas = isDistFeas(route_mileage[route_mileage.size()-1]);
    bool linkfeas = true;
    for(int i = 0; i < used_paths_in_compact_route.size(); i++)
    {
        linkfeas = isLinkFeas(i, compact_route) && linkfeas;
    }
    return timefeas && loadfeas && distfeas && linkfeas;
}

double ARoute::calInsertionCosts(int insert_pos, int insert_nodeid)
{
    double cost;
    int orig_usedpath_pos = used_paths_in_compact_route[insert_pos-1];
    if(insert_pos < compact_route.size() && insert_pos > 0) //insert the node in the middle of the route
    {
        double add_cost1 = nodeset->getOnePath(compact_route[insert_pos-1], insert_nodeid, 0)->getDist();
        double add_cost2 = nodeset->getOnePath(insert_nodeid, compact_route[insert_pos], 0)->getDist();
        double minus_cost = nodeset->getOnePath(compact_route[insert_pos-1], compact_route[insert_pos], orig_usedpath_pos)->getDist();
        cost = add_cost1 + add_cost2 - minus_cost;
    }
    else //insert the node at the start or the end of the route -> cerr
    {
        cerr << "Wrong insertion position!";
        cost = INF;
    }
    return cost;
}

pair<double, int> ARoute::calCheapestInsertionCosts(int insert_nodeid) //! <insert_cost, insert_pos>
{
    pair<double, int> cost_pos;  // a pair is created to store the cheapest insertion cost and the corresponding position of the node in the route
    vector<double> all_insertion_costs;
    for(int i = 1; i < compact_route.size(); i++) //the node cannot be inserted before the starting depot and after the ending depot
    {
        evaluateRouteByInsertNode(i, insert_nodeid);
        double insertioncosts = getRouteOperatorCosts(InsertOneNode);
        all_insertion_costs.push_back(insertioncosts);
    }
    auto min_pos = min_element(all_insertion_costs.begin(), all_insertion_costs.end());
    double min_cost = *min_pos;
    nodeInsertionCosts = min_cost; //! update the nodeInsertionCosts to be the cheapest insertion costs of the given node
    int insert_pos = min_pos - all_insertion_costs.begin() + 1; //1 is for the starting depot
    cost_pos = make_pair(min_cost, insert_pos);
    return cost_pos;
}

double ARoute::calRemovalCosts(int removal_pos)
{
    double cost;
    if(removal_pos > 0 && removal_pos < compact_route.size()-1) //remove the node in the middle of the route
    {
        double add_cost1 = nodeset->getOnePath(compact_route[removal_pos-1], compact_route[removal_pos], used_paths_in_compact_route[removal_pos-1])->getDist();
        double add_cost2 = nodeset->getOnePath(compact_route[removal_pos], compact_route[removal_pos+1], used_paths_in_compact_route[removal_pos])->getDist();
        double minus_cost = nodeset->getOnePath(compact_route[removal_pos-1], compact_route[removal_pos+1], 0)->getDist(); //initially, link by the shortest path
        cost = add_cost1 + add_cost2 - minus_cost;
    }
    else //pos == 0 or pos == route.size()-1
    {
        cerr << "Error! You are removing the depot.";
        cost = INF;
    }
    return cost;
}

double ARoute::getRouteOperatorCosts(RouteOperationKind routeOperator)
{
    switch (routeOperator)
    {
        case InsertOneNode: return nodeInsertionCosts;
        case RemoveOneNode: return nodeRemovalCosts;
        case ModifyOnePath: return pathModificationCosts;
        case Initialization: return 0;
        case NoChange: return 0;
    }
    return 0;
}