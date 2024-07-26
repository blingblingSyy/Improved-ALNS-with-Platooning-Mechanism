#include <iostream>  
#include <vector>  
#include <set>
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include <tuple>
#include "HeuristicFramework.h"
// #include "TimeWindowUpdater.h"
#include "PlatoonMaker.h"
#include "config.h"
#include "utility.h"
using namespace std;

/*HeuristicFramework: establish a framework for building solutions*/
HeuristicFramework::HeuristicFramework(Nodes &nodes, Vehicles &vehs) //, Nodes &nodes
{
    nodeset = nodes;
    vehset = vehs;
}

//for nodes and routes operations

int HeuristicFramework::tranform_vehid_to_routeid(vector<Route> sol_config, int input_vehid)
{
    auto iter = find_if(sol_config.begin(), sol_config.end(), [&](Route x){return x.veh_id == input_vehid;});
    if(iter != sol_config.end())
    {
        return (iter - sol_config.begin());
    }
    else
    {
        return -1;
    }
}

int HeuristicFramework::tranform_nodepos_extend_to_compact(Route &route, int node_pos_extend)
{
    if(route.node_labels[node_pos_extend] == 1)
    {
        vector<int>::iterator compact_it = find(route.compact_route.begin(), route.compact_route.end(), route.extended_route[node_pos_extend]);
        if(compact_it != route.compact_route.end())
        {
            return (compact_it - route.compact_route.begin());
        }
    }
    else //route.node_labels[node_pos_extend] == 1 -> find the previous served node
    {
        for(auto iter = route.extended_route.begin() + node_pos_extend; iter != route.extended_route.end(); iter++)
        {
            if(route.node_labels[iter - route.extended_route.begin()] == 1)
            {
                vector<int>::iterator compact_it = find(route.compact_route.begin(), route.compact_route.end(), (*iter));
                if(compact_it != route.compact_route.end())
                {
                    return (compact_it - route.compact_route.begin() - 1);
                    break;
                }
            }
        }
    }
    return -1;
}

/*building a route*/
//for routes
Route HeuristicFramework::build_new_route_struct(int &input_veh_id, int &input_veh_type, vector<int> &input_compact_route)
{
    Route new_route;
    new_route.veh_id = input_veh_id;
    new_route.veh_type = input_veh_type;
    new_route.compact_route = input_compact_route;
    init_used_paths_in_compact_route(new_route);
    set_route_loads(new_route);
    set_route_mileage(new_route);
    return new_route;
}

//initiate the used paths in the initial feasible solution
void HeuristicFramework::init_used_paths_in_compact_route(Route &route)
{
    int path_size = route.compact_route.size() - 1;
    if(path_size > 0)
    {
        vector<int> used_paths_vec(path_size, 0);
        route.used_paths_in_compact_route = used_paths_vec;        
    }
}

//set a used path in a given node position in the compact route
void HeuristicFramework::set_used_paths(Route &route, int compact_nodepos, int used_path_id)
{
    route.used_paths_in_compact_route[compact_nodepos-1] = used_path_id;
}

void HeuristicFramework::set_extended_route(Route &route)
{
    route.extended_route.clear();
    vector<vector<int>> extended_route_set;
    for(int i = 0; i < route.compact_route.size()-1; i++)
    {
        int node1 = route.compact_route[i];
        int node2 = route.compact_route[i+1];
        int used_path_id = route.used_paths_in_compact_route[i];
        vector<int> ij_used_path = nodeset.avail_path_set[node1][node2][used_path_id].KSP_Path;
        extended_route_set.push_back(ij_used_path);
    }
    route.extended_route.insert(route.extended_route.end(), extended_route_set[0].begin(), extended_route_set[0].end());
    for(int s = 1; s < extended_route_set.size(); s++)
    {
        route.extended_route.insert(route.extended_route.end(), extended_route_set[s].begin()+1, extended_route_set[s].end());
    }
}

void HeuristicFramework::set_node_labels(Route &route)
{
    route.node_labels.clear();
    vector<vector<int>> node_labels_set;
    for(int i = 0; i < route.compact_route.size()-1; i++)
    {
        int node1 = route.compact_route[i];
        int node2 = route.compact_route[i+1];
        int used_path_id = route.used_paths_in_compact_route[i];
        vector<int> ij_used_path = nodeset.avail_path_set[node1][node2][used_path_id].KSP_Path;
        vector<int> node_labels_vec;
        for(int p = 0; p < ij_used_path.size(); p++)
        {
            if(p == ij_used_path.size()-1)
            {
                node_labels_vec.push_back(1);
            }
            else
            {
                node_labels_vec.push_back(0);
            }
        }
        node_labels_set.push_back(node_labels_vec);
    }
    route.node_labels.insert(route.node_labels.end(), node_labels_set[0].begin(), node_labels_set[0].end());
    for(int s = 1; s < node_labels_set.size(); s++)
    {
        route.node_labels.insert(route.node_labels.end(), node_labels_set[s].begin()+1, node_labels_set[s].end());
    }
    route.node_labels[route.node_labels.size()-1] = 0;
}

void HeuristicFramework::set_route_arrdep_tw(Route &route)
{
    TimeWindowUpdater twupdater(route, nodeset);
    twupdater.cal_route_tw();
    twupdater.set_route_tw(route.route_deptw, route.route_arrtw);
}

void HeuristicFramework::set_route_arrdep_time(Route &route)
{
    TimeWindowUpdater twupdater(route, nodeset);
    twupdater.set_arrdep_time_one_route(route.route_arrtw, route.route_deptw, route.route_arrtime, route.route_deptime);
}

void HeuristicFramework::set_route_loads(Route &route)
{
    route.route_load.resize(route.compact_route.size());
    for(int i = 1; i < route.compact_route.size()-1; i++)
    {
        int node_id = route.compact_route[i];
        route.route_load[0] += (nodeset.demand_type[node_id] == 1) ? -nodeset.demands[node_id] : 0;
    }
    for(int k = 1; k < route.compact_route.size(); k++)
    {
        route.route_load[k] = route.route_load[k-1] + nodeset.demands[route.compact_route[k]];
    }
}

void HeuristicFramework::set_route_mileage(Route &route)
{
    route.route_mileage.resize(route.compact_route.size());
    route.route_mileage[0] = 0;
    for(int i = 0; i < route.compact_route.size()-1; i++)
    {
        double dist_from_i = nodeset.avail_path_set[route.compact_route[i]][route.compact_route[i+1]][route.used_paths_in_compact_route[i]].KSP_Dist;
        route.route_mileage[i+1] += route.route_mileage[i] + dist_from_i;
    }
}

void HeuristicFramework::insert_used_path(vector<int> &used_paths_vec, int insert_nodepos_compact, int used_path_before, int used_path_after)
{
    used_paths_vec[insert_nodepos_compact-1] = used_path_before;
    used_paths_vec.insert(used_paths_vec.begin() + insert_nodepos_compact, used_path_after);
}

void HeuristicFramework::remove_used_path(vector<int> &used_paths_vec, int remove_nodepos_compact, int changed_used_path)
{
    used_paths_vec.erase(used_paths_vec.begin()+remove_nodepos_compact);
    used_paths_vec[remove_nodepos_compact-1] = changed_used_path;
}

//should also consider the case of inserting multiple nodes together? -> but in reality, nodes are inserted one by one -> still, the feasibility check and the actual insertion or removal operation should be seperate and independent, otherwise the cal_cheapest_cost will be problematic
void HeuristicFramework::modify_route_insert_node(Route &route, int node_pos_compact, int node_id) 
{
    //veh_id and veh_type unchanged;
    route.compact_route.insert(route.compact_route.begin() + node_pos_compact, node_id);
    insert_used_path(route.used_paths_in_compact_route, node_pos_compact, 0, 0);
    set_extended_route(route);
    set_node_labels(route);
    // TimeWindowUpdater twupdater(route, nodeset);
    // twupdater.cal_route_tw();
    // twupdater.set_route_tw(route.route_deptw, route.route_arrtw);
    route.route_arrtw.clear();
    route.route_deptw.clear();
    route.route_arrtime.clear();
    route.route_deptime.clear();
    route.route_load.clear();
    route.route_mileage.clear();
    // set_route_loads(route);
    // set_route_mileage(route);
}

//add later: should also consider the case of removing multiple nodes together
void HeuristicFramework::modify_route_remove_node(Route &route, int node_pos_compact)
{
    //veh_id and veh_type unchanged;
    // int node_id = route.compact_route[node_pos_compact];
    route.compact_route.erase(route.compact_route.begin() + node_pos_compact);
    remove_used_path(route.used_paths_in_compact_route, node_pos_compact, 0);
    set_extended_route(route);
    set_node_labels(route);
    // TimeWindowUpdater twupdater(route, nodeset);
    // twupdater.cal_route_tw();
    // twupdater.set_route_tw(route.route_deptw, route.route_arrtw);
    route.route_arrtw.clear();
    route.route_deptw.clear();
    route.route_arrtime.clear();
    route.route_deptime.clear();
    route.route_load.clear();
    route.route_mileage.clear();
    // set_route_loads(route);
    // set_route_mileage(route);
}

bool HeuristicFramework::modify_route_used_path(Route &route, int modify_arcpos_compact, int modified_usedpath_id) //may be revised later -> seperate the feasibility check and the actual modification?
{
    Route routecopy = route;
    routecopy.used_paths_in_compact_route[modify_arcpos_compact] = modified_usedpath_id;
    set_extended_route(routecopy);
    set_node_labels(routecopy);

    TimeWindowUpdater twupdater(routecopy, nodeset);
    twupdater.cal_route_tw();
    bool istimefeas = twupdater.check_route_feasibility();
    //bool isdistfeas = check_mileage_insert_feas -> this only consider the feasibility of inserting a node rather than modifying a used path

    bool isdistfeas;
    int node1 = route.compact_route[modify_arcpos_compact];
    int node2 = route.compact_route[modify_arcpos_compact+1];
    double orig_usedpath = route.used_paths_in_compact_route[modify_arcpos_compact];
    double orig_dist = nodeset.avail_path_set[node1][node2][orig_usedpath].KSP_Dist;
    double new_dist = nodeset.avail_path_set[node1][node2][modified_usedpath_id].KSP_Dist;
    isdistfeas = (route.route_mileage[route.compact_route.size()-1] + new_dist - orig_dist) <= vehset.max_range;

    if(istimefeas && isdistfeas)
    {
        route = routecopy;
        twupdater.set_route_tw(route.route_deptw, route.route_arrtw);
        return true;
    }
    else
    {
        return false; //the route is not changed
    }
}

// bool HeuristicFramework::insert_check_module(Route &route, int node_pos_compact, int node_id)
// {
//     Route route_copy = route;
//     modify_route_insert_node(route_copy, node_pos_compact, node_id);
//     TimeWindowUpdater twupdater(route_copy, nodeset);
//     twupdater.cal_route_tw();
//     bool timefeas = twupdater.check_route_feasibility();
//     bool loadfeas = check_load_insert_feas(route, node_pos_compact, node_id);
//     bool milefeas = check_mileage_insert_feas(route, node_pos_compact, node_id);
//     if(timefeas && loadfeas && milefeas)
//     {
//         route = route_copy;
//         twupdater.set_route_tw(route.route_deptw, route.route_arrtw);
//         set_route_loads(route);
//         set_route_mileage(route);
//         return true;
//     }
//     else
//     {
//         return false;
//     }
// }

// bool HeuristicFramework::remove_check_module(Route &route, int node_pos_compact)
// {
//     Route route_copy = route;
//     modify_route_remove_node(route_copy, node_pos_compact);
//     TimeWindowUpdater twupdater(route_copy, nodeset);
//     twupdater.cal_route_tw();
//     bool timefeas = twupdater.check_route_feasibility();
//     if(timefeas)
//     {
//         route = route_copy;
//         twupdater.set_route_tw(route.route_deptw, route.route_arrtw);
//         set_route_loads(route);
//         set_route_mileage(route);
//         return true;
//     }
//     else
//     {
//         return false;
//     }
// }

// void HeuristicFramework::complete_modified_route(Route &route)
// {
//     set_route_arrdep_tw(route);
//     set_route_loads(route);
//     set_route_mileage(route);
// }

/*calculating a route*/
// int HeuristicFramework::cal_nodepos_loads(Route &route, int node_pos_in_compact_route)
// {
//     if(node_pos_in_compact_route > 0) //for starting depot
//     {
//         int load = route.route_load[node_pos_in_compact_route-1] + nodeset.demands[route.compact_route[node_pos_in_compact_route]];
//         return load;
//     }
//     else //for nodes afterward
//     {
//         int load;
//         for(int i = 1; i < route.compact_route.size()-1; i++)
//         {
//             int node_id = route.compact_route[i];
//             load += (nodeset.demand_type[node_id] == 1) ? -nodeset.demands[node_id] : 0;
//         }
//         return load;
//     }
// }

//calculate the insertion cost of a customer into a route at a given position -> between the starting and ending depot;
double HeuristicFramework::cal_insertion_cost(vector<int> compact_route, int node, int pos, int orig_usedpath_pos)
{
    double cost;
    if(pos < compact_route.size() && pos > 0) //insert the node in the middle of the route
    {
        double add_cost1 = nodeset.avail_path_set[compact_route[pos-1]][node][0].KSP_Dist;
        double add_cost2 = nodeset.avail_path_set[node][compact_route[pos]][0].KSP_Dist;
        double minus_cost = nodeset.avail_path_set[compact_route[pos-1]][compact_route[pos]][orig_usedpath_pos].KSP_Dist;
        cost = add_cost1 + add_cost2 - minus_cost;
    }
    else //insert the node at the start or the end of the route -> cerr
    {
        cerr << "Wrong insertion position!";
        cost = INF;
    }
    return cost;
}

// //calculate the cheapest insertion cost of a customer into a route
// auto HeuristicFramework::cal_cheapest_insert_cost(vector<int> compact_route, vector<int> orig_usedpath_compact, int node)
// {
//     pair<double, int> cost_pos;  // a pair is created to store the cheapest insertion cost and the corresponding position of the node in the route
//     vector<double> all_insertion_costs;
//     for(int i = 1; i < compact_route.size(); i++) //the node cannot be inserted before the starting depot and ending depot
//     {
//         int usedpath_id = orig_usedpath_compact[i-1];
//         double cost = cal_insertion_cost(compact_route, node, i, usedpath_id);
//         all_insertion_costs.push_back(cost);
//     }
//     auto min_pos = min_element(all_insertion_costs.begin(), all_insertion_costs.end());
//     double min_cost = *min_pos;
//     int insert_pos = min_pos - all_insertion_costs.begin() + 1; //1 is for the starting depot
//     cost_pos = make_pair(min_cost,insert_pos);
//     return cost_pos;
// }

//calculate the cheapest insertion cost of a customer into a route
pair<double, int> HeuristicFramework::cal_cheapest_insert_cost(Route &route, int node)
{
    pair<double, int> cost_pos;  // a pair is created to store the cheapest insertion cost and the corresponding position of the node in the route
    vector<double> all_insertion_costs;
    for(int i = 1; i < route.compact_route.size(); i++) //the node cannot be inserted before the starting depot and ending depot
    {
        // Route route_copy = route;
        // modify_route_insert_node(route_copy, i, route.compact_route[i]);
        // TimeWindowUpdater twupdater(route_copy, nodeset);
        // twupdater.cal_route_tw();
        // bool timefeas = twupdater.check_route_feasibility();
        // bool isfeas = check_route_insert_feas(timefeas, route, i, node);
        bool isfeas = check_route_insert_feas(route, i, node); //i: inserted position; 
        if(isfeas)
        {
            int usedpath_id = route.used_paths_in_compact_route[i-1]; //this is the original used path before inserting the node
            double cost = cal_insertion_cost(route.compact_route, node, i, usedpath_id);
            all_insertion_costs.push_back(cost);
        }
        else
        {
            double cost = INF;
            all_insertion_costs.push_back(cost);
        }
    }
    auto min_pos = min_element(all_insertion_costs.begin(), all_insertion_costs.end());
    double min_cost = *min_pos;
    int insert_pos = min_pos - all_insertion_costs.begin() + 1; //1 is for the starting depot
    cost_pos = make_pair(min_cost,insert_pos);
    return cost_pos;
}

//calculate the removal cost of a customer out of a route at a given position
double HeuristicFramework::cal_removal_cost(vector<int> compact_route, vector<int> orig_usedpath_compact, int pos)
{
    double cost;
    if(pos > 0 && pos < compact_route.size()-1) //remove the node in the middle of the route
    {
        double add_cost1 = nodeset.avail_path_set[compact_route[pos-1]][compact_route[pos]][orig_usedpath_compact[pos-1]].KSP_Dist;
        double add_cost2 = nodeset.avail_path_set[compact_route[pos]][compact_route[pos+1]][orig_usedpath_compact[pos]].KSP_Dist;
        double minus_cost = nodeset.avail_path_set[compact_route[pos-1]][compact_route[pos+1]][0].KSP_Dist; //initially, link by the shortest path
        cost = add_cost1 + add_cost2 - minus_cost;
    }
    else //pos == 0 or pos == route.size()-1
    {
        cerr << "Error! You are removing the depot.";
    }
    return cost;
}
int HeuristicFramework::cal_route_pickup_dmd(Route &route)
{
    int pickup_dmd = 0;
    for(int i = 1; i < route.compact_route.size()-1; i++)
    {
        if(nodeset.demand_type[route.compact_route[i]] == 0)
        {
            pickup_dmd += nodeset.demands[route.compact_route[i]];
        }
    }
    return pickup_dmd;
}

int HeuristicFramework::cal_route_delivery_dmd(Route &route)
{
    int delivery_dmd = 0;
    for(int i = 1; i < route.compact_route.size()-1; i++)
    {
        if(nodeset.demand_type[route.compact_route[i]] == 1)
        {
            delivery_dmd -= nodeset.demands[route.compact_route[i]];
        }
    }
    return delivery_dmd;
}

int HeuristicFramework::cal_route_trip_dur(Route &route)
{
    return (route.route_arrtime[route.extended_route.size()-1] - route.route_deptime[0]);
}

double HeuristicFramework::cal_route_total_dist(Route &route)
{
    double total_dist = 0;
    for(int i = 0; i < route.compact_route.size()-1; i++)
    {
        int node1 = route.compact_route[i];
        int used_path_id = route.used_paths_in_compact_route[i];
        for(int j = 0; j < route.compact_route.size(); j++)
        {
            int node2 = route.compact_route[i];
            double ij_used_path_dist = nodeset.avail_path_set[node1][node2][used_path_id].KSP_Dist;
            total_dist += ij_used_path_dist;
        }
    }
    return total_dist;
}

// //find the indices for the served nodes
// vector<int> HeuristicFramework::find_servednode_id(vector<int> labels)
// {
//     vector<int> serve_idvec;
//     auto routelen = labels.size();
//     for(int i = 0; i < routelen; i++)
//     {
//         if(labels[i]) //label == 1: serve
//         {
//             serve_idvec.push_back(i);
//         }
//     }
//     return serve_idvec;
// }

/*route feasibility check*/
bool HeuristicFramework::check_load_insert_feas(Route &route, int insert_nodepos_compact, int insert_nodeid)
{
    bool isfeas = true;
    int vehcap = vehset.veh_cap[route.veh_id];
    auto IsLoadFeas = [&](int node_load) -> bool {return node_load > 0 && node_load < vehcap;}; //if true -> feasible
    int insert_nodedmd = nodeset.demands[insert_nodeid];
    if(nodeset.demand_type[insert_nodeid] == 1) //delivery node
    {
        int starting_load = route.route_load[0] - insert_nodedmd;
        if(!IsLoadFeas(starting_load))
        {
            return false;
        }
        for(int i = 1; i < insert_nodepos_compact; i++)
        {
            starting_load += nodeset.demands[route.compact_route[i]];
            if(!IsLoadFeas(starting_load))
            {
                return false;
            }
        }
    }
    else //pickup node
    {
        int starting_load = route.route_load[insert_nodepos_compact-1] + insert_nodedmd;
        if(!IsLoadFeas(starting_load))
        {
            return false;
        }
        for(int i = insert_nodepos_compact; i < route.compact_route.size()-1; i++)
        {
            starting_load += nodeset.demands[route.compact_route[i]];
            if(!IsLoadFeas(starting_load))
            {
                return false;
            }
        }
    }
    return isfeas;
}

bool HeuristicFramework::check_mileage_insert_feas(Route &route, int insert_nodepos_compact, int insert_nodeid)
{
    int orig_usedpath_pos = route.used_paths_in_compact_route[insert_nodepos_compact-1];
    double inc_cost = cal_insertion_cost(route.compact_route, insert_nodeid, insert_nodepos_compact, orig_usedpath_pos);
    double new_cost = cal_route_total_dist(route) + inc_cost;
    if(new_cost > vehset.max_range)
    {
        return false;
    }
    return true;
}

bool HeuristicFramework::check_tw_insert_feas(Route &route, int insert_nodepos_compact, int insert_nodeid)
{
    Route route_copy = route;
    modify_route_insert_node(route_copy, insert_nodepos_compact, insert_nodeid);
    TimeWindowUpdater twupdater(route_copy, nodeset);
    twupdater.cal_route_tw();
    return twupdater.check_route_feasibility();
}

bool HeuristicFramework::check_tw_remove_feas(Route &route, int remove_nodepos_compact)
{
    Route route_copy = route;
    modify_route_remove_node(route_copy, remove_nodepos_compact);
    TimeWindowUpdater twupdater(route_copy, nodeset);
    return twupdater.check_route_feasibility();
}

bool HeuristicFramework::check_route_insert_feas(Route &route, int insert_nodepos_compact, int insert_nodeid)
{
    // Route route_copy = route;
    // modify_route_insert_node(route_copy, insert_nodepos_compact, insert_nodeid);
    // TimeWindowUpdater twupdater(route_copy, nodeset);
    // inserted_twupdater.cal_route_tw();
    // bool timefeas = inserted_twupdater.check_route_feasibility();
    bool timefeas = check_tw_insert_feas(route, insert_nodepos_compact, insert_nodeid);
    bool loadfeas = check_load_insert_feas(route, insert_nodepos_compact, insert_nodeid);
    bool milefeas = check_mileage_insert_feas(route, insert_nodepos_compact, insert_nodeid);
    return (timefeas && loadfeas && milefeas) ? true : false;
}

//when removing a node, need to check the time feasibility
bool HeuristicFramework::check_route_remove_feas(Route &route, int remove_nodepos_compact)
{
    return check_tw_remove_feas(route, remove_nodepos_compact);
}

/*building a solution*/
void HeuristicFramework::find_platoons_all_arcs(Solution &sol)
{
    PlatoonMaker plmaker(sol, nodeset);
    sol.sol_platoons_all_arcs = plmaker.get_coupling_sol();
}

void HeuristicFramework::build_complete_sol(Solution &sol)
{
    // find_platoons_all_arcs(sol);
    PlatoonMaker plmaker(sol, nodeset);
    sol.sol_platoons_all_arcs = plmaker.get_coupling_sol();
    plmaker.set_arrdep_time_all_routes();
    for(int i = 0; i < sol.sol_config.size(); i++)
    {
        sol.sol_config[i].route_arrtw = plmaker.get_arrtw_after_platoon(i);
        sol.sol_config[i].route_deptw = plmaker.get_deptw_after_platoon(i);
        sol.sol_config[i].route_arrtime = plmaker.get_arrtime_after_platoon(i);
        sol.sol_config[i].route_deptime = plmaker.get_arrtime_after_platoon(i);
    }
    sol.total_energy_related_dist = cal_sol_total_energy_dist_cost(sol);
    sol.total_trip_duration = cal_sol_total_trip_dur(sol);
    sol.total_unserved_requests = cal_sol_total_unserved_requests(sol);
    sol.total_obj_val = cal_sol_total_obj_val(sol);
}

/*calculating a solution*/
//get the number of routes in the initial solution
int HeuristicFramework::get_sol_route_num(Solution &sol)
{
    return sol.sol_config.size();
}

Route HeuristicFramework::get_route_config(Solution &sol, int route_id) //route_id != veh_id
{
    return sol.sol_config[route_id];
}

double HeuristicFramework::cal_sol_total_dist(Solution &sol)
{
    double total_dist = 0;
    for(int i = 0; i < sol.sol_config.size(); i++)
    {
        total_dist += cal_route_total_dist(sol.sol_config[i]);
    }
    return total_dist;
}

double HeuristicFramework::cal_sol_total_energy_dist_cost(Solution &sol)
{
    double sol_total_energy_dist = 0;
    PlatoonMaker plmaker(sol, nodeset);
    return plmaker.cal_sol_total_energy_dist();
}

int HeuristicFramework::cal_sol_total_trip_dur(Solution &sol)
{
    int total_tripdur = 0;
    for(int i = 0; i < sol.sol_config.size(); i++)
    {
        total_tripdur += cal_route_trip_dur(sol.sol_config[i]);
    }
    return total_tripdur;
}

int HeuristicFramework::cal_sol_total_unserved_requests(Solution &sol)
{
    int total_cus = nodeset.nodenum - 1;  //exclude the depot;
    int total_served = 0;
    for(int i = 0; i < sol.sol_config.size(); i++)
    {
        total_served += sol.sol_config[i].compact_route.size()-2;
    }
    return total_served;
}

double HeuristicFramework::cal_sol_total_obj_val(Solution &sol)
{
    //total obj val = sum weight of total energy-related distance cost, total trip duration, total unserved requests
    double total_obj_val = 0;
    total_obj_val += obj_w1 * cal_sol_total_energy_dist_cost(sol);
    total_obj_val += obj_w2 * cal_sol_total_trip_dur(sol);
    total_obj_val += obj_w3 * cal_sol_total_unserved_requests(sol);
    return total_obj_val;
}

//get the best solution
Solution HeuristicFramework::get_best_solution()
{
    return best_sol;
}

vector<double> HeuristicFramework::get_cputime()
{
    vector<double> cpu_time = {cpu_time_before_platoon, cpu_time_after_platoon};
    return cpu_time;
}