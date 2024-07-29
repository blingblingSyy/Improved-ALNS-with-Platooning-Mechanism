#include <iostream>  
#include <vector>  
#include <set>
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include <tuple>
#include <numeric>
#include "PlatoonMaker.h"
#include "config.h"
#include "utility.h"
using namespace std;  

/*PlatoonMaker: make platoons based on the current solution*/
PlatoonMaker::PlatoonMaker(Solution &sol, Nodes nodes) //, Nodes &nodes
{
    cur_sol = sol;
    nodeset = nodes;
    routes_num = sol.sol_config.size();
    set_arcs_based_extended_routes();  //arcs_based_extended_routes
    find_unqiue_arcs(); //unique_arcs_set
    unique_arcs_num = unique_arcs_set.size();
    unique_arcs_config.resize(unique_arcs_num);
    find_vehs_on_all_arcs(); //unique_arcs_config.common_vehs & unique_arcs_config.routes_id
    find_all_arcpos_each_veh(); //unique_arcs_config.arc_all_pos
}

//make route with arcs
vector<vector<int>> PlatoonMaker::make_arc_based_route(vector<int> input_extended_route)
{
    vector<vector<int>> arcs_based_route;
    int routelen = input_extended_route.size();
    for(int i = 0; i < routelen-1; i++)
    {
        arcs_based_route.push_back({input_extended_route[i], input_extended_route[i+1]});
    }
    return arcs_based_route;
}

void PlatoonMaker::set_arcs_based_extended_routes()
{
    for(int i = 0; i < routes_num; i++)
    {
        arcs_based_extended_routes.push_back(make_arc_based_route(cur_sol.sol_config[i].extended_route));
    }
}

//find the set of unique arcs in the given solution
void PlatoonMaker::find_unqiue_arcs()
{
    for(int i = 0; i < routes_num; i++)
    {
        //add the arcs of the current route to the unique_arcs_set
        unique_arcs_set.insert(unique_arcs_set.end(), arcs_based_extended_routes[i].begin(), arcs_based_extended_routes[i].end());
    }
    //find the unique arc set of all routes
    set<vector<int>>unique_arcs_all_routes(unique_arcs_set.begin(), unique_arcs_set.end());
    unique_arcs_set.assign(unique_arcs_all_routes.begin(), unique_arcs_all_routes.end());
}

//find the set of vehicles sharing the same arc
//is it possible that an arc appears more than one times in the same route? (possible -> but may barely appear -> show it in the vehicle-arc-position matrix)
void PlatoonMaker::find_vehs_on_all_arcs()
{
    for(int u = 0; u < unique_arcs_num; u++)
    {
        unique_arcs_config[u].thisarc = unique_arcs_set[u];
        for(int r = 0; r < routes_num; r++)
        {
            if(find(arcs_based_extended_routes[r].begin(), arcs_based_extended_routes[r].end(), unique_arcs_set[u]) != arcs_based_extended_routes[r].end())
            {
                unique_arcs_config[u].common_vehs.push_back(cur_sol.sol_config[r].veh_id);
                unique_arcs_config[u].routes_id.push_back(r);
            }
        }
    }
}

//set the matrix of vehicle-arc-positions
void PlatoonMaker::find_all_arcpos_each_veh()
{
    for(int u = 0; u < unique_arcs_num; u++)
    {
        int common_vehs_num = unique_arcs_config[u].routes_id.size();
        unique_arcs_config[u].arc_all_pos.resize(common_vehs_num);
        for(int i = 0; i < common_vehs_num; i++)
        {
            int rid = unique_arcs_config[u].routes_id[i];
            vector<vector<int>>::iterator it;
            it = find(arcs_based_extended_routes[rid].begin(), arcs_based_extended_routes[rid].end(), unique_arcs_set[u]);
            //find all positions of the arc in the route r
            while(it != arcs_based_extended_routes[rid].end())
            {
                int arc_pos = it - arcs_based_extended_routes[rid].begin();
                unique_arcs_config[u].arc_all_pos[i].push_back(arc_pos);
                it = find(it+1, arcs_based_extended_routes[rid].end(), unique_arcs_set[u]);
            }
        }
    }
}

// //set the matrix of vehicle-arc-positions
// void PlatoonMaker::find_all_arcpos_each_veh()
// {
//     vehs_arcs_matrix.resize(routes_num); //cur_sol.sol_config[r].veh_id
//     for(int r = 0; r < routes_num; r++)
//     {
//         int vehid = cur_sol.sol_config[r].veh_id;
//         vehs_arcs_matrix[r].resize(unique_arcs_num);
//         for(int u = 0; u < unique_arcs_num; u++)
//         {
//             //the route does not contain the arc
//             if(find(vehs_on_unique_arcs[u].begin(), vehs_on_unique_arcs[u].end(), vehid) == vehs_on_unique_arcs[u].end())
//             {
//                 vehs_arcs_matrix[r][u] = {};
//             }
//             else //the route contains the arc
//             {
//                 vector<vector<int>>::iterator it;
//                 it = find(arcs_based_extended_routes[r].begin(), arcs_based_extended_routes[r].end(), unique_arcs_set[u]);
//                 //find all positions of the arc in the route r
//                 while(it != arcs_based_extended_routes[r].end())
//                 {
//                     int arc_pos = it - arcs_based_extended_routes[r].begin();
//                     vehs_arcs_matrix[r][u].push_back(arc_pos);
//                     it = find(it, arcs_based_extended_routes[r].end(), unique_arcs_set[u]);
//                 }
//             }
//         }
//     }
// }

//check whether two vehicles can be connected in the pairwise feasibility graph with the corresponding arc positions
bool PlatoonMaker::check_pair_connectivity(pair<int, int> route_pos1, pair<int, int> route_pos2, vector<Route> sol_config_copy)
{
    int routeid1 = route_pos1.first;
    int arc_pos1 = route_pos1.second;
    int routeid2 = route_pos2.first;
    int arc_pos2 = route_pos2.second;
    if(routeid1 == routeid2)
    {
        return false;
    }
    else //routeid1 != routeid2
    {
        vector<int> deptw_veh_arc1 = sol_config_copy[routeid1].route_deptw[arc_pos1];
        vector<int> deptw_veh_arc2 = sol_config_copy[routeid2].route_deptw[arc_pos2];
        vector<int> overlap_tw_veh_arc = {max(deptw_veh_arc1[0], deptw_veh_arc2[0]), min(deptw_veh_arc1[1], deptw_veh_arc2[1])};
        if(overlap_tw_veh_arc[0] > overlap_tw_veh_arc[1])
        {
            return false;
        }
        return true;
    }
}

//build node set of the pairwise feasibility graph for a given arc
vector<pair<int, int>> PlatoonMaker::build_graph_node_set(CouplingArc input_arc_config)
{
    vector<pair<int, int>> graph_nodeset;
    for(int i = 0; i < input_arc_config.routes_id.size(); i++)
    {
        for(int j = 0; j < input_arc_config.arc_all_pos[i].size(); j++)
        {
            graph_nodeset.push_back(make_pair(input_arc_config.routes_id[i], input_arc_config.arc_all_pos[i][j]));
        }  //routes_id can identify the vehicle id but it is difficult to identify the route id with the vehicle id
    }
    return graph_nodeset;
}

// //build pairwise feasibility graph for a given arc
// vector<vector<pair<int, int>>> PlatoonMaker::build_pairwise_feasibility_graph(CouplingArc input_arc_config)
// {
//     vector<pair<int, int>> graph_nodes = build_graph_node_set(input_arc_config);
//     vector<vector<pair<int, int>>> pairwise_feasibility_graph;
//     for(int i = 0; i < graph_nodes.size()-1; i++)
//     {
//         for(int j = i+1; j < graph_nodes.size(); j++)
//         {
//             bool veh_connect = check_pair_connectivity(graph_nodes[i], graph_nodes[j]);
//             if(veh_connect)
//             {
//                 pairwise_feasibility_graph.push_back({graph_nodes[i], graph_nodes[j]});
//             }
//         }
//     }
//     return pairwise_feasibility_graph;
// }

//build pairwise feasibility graph for a given arc
vector<vector<bool>> PlatoonMaker::build_pairwise_feasibility_graph(vector<pair<int, int>> &graph_nodes, vector<Route> &sol_config_copy) //CouplingArc input_arc_config
{
    // vector<pair<int, int>> graph_nodes = build_graph_node_set(input_arc_config);
    int graph_nodes_size = graph_nodes.size();
    vector<vector<bool>> pairwise_feasibility_graph(graph_nodes_size);
    for(int i = 0; i < graph_nodes_size; i++)
    {
        pairwise_feasibility_graph[i].resize(graph_nodes_size);
        for(int j = i; j < graph_nodes_size; j++) //case: j >= i
        {
            bool veh_connect = check_pair_connectivity(graph_nodes[i], graph_nodes[j], sol_config_copy);
            pairwise_feasibility_graph[i][j] = (veh_connect) ? true : false;
        }
    }
    for(int i = 1; i < graph_nodes_size; i++)
    {
        for(int j = 0; j < i; j++) //case: j < i
        {
            pairwise_feasibility_graph[i][j] = pairwise_feasibility_graph[j][i];
        }
    }
    return pairwise_feasibility_graph;
}

//erase the neighbourhood of a given element from a given vector set
vector<int> PlatoonMaker::process_neighbor_in_set(int input_u, vector<int> input_set, vector<vector<bool>> &pair_feas_graph, bool keep_or_not)
{
    vector<int> input_set_copy = input_set;
    for(int i = 0; i < input_set_copy.size(); i++) //if input_set_copy is empty, then the for cycle will not be executed
    {
        if(!keep_or_not) //remove the neighbourhood (keep_or_not == 0)
        {
            if(pair_feas_graph[input_u][input_set_copy[i]]) //if u and Pset[i] are connected. If u = Pset[i], they are unconnected
            {
                input_set_copy[i] = -1;
            }
        }
        else //keep the neighbourhood (keep_or_not == 1), remove the elements that are not neighbourhood
        {
            if(!pair_feas_graph[input_u][input_set_copy[i]]) //if u and Pset[i] are unconnected. If u = Pset[i], they are unconnected
            {
                input_set_copy[i] = -1;
            }
        }
    }
    input_set_copy.erase(remove(input_set_copy.begin(), input_set_copy.end(), -1), input_set_copy.end()); //P \ N(u)
    return input_set_copy;
}

//basic Bron-Kerbosch algorithm
void PlatoonMaker::BronKerbosch(vector<int> Rset, vector<int> Pset, vector<int> Xset, vector<vector<bool>> &pair_feas_graph, vector<vector<int>> &maximal_cliques_set)
{
    if(Pset.empty() && Xset.empty()) //report a maximal clique when both Pset and Xset are empty
    {
        maximal_cliques_set.push_back(Rset); //report R
        // if(find(maximal_cliques_set.begin(), maximal_cliques_set.end(), Rset) == maximal_cliques_set.end())
        // {
        //     maximal_cliques_set.push_back(Rset); //report R
        // }
    }
    else
    {
        vector<int> Mixset = Pset;
        Mixset.insert(Mixset.end(), Xset.begin(), Xset.end());
        int u = Mixset[0]; //choose pivot to be the first element in Pset & Xset -> the element is the index of the first graph node
        vector<int> P_erase_u_neighbour = process_neighbor_in_set(u, Pset, pair_feas_graph, false); //P \ N(u)
        if(!P_erase_u_neighbour.empty())
        {
            for(int i = 0; i < P_erase_u_neighbour.size(); i++)
            {
                int v = P_erase_u_neighbour[i];
                vector<int> Rset1 = Rset;
                Rset1.push_back(v); //Rset1 = Rset + {v}
                // if(find(Rset1.begin(), Rset1.end(), v) == Rset1.end())
                // {
                //     Rset1.push_back(v); //Rset1 = Rset + {v}
                // }
                vector<int> P_keep_v_neighbour = process_neighbor_in_set(v, Pset, pair_feas_graph, true); //Pset1 = N(v) in Pset (upper layer of Pset)
                vector<int> X_keep_v_neighbour = process_neighbor_in_set(v, Xset, pair_feas_graph, true); //Xset1 = N(v) in Xset (upper layer of Xset)
                BronKerbosch(Rset1, P_keep_v_neighbour, X_keep_v_neighbour, pair_feas_graph, maximal_cliques_set);
                Pset.erase(remove(Pset.begin(), Pset.end(), v), Pset.end()); //Pset = Pset \ {v};
                Xset.push_back(v); //Xset = Xset + {v}
            }
        }
    }
}

// //(original) basic Bron-Kerbosch algorithm
// void PlatoonMaker::BronKerbosch(vector<int> Rset, vector<int> Pset, vector<int> Xset, vector<vector<bool>> &pair_feas_graph, vector<vector<int>> &maximal_cliques_set)
// {
//     if(Pset.empty() && Xset.empty())
//     {
//         maximal_cliques_set.push_back(Rset); //report R
//     }
//     vector<int> Mixset = Pset;
//     Mixset.insert(Mixset.end(), Xset.begin(), Xset.end());
//     int u = Mixset[0]; //choose pivot to be the first element in Pset & Xset -> the element is the index of the first graph node
//     process_neighbor_in_set(u, Pset, pair_feas_graph, false); //P \ N(u)
//     while(!Pset.empty())
//     {
//         int v = Pset[0];
//         vector<int> Rset1 = Rset, Pset1 = Pset, Xset1 = Xset;
//         if(find(Rset1.begin(), Rset1.end(), v) == Rset1.end())
//         {
//             Rset1.push_back(v); //Rset1 = Rset + {v}
//         }
//         process_neighbor_in_set(v, Pset1, pair_feas_graph, true); //Pset1 = N(v) in Pset
//         process_neighbor_in_set(v, Xset1, pair_feas_graph, true); //Xset1 = N(v) in Xset
//         BronKerbosch(Rset1, Pset1, Xset1, pair_feas_graph, maximal_cliques_set);
//         Pset.erase(Pset.begin()); //Pset = Pset \ {v};
//         Xset.push_back(v); //Xset = Xset + {v}
//     }
// }

//transform the vector of node id to the corresponding graph node
vector<pair<int, int>> PlatoonMaker::transform_id_graphnode(vector<int> &clique_idset, vector<pair<int, int>> &graph_nodes)
{
    vector<pair<int, int>> intersect_nodeset;
    for(int i = 0; i < clique_idset.size(); i++)
    {
        intersect_nodeset.push_back(graph_nodes[clique_idset[i]]);
    }
    return intersect_nodeset;
}


//find all maximal cliques with node indices based on the pairwise feasibility graph with Bron-Kerbosch algorithm
vector<vector<int>> PlatoonMaker::BK_maximal_cliques_idset(vector<pair<int, int>> &graph_nodes, vector<vector<bool>> &pair_feas_graph) //CouplingArc input_arc_config
{
    // vector<pair<int, int>> graph_nodes = build_graph_node_set(input_arc_config);
    // vector<vector<bool>> pair_feas_graph = build_pairwise_feasibility_graph(graph_nodes); //CouplingArc input_arc_config
    vector<vector<int>> maximal_cliques_id_set;
    vector<int> init_Rset = {}, init_Pset, init_Xset = {};
    for(int i = 0; i < graph_nodes.size(); i++)
    {
        init_Pset.push_back(i); //Pset = {all v's}
    }
    BronKerbosch(init_Rset, init_Pset, init_Xset, pair_feas_graph, maximal_cliques_id_set);
    return maximal_cliques_id_set;
}

//find all maximal cliques based on the pairwise feasibility graph with Bron-Kerbosch algorithm
vector<vector<pair<int, int>>> PlatoonMaker::BK_maximal_cliques(vector<pair<int, int>> &graph_nodes, vector<vector<bool>> &pair_feas_graph) //CouplingArc input_arc_config
{
    // vector<pair<int, int>> graph_nodes = build_graph_node_set(input_arc_config);
    // vector<vector<bool>> pair_feas_graph = build_pairwise_feasibility_graph(graph_nodes); //CouplingArc input_arc_config
    vector<vector<int>> maximal_cliques_id_set;
    vector<int> init_Rset = {}, init_Pset, init_Xset = {};
    for(int i = 0; i < graph_nodes.size(); i++)
    {
        init_Pset.push_back(i); //Pset = {all v's}
    }
    BronKerbosch(init_Rset, init_Pset, init_Xset, pair_feas_graph, maximal_cliques_id_set);
    //transform id set to pair<int, int> set
    vector<vector<pair<int, int>>> maximal_cliques_set(maximal_cliques_id_set.size());
    for(int i = 0; i < maximal_cliques_id_set.size(); i++)
    {
        maximal_cliques_set.resize(maximal_cliques_id_set[i].size());
        for(int j = 0; j < maximal_cliques_id_set[i].size(); j++)
        {
            maximal_cliques_set[i][j] = graph_nodes[j];
        }
    }
    return maximal_cliques_set;
}

//find the maximum clique with node indices for one arc given the current graph nodes
vector<int> PlatoonMaker::BK_maximum_clique_idset(vector<vector<int>> all_maximal_cliques_idset_one_arc)
{   
    int all_cliques_num = all_maximal_cliques_idset_one_arc.size();
    vector<int> all_cliques_size(all_cliques_num);
    for(int i = 0; i < all_cliques_num; i++)
    {
        all_cliques_size[i] = all_maximal_cliques_idset_one_arc[i].size();
    }
    auto it = max_element(all_cliques_size.begin(), all_cliques_size.end());
    return all_maximal_cliques_idset_one_arc[it - all_cliques_size.begin()];

    // int maximal_clique_size = all_maximal_cliques_idset_one_arc[0].size();
    // int maximal_clique_id = 0;
    // for(int i = 1; i < all_maximal_cliques_idset_one_arc.size(); i++)
    // {
    //     int clique_size = all_maximal_cliques_idset_one_arc[i].size();
    //     if(clique_size > maximal_clique_size)
    //     {
    //         maximal_clique_id = i;
    //         maximal_clique_size = clique_size;
    //     }
    // }
    // return all_maximal_cliques_idset_one_arc[maximal_clique_id];
}


//find the maximum clique
vector<pair<int, int>> PlatoonMaker::BK_maximum_clique(vector<vector<pair<int, int>>> all_maximal_cliques_one_arc)
{
    int maximal_clique_size = 0;
    int maximal_clique_id = all_maximal_cliques_one_arc[0].size();
    for(int i = 1; i < all_maximal_cliques_one_arc.size(); i++)
    {
        int clique_size = all_maximal_cliques_one_arc[i].size();
        if(clique_size > maximal_clique_size)
        {
            maximal_clique_id = i;
        }
    }
    return all_maximal_cliques_one_arc[maximal_clique_id];
}

//calcualate the overlapped departure time window of the start node on the given arc
vector<int> PlatoonMaker::cal_overlap_deptw_on_arc_per_platoon(vector<pair<int, int>> platoon_config_on_arc) //, vector<int> input_arc
{
    //note that the size of the used vehicles should be equal to the size of the set of arc positions
    //arc position is the position of the start node of the arc in each route
    vector<int> overlapped_tw(2);
    int init_rid = platoon_config_on_arc[0].first;
    int init_start_nodepos = platoon_config_on_arc[0].second;
    overlapped_tw = cur_sol.sol_config[init_rid].route_deptw[init_start_nodepos];
    for(int i = 1; i < platoon_config_on_arc.size(); i++)
    {
        int rid = platoon_config_on_arc[i].first;
        int start_nodepos = platoon_config_on_arc[i].second;
        vector<int> arc_deptw_rid = cur_sol.sol_config[rid].route_deptw[start_nodepos];
        overlapped_tw = {max(arc_deptw_rid[0], overlapped_tw[0]), min(arc_deptw_rid[1], overlapped_tw[1])};
    }
    if(overlapped_tw[0] > overlapped_tw[1])
    {
        return {};
    }
    else
    {
        return overlapped_tw;
    }
}

//update the arrival time windows of all nodes within a given route
void PlatoonMaker::update_arrdeptw_on_route(vector<pair<int, int>> &platoon_config_on_arc, vector<int> maximum_platoon_tw, vector<Route> &sol_config_copy)
{
    // vector<int> maximum_platoon_tw = cal_overlap_deptw_on_arc_per_platoon(platoon_config_on_arc);
    if(!maximum_platoon_tw.empty())
    {
        for(int i = 0; i < platoon_config_on_arc.size(); i++)
        {
            int rid = platoon_config_on_arc[i].first;
            int arc_start_pos = platoon_config_on_arc[i].second;
            TimeWindowUpdater twupdater(sol_config_copy[rid], nodeset);  //only update the departure time windows of the routes used in the platoon
            twupdater.calibrate_route_tw(arc_start_pos, maximum_platoon_tw);
            twupdater.set_route_tw(sol_config_copy[rid].route_deptw, sol_config_copy[rid].route_arrtw); //modify the arrival and departure time windows for a route given a modified departure time window for a node //..., vector<vector<int>> &route_deptw, vector<vector<int>> &route_arrtw
        }
    }
}

// find the maximum cliques heuristically
vector<vector<pair<int, int>>> PlatoonMaker::find_platoons_one_arc(vector<pair<int, int>> &graph_nodes, vector<vector<bool>> &pair_feas_graph, vector<vector<int>> &overlapped_deptw_each_platoon) //CouplingArc input_arc_config
{
    // vector<pair<int, int>> graph_nodes = build_graph_node_set(input_arc_config);
    // vector<vector<bool>> pair_feas_graph = build_pairwise_feasibility_graph(graph_nodes); //CouplingArc input_arc_config
    vector<vector<pair<int, int>>> all_cliques_thisarc;
    while(!graph_nodes.empty())
    {
        vector<vector<int>> all_maximal_cliques_thisarc = BK_maximal_cliques_idset(graph_nodes, pair_feas_graph);
        vector<int> maximum_clique_thisarc_idset = BK_maximum_clique_idset(all_maximal_cliques_thisarc); //find the maximum clique
        //transform maximum clique ids to nodes
        vector<pair<int, int>> maximum_clique_thisarc = transform_id_graphnode(maximum_clique_thisarc_idset, graph_nodes);
        all_cliques_thisarc.push_back(maximum_clique_thisarc);
        //output the overlapped departure time windows of each platoon
        vector<int> maximum_platoon_tw = cal_overlap_deptw_on_arc_per_platoon(maximum_clique_thisarc); //what if returning {}?
        overlapped_deptw_each_platoon.push_back(maximum_platoon_tw);
        //update time windows with the maximum platoon
        vector<Route> sol_config_copy = cur_sol.sol_config;
        update_arrdeptw_on_route(maximum_clique_thisarc, maximum_platoon_tw, sol_config_copy);
        //remove all the nodes corresponding to the nodes in the maximum clique
        process_intersections(graph_nodes, maximum_clique_thisarc);
        //remove all the links adjacent to the nodes in the maximum clique
        if(!graph_nodes.empty())
        {
            pair_feas_graph = build_pairwise_feasibility_graph(graph_nodes, sol_config_copy);
        }
        // pair_feas_graph = build_pairwise_feasibility_graph(graph_nodes, sol_config_copy);
    }
    return all_cliques_thisarc;
}

// //calculate the number of platoons formed on the given arc
// int PlatoonMaker::cal_platoon_num_one_arc(vector<vector<pair<int, int>>> &all_cliques_onearc)
// {
//     return all_cliques_onearc.size();
// }

//calculate the energy saving amount based on the platoons formed on the given arc
double PlatoonMaker::cal_energy_saving(vector<int> thisarc, vector<pair<int, int>> &graph_nodes, vector<vector<pair<int, int>>> &all_cliques_one_arc)
{
    int nodes_size = graph_nodes.size();
    int platoon_size = all_cliques_one_arc.size();  //calculate the number of platoons formed on the given arc
    double arc_len = nodeset.initial_distmat[thisarc[0]][thisarc[1]];
    return 0.1*(nodes_size - platoon_size)*arc_len;
}

//integrate all procedures of coupling on an arc
CouplingArcSol PlatoonMaker::arc_coupling_module(CouplingArc input_arc_config)
{
    CouplingArcSol arc_coupling_sol;

    arc_coupling_sol.thisarc = input_arc_config.thisarc;
    // arc_coupling_sol.route_id_set = input_arc_config.routes_id;
    // arc_coupling_sol.veh_set = input_arc_config.common_vehs;
    // arc_coupling_sol.arc_pos_each_route = input_arc_config.arc_all_pos;
    vector<pair<int, int>> graph_nodes_init = build_graph_node_set(input_arc_config);
    vector<pair<int, int>> graph_nodes = graph_nodes_init;
    vector<vector<bool>> pair_feas_graph = build_pairwise_feasibility_graph(graph_nodes_init, cur_sol.sol_config); //CouplingArc input_arc_config
    //overlapped departure time windows of each platoon
    arc_coupling_sol.overlaped_deptw_startnode.clear();
    //all platoons configuration
    vector<vector<pair<int, int>>> all_platoons_one_arc = find_platoons_one_arc(graph_nodes, pair_feas_graph, arc_coupling_sol.overlaped_deptw_startnode); //graph nodes will change
    arc_coupling_sol.platoons_config_on_arc = all_platoons_one_arc;
    //total energy saving of all platoons
    double energy_saving = cal_energy_saving(input_arc_config.thisarc, graph_nodes_init, all_platoons_one_arc);
    arc_coupling_sol.energy_saving = energy_saving;
    
    return arc_coupling_sol;
}

// void PlatoonMaker::coupling_heuristic_all_arcs()
// {
//     coupling_arcs_sol.resize(unique_arcs_num);
//     vector<CouplingArc> unique_arcs_config_copy = unique_arcs_config;
//     vector<CouplingArc> no_platoon_arcs_config;
//     vector<CouplingArcSol> no_platoon_arcs_sol;
//     // vector<int> no_platoon_arcs_idset;
//     //initialize the no_platoon_arcs_idset
//     for(int i = 0; i < unique_arcs_num; i++)
//     {
//         // no_platoon_arcs_idset.push_back(i);
//         coupling_arcs_sol[i] = arc_coupling_module(unique_arcs_config_copy[i]);
//         if(coupling_arcs_sol[i].energy_saving == 0)
//         {
//             // no_platoon_arcs_idset.push_back(i);
//             no_platoon_arcs_sol.push_back(coupling_arcs_sol[i]);
//             no_platoon_arcs_config.push_back(unique_arcs_config_copy[i]);
//         }
//     }
//     //remove the arcs that form no platoons
//     process_intersections(coupling_arcs_sol, no_platoon_arcs_sol); //no_platoon_arcs will become empty
//     process_intersections(unique_arcs_config_copy, no_platoon_arcs_config);
//     //to rank the coupling arc solutions by energy saving amount in descending order
//     auto compare = [&](int s, int r) {return coupling_arcs_sol[s].energy_saving > coupling_arcs_sol[r].energy_saving;};
//     while(!unique_arcs_config_copy.empty()) //!coupling_arcs_sol.empty()
//     {
//         // no_platoon_arcs_config.clear();
//         // no_platoon_arcs_sol.clear();
//         //rank the set of unique arcs (coupling solutions) based on the energy saving amount in descending order
//         sort(coupling_arcs_sol.begin(), coupling_arcs_sol.end(), compare);
//         sort(unique_arcs_config_copy.begin(), unique_arcs_config_copy.end(), compare);
//         //select the first arc with the most energy saving
//         CouplingArcSol selected_arcsol = coupling_arcs_sol[0];
//         //modify the arrival and departure time windows of cur_sol based on all platoons coupled on this arc
//         for(int k = 0; k < selected_arcsol.platoons_config_on_arc.size(); k++)
//         {
//             //update the arrival and departure time windows of all common routes in cur_sol
//             update_arrdeptw_on_route(selected_arcsol.platoons_config_on_arc[k], selected_arcsol.overlaped_deptw_startnode[k], cur_sol.sol_config);
//         }
//         //update the number of platoons of other arcs based on the modified time windows
//         for(int s = 1; s < coupling_arcs_sol.size(); s++)
//         {
//             coupling_arcs_sol[s] = arc_coupling_module(unique_arcs_config_copy[s]); //pair_feas_graph has been changed
//             if(coupling_arcs_sol[s].energy_saving == 0)
//             {
//                 no_platoon_arcs_sol.push_back(coupling_arcs_sol[s]);
//                 no_platoon_arcs_config.push_back(unique_arcs_config_copy[s]);
//             }
//             //only keep the coupling arcs that can be platooned
//             process_intersections(coupling_arcs_sol, no_platoon_arcs_sol); //no_platoon_arcs will become empty
//             process_intersections(unique_arcs_config_copy, no_platoon_arcs_config);
//         }
//     }
//     // return coupling_arcs_sol;
// }

void PlatoonMaker::coupling_heuristic_all_arcs()
{
    vector<pair<int, CouplingArcSol>> remaining_coupling_arcs_sol;
    vector<pair<int, CouplingArcSol>> no_platoon_arcs_sol;

    //initialize the no_platoon_arcs_sol
    for(int i = 0; i < unique_arcs_num; i++)
    {
        CouplingArcSol one_coupling_arc_sol = arc_coupling_module(unique_arcs_config[i]);
        remaining_coupling_arcs_sol.push_back(make_pair(i, one_coupling_arc_sol));

        if(one_coupling_arc_sol.energy_saving == 0)
        {
            no_platoon_arcs_sol.push_back(make_pair(i, one_coupling_arc_sol));
        }
    }
    //remove the arcs that form no platoons -> this will update the remaining_coupling_arcs_sol
    process_intersections(remaining_coupling_arcs_sol, no_platoon_arcs_sol);

    //to rank the coupling arc solutions by energy saving amount in descending order
    auto compare = [&](pair<int, CouplingArcSol> s, pair<int, CouplingArcSol> r) {return s.second > r.second;};
    while(!remaining_coupling_arcs_sol.empty())
    {
        //rank the set of unique arcs (coupling solutions) based on the energy saving amount in descending order
        sort(remaining_coupling_arcs_sol.begin(), remaining_coupling_arcs_sol.end(), compare);
        //select the first arc with the most energy saving
        CouplingArcSol selected_arcsol = remaining_coupling_arcs_sol[0].second;
        coupling_arcs_sol.push_back(selected_arcsol); //this will update the coupling_arcs_sol
        remaining_coupling_arcs_sol.erase(remaining_coupling_arcs_sol.begin()); //this will update the remaining_coupling_arcs_sol
        //modify the arrival and departure time windows of cur_sol based on all platoons coupled on this arc
        for(int k = 0; k < selected_arcsol.platoons_config_on_arc.size(); k++)
        {
            //update the arrival and departure time windows of all common routes in cur_sol
            update_arrdeptw_on_route(selected_arcsol.platoons_config_on_arc[k], selected_arcsol.overlaped_deptw_startnode[k], cur_sol.sol_config);
        }
        //update the number of platoons of other arcs based on the modified time windows
        for(int s = 1; s < remaining_coupling_arcs_sol.size(); s++)
        {
            int unique_arc_index = remaining_coupling_arcs_sol[s].first;
            remaining_coupling_arcs_sol[s].second = arc_coupling_module(unique_arcs_config[unique_arc_index]); //pair_feas_graph has been changed
            if(remaining_coupling_arcs_sol[s].second.energy_saving == 0)
            {
                no_platoon_arcs_sol.push_back(remaining_coupling_arcs_sol[s]);
            }
            //only keep the coupling arcs that can be platooned
            process_intersections(remaining_coupling_arcs_sol, no_platoon_arcs_sol);
        }
    }
}

//before set_arrdep_time_all_routes(), do coupling_heuristic_all_arcs() to get the updated route_arrtw and route_deptw
void PlatoonMaker::set_arrdep_time_all_routes()
{
    for(int i = 0; i < cur_sol.sol_config.size(); i++)
    {
        TimeWindowUpdater tw_updater(cur_sol.sol_config[i], nodeset);
        tw_updater.set_arrdep_time_one_route(cur_sol.sol_config[i].route_arrtw, cur_sol.sol_config[i].route_deptw, cur_sol.sol_config[i].route_arrtime, cur_sol.sol_config[i].route_deptime);
    }
}

vector<CouplingArcSol> PlatoonMaker::get_coupling_sol()
{
    coupling_heuristic_all_arcs();
    return coupling_arcs_sol;
}

vector<vector<int>> PlatoonMaker::get_arrtw_after_platoon(int rid)
{
    return cur_sol.sol_config[rid].route_arrtw;
}

vector<vector<int>> PlatoonMaker::get_deptw_after_platoon(int rid)
{
    return cur_sol.sol_config[rid].route_deptw;
}

vector<int> PlatoonMaker::get_arrtime_after_platoon(int rid)
{
    return cur_sol.sol_config[rid].route_arrtime;
}

vector<int> PlatoonMaker::get_deptime_after_platoon(int rid)
{
    return cur_sol.sol_config[rid].route_deptime;
}

//get the set of unique arcs in the solution
vector<vector<int>> PlatoonMaker::get_unique_arcs_set()
{
    return unique_arcs_set;
}

int PlatoonMaker::get_arc_appear_times(vector<int> input_arc)
{
    int arcsize = 0;
    auto iter_config = find_if(unique_arcs_config.begin(), unique_arcs_config.end(), [&](CouplingArc x){return x.thisarc == input_arc;});
    for(int i = 0; i < (*iter_config).arc_all_pos.size(); i++)
    {
        arcsize += (*iter_config).arc_all_pos[i].size();
    }    
    return arcsize;
}

//given an arc, according to its platooning results, calculate its total energy-related distance cost of the solution
double PlatoonMaker::cal_arc_total_energy_len(vector<int> input_arc)
{
    double arc_total_energy_dist = 0;
    double arclen = nodeset.initial_distmat[input_arc[0]][input_arc[1]];  //the length of the given arc
    int arcsize = get_arc_appear_times(input_arc);  //the number of times the arc appears in the solution
    auto iter_sol = find_if(cur_sol.sol_platoons_all_arcs.begin(), cur_sol.sol_platoons_all_arcs.end(), [&](CouplingArcSol x){return x.thisarc == input_arc;});
    //discuss two cases: 1. the arc is coupled; 2. the arc is not coupled
    if(iter_sol == cur_sol.sol_platoons_all_arcs.end()) //no platoons are formed
    {
        arc_total_energy_dist = arclen * arcsize;
        return arc_total_energy_dist;
    }
    else //platoons formed
    {
        int plnum = (*iter_sol).platoons_config_on_arc.size();
        for(int p = 0; p < plnum; p++)
        {
            int plen = (*iter_sol).platoons_config_on_arc[p].size();
            arc_total_energy_dist += pl_factor(plen) * plen * arclen;
        }
        return arc_total_energy_dist;
    }
}

//get the total length of the solution -> including the total number of vehicles and total number of positions within each vehicle route
double PlatoonMaker::cal_sol_total_energy_dist()
{
    double sol_total_energy_dist = 0;
    for(int i = 0; i < unique_arcs_set.size(); i++)
    {
        sol_total_energy_dist += cal_arc_total_energy_len(unique_arcs_set[i]);
    }
    return sol_total_energy_dist;
}
