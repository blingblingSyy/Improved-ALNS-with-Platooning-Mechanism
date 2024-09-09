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
#include <assert.h>
#include "example/model/establish/VRPSolution.h"
#include "example/model/establish/APlatoon.h"
#include "example/model/establish/ARoute.h"
#include "example/model/basic/Nodes.h"
#include "example/model/establish/TimeWindowUpdater.h"
#include "example/model/establish/PlatoonMaker.h"
#include "example/model/basic/config.h"
#include "utility.h"
using namespace std;  

PlatoonMaker::PlatoonMaker(VRPSolution& sol, Nodes& nodes)
{
    cur_sol = &sol;
    nodeset = &nodes;
    routes_num = cur_sol->getRoutesNum();
    setAllArcBasedExtendedRoutes();  //set arcs_based_extended_routes
    findAllUniqueArcs(); //set unique_arcs_set
    unique_arcs_num = unique_arcs_set.size();
    unique_arcs_occurance.resize(unique_arcs_num);
    findCommonVehs(); //set unique_arcs_occurance.common_vehs & unique_arcs_occurance.common_routes
    findArcPosAllRoutes(); //set unique_arcs_occurance.arc_all_pos
    pivotArc_all_routes.resize(routes_num, -1);
}

PlatoonMaker::~PlatoonMaker()
{
    delete cur_sol;
    delete nodeset;
    for(int i = 0; i < all_valid_platoons.size(); i++)
    {
        delete all_valid_platoons[i];
    }
}

vector<vector<int>> PlatoonMaker::buildArcBasedRoute(vector<int> input_extended_route)
{
    vector<vector<int>> arcs_based_route;
    int extendroutelen = input_extended_route.size();
    for(int i = 0; i < extendroutelen-1; i++)
    {
        arcs_based_route.push_back({input_extended_route[i], input_extended_route[i+1]});
    }
    return arcs_based_route;
}

void PlatoonMaker::setAllArcBasedExtendedRoutes()
{
    for(int i = 0; i < routes_num; i++)
    {
        arcs_based_extended_routes.push_back(buildArcBasedRoute(cur_sol->getOneRoute(i)->getExtendedRoute()));
    }
}

void PlatoonMaker::findAllUniqueArcs()
{
    for(int i = 0; i < routes_num; i++)
    {
        //add the arcs of the current route to the unique_arcs_set
        unique_arcs_set.insert(unique_arcs_set.end(), arcs_based_extended_routes[i].begin(), arcs_based_extended_routes[i].end());
    }
    //find the unique arc set of all routes
    set<vector<int>> unique_arcs_all_routes(unique_arcs_set.begin(), unique_arcs_set.end());
    unique_arcs_set.assign(unique_arcs_all_routes.begin(), unique_arcs_all_routes.end());
}

void PlatoonMaker::findCommonVehs()
{
    for(int u = 0; u < unique_arcs_num; u++)
    {
        unique_arcs_occurance[u].thisarc = unique_arcs_set[u];
        for(int r = 0; r < routes_num; r++)
        {
            if(find(arcs_based_extended_routes[r].begin(), arcs_based_extended_routes[r].end(), unique_arcs_set[u]) != arcs_based_extended_routes[r].end())
            {
                unique_arcs_occurance[u].common_vehs.push_back(cur_sol->getOneRoute(r)->getVehID());
                unique_arcs_occurance[u].common_routes.push_back(r);
            }
        }
    }
}

void PlatoonMaker::findArcPosAllRoutes() //it is possible that an arc appears more than one times in the same route
{
    for(int u = 0; u < unique_arcs_num; u++)
    {
        int common_vehs_num = unique_arcs_occurance[u].common_routes.size();
        unique_arcs_occurance[u].arc_all_pos.resize(common_vehs_num);
        for(int i = 0; i < common_vehs_num; i++)
        {
            int rid = unique_arcs_occurance[u].common_routes[i];
            vector<vector<int>>::iterator it;
            it = find(arcs_based_extended_routes[rid].begin(), arcs_based_extended_routes[rid].end(), unique_arcs_set[u]);
            //find all positions of the arc in the route r
            while(it != arcs_based_extended_routes[rid].end())
            {
                int arc_pos = it - arcs_based_extended_routes[rid].begin();
                unique_arcs_occurance[u].arc_all_pos[i].push_back(arc_pos);
                unique_arcs_occurance[u].pairwise_feas_graph_nodeset.push_back(make_pair(rid, arc_pos));
                it = find(it+1, arcs_based_extended_routes[rid].end(), unique_arcs_set[u]);
            }
        }
    }
}

bool PlatoonMaker::checkPairConnectivity(pair<int, int> route_pos1, pair<int, int> route_pos2, vector<ARoute*>& sol_config_copy)
{
    int routeid1 = route_pos1.first;
    int arc_pos1 = route_pos1.second;
    int routeid2 = route_pos2.first;
    int arc_pos2 = route_pos2.second;
    vector<int> arc1 = arcs_based_extended_routes[routeid1][arc_pos1];
    vector<int> arc2 = arcs_based_extended_routes[routeid2][arc_pos2];
    if(arc1 != arc2)
    {
        return false;
    }
    if(routeid1 == routeid2)
    {
        return false;
    }
    else //routeid1 != routeid2
    {
        vector<int> deptw_veh_arc1 = sol_config_copy[routeid1]->getExpectedDepTW(arc_pos1);
        vector<int> deptw_veh_arc2 = sol_config_copy[routeid2]->getExpectedDepTW(arc_pos2);
        vector<int> overlap_tw_veh_arc = {max(deptw_veh_arc1[0], deptw_veh_arc2[0]), min(deptw_veh_arc1[1], deptw_veh_arc2[1])};
        if(overlap_tw_veh_arc[0] > overlap_tw_veh_arc[1])
        {
            return false;
        }
        return true;
    }
}

vector<pair<int, int>> PlatoonMaker::buildFullGraphNodesSet(OccuranceOneArc input_arc_config)
{
    return input_arc_config.pairwise_feas_graph_nodeset;
}

vector<vector<bool>> PlatoonMaker::buildPairwiseFeasGraph(vector<pair<int, int>>& graph_nodes, vector<ARoute*>& sol_config_copy)
{
    int graph_nodes_size = graph_nodes.size();
    vector<vector<bool>> pairwise_feasibility_graph(graph_nodes_size);
    for(int i = 0; i < graph_nodes_size; i++)
    {
        pairwise_feasibility_graph[i].resize(graph_nodes_size);
        for(int j = i; j < graph_nodes_size; j++) //case: j >= i
        {
            pairwise_feasibility_graph[i][j] = checkPairConnectivity(graph_nodes[i], graph_nodes[j], sol_config_copy);
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

vector<int> PlatoonMaker::processNodeNeighborsInSet(int input_u, vector<int> input_set, vector<vector<bool>>& pair_feas_graph, bool keep)
{
    vector<int> input_set_copy = input_set;
    for(int i = 0; i < input_set_copy.size(); i++) //if input_set_copy is empty, then the for cycle will not be executed
    {
        if(!keep) //remove the neighbourhood (keep == 0)
        {
            if(pair_feas_graph[input_u][input_set_copy[i]]) //if u and Pset[i] are connected. If u = Pset[i], they are unconnected
            {
                input_set_copy[i] = -1;
            }
        }
        else //keep the neighbourhood (keep == 1), remove the elements that are not neighbourhood
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

void PlatoonMaker::BronKerbosch(vector<int> Rset, vector<int> Pset, vector<int> Xset, vector<vector<bool>>& pair_feas_graph, vector<vector<int>>& maximal_cliques_set)
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
        vector<int> P_erase_u_neighbour = processNodeNeighborsInSet(u, Pset, pair_feas_graph, false); //P \ N(u)
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
                vector<int> P_keep_v_neighbour = processNodeNeighborsInSet(v, Pset, pair_feas_graph, true); //Pset1 = N(v) in Pset (upper layer of Pset)
                vector<int> X_keep_v_neighbour = processNodeNeighborsInSet(v, Xset, pair_feas_graph, true); //Xset1 = N(v) in Xset (upper layer of Xset)
                BronKerbosch(Rset1, P_keep_v_neighbour, X_keep_v_neighbour, pair_feas_graph, maximal_cliques_set);
                Pset.erase(remove(Pset.begin(), Pset.end(), v), Pset.end()); //Pset = Pset \ {v};
                Xset.push_back(v); //Xset = Xset + {v}
            }
        }
    }
}

vector<pair<int, int>> PlatoonMaker::transformIDsetToNodeset(vector<int>& clique_idset, vector<pair<int, int>>& graph_nodes)
{
    vector<pair<int, int>> PFGnodeset;
    for(int i = 0; i < clique_idset.size(); i++)
    {
        PFGnodeset.push_back(graph_nodes[clique_idset[i]]);
    }
    return PFGnodeset;
}


vector<vector<int>> PlatoonMaker::findMaximalCliquesIDset(vector<pair<int, int>>& graph_nodes, vector<vector<bool>>& pair_feas_graph)
{
    vector<vector<int>> maximal_cliques_id_set;
    vector<int> init_Rset = {}, init_Pset, init_Xset = {};
    for(int i = 0; i < graph_nodes.size(); i++)
    {
        init_Pset.push_back(i); //Pset = {all v's}
    }
    BronKerbosch(init_Rset, init_Pset, init_Xset, pair_feas_graph, maximal_cliques_id_set);
    return maximal_cliques_id_set;
}

vector<APlatoon*> PlatoonMaker::findMaximalCliquesNodeset(vector<pair<int, int>>& graph_nodes, vector<vector<bool>>& pair_feas_graph)
{
    vector<APlatoon*> all_maximal_cliques;
    vector<vector<int>> maximal_cliques_id_set;
    vector<int> init_Rset = {}, init_Pset, init_Xset = {};
    for(int i = 0; i < graph_nodes.size(); i++)
    {
        init_Pset.push_back(i); //Pset = {all v's}
    }
    BronKerbosch(init_Rset, init_Pset, init_Xset, pair_feas_graph, maximal_cliques_id_set);
    //transform id set to pair<int, int> set
    all_maximal_cliques.resize(maximal_cliques_id_set.size());
    for(int i = 0; i < maximal_cliques_id_set.size(); i++)
    {
        vector<pair<int, int>> platoon_config = transformIDsetToNodeset(maximal_cliques_id_set[i], graph_nodes);
        all_maximal_cliques[i]->setArc(arcs_based_extended_routes[platoon_config[0].first][platoon_config[0].second]);
        all_maximal_cliques[i]->setConfig(platoon_config);
    }
    return all_maximal_cliques;
}

//! original findMaximalCliquesNodeset()
// vector<vector<pair<int, int>>> PlatoonMaker::findMaximalCliquesNodeset(vector<pair<int, int>>& graph_nodes, vector<vector<bool>>& pair_feas_graph)
// {
//     vector<vector<int>> maximal_cliques_id_set;
//     vector<int> init_Rset = {}, init_Pset, init_Xset = {};
//     for(int i = 0; i < graph_nodes.size(); i++)
//     {
//         init_Pset.push_back(i); //Pset = {all v's}
//     }
//     BronKerbosch(init_Rset, init_Pset, init_Xset, pair_feas_graph, maximal_cliques_id_set);
//     //transform id set to pair<int, int> set
//     vector<vector<pair<int, int>>> maximal_cliques_set;
//     for(int i = 0; i < maximal_cliques_id_set.size(); i++)
//     {
//         maximal_cliques_set.push_back(transformIDsetToNodeset(maximal_cliques_id_set[i], graph_nodes));
//     }
//     return maximal_cliques_set;
// }

vector<int> PlatoonMaker::findMaxCliqueSizeIDset(vector<vector<int>> all_maximal_cliques_idset)
{   
    assert(all_maximal_cliques_idset.size() != 0);
    int max_clique_id = 0;
    int max_clique_size = all_maximal_cliques_idset[max_clique_id].size();
    for(int i = 1; i < all_maximal_cliques_idset.size(); i++)
    {
        if(all_maximal_cliques_idset[i].size() > max_clique_size)
        {
            max_clique_id = i;
            max_clique_size = all_maximal_cliques_idset[max_clique_id].size();
        }
    }
    return all_maximal_cliques_idset[max_clique_id];
}

APlatoon* PlatoonMaker::findMaxCliqueSavingNodeset(vector<APlatoon*> all_maximal_cliques)
{
    assert(all_maximal_cliques.size() != 0);
    int max_clique_id = 0;
    int max_clique_saving = all_maximal_cliques[max_clique_id]->calEnergySaving();
    for(int i = 1; i < all_maximal_cliques.size(); i++)
    {
        if(all_maximal_cliques[i]->calEnergySaving() > max_clique_saving)
        {
            max_clique_id = i;
            max_clique_saving = all_maximal_cliques[i]->getEnergySaving();
        }
    }
    return all_maximal_cliques[max_clique_id];
}

vector<int> PlatoonMaker::calOverlapDeptwOnePlatoon(vector<pair<int, int>> platoon_config_on_arc) 
{
    vector<int> overlapped_tw(2);
    int init_rid = platoon_config_on_arc[0].first;
    int init_start_nodepos = platoon_config_on_arc[0].second;
    overlapped_tw = cur_sol->getOneRoute(init_rid)->getExpectedDepTW(init_start_nodepos);
    for(int i = 1; i < platoon_config_on_arc.size(); i++)
    {
        int rid = platoon_config_on_arc[i].first;
        int start_nodepos = platoon_config_on_arc[i].second;
        vector<int> arc_deptw_rid = cur_sol->getOneRoute(rid)->getExpectedDepTW(start_nodepos);
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

vector<int> PlatoonMaker::shrinkOverlapDeptwOnePlatoon(APlatoon* input_platoon, vector<int> overlap_deptw)
{
    vector<pair<int, int>> platoon_config = input_platoon->getPlatoonConfig();
    vector<int> updated_overlap_tw(2);
    vector<int> used_pivot_arcpos;
    vector<int> temp_deptime(platoon_config.size());
    for(int i = 0; i < platoon_config.size(); i++)
    {
        used_pivot_arcpos.push_back(pivotArc_all_routes[platoon_config[i].first]);
    }
    for(int r = 0; r < platoon_config.size(); r++)
    {
        int pivot_arcpos_thisroute = pivotArc_all_routes[platoon_config[r].first];
        if(pivot_arcpos_thisroute != -1)
        {
            int arcpos_thisroute = platoon_config[r].second;
            if(arcpos_thisroute < pivot_arcpos_thisroute)
            {
                temp_deptime[r] = overlap_deptw[1];
            }
            else //! >=
            {
                temp_deptime[r] = overlap_deptw[0];
            }
        }
        else if(pivot_arcpos_thisroute == -1)
        {
            temp_deptime[r] = int(calAverage(overlap_deptw));
            pivotArc_all_routes[platoon_config[r].first] = platoon_config[r].second;
        }
    }
    updated_overlap_tw[0] = int(calAverage(temp_deptime));
    updated_overlap_tw[1] = updated_overlap_tw[0];
    input_platoon->setOverlapDTW(updated_overlap_tw);
    return updated_overlap_tw;
}

void PlatoonMaker::updateArrDeptwAllCouplingRoutes(vector<pair<int, int>>& platoon_config_on_arc, vector<int> maximum_platoon_tw, vector<ARoute*>& sol_config_copy)
{
    if(!maximum_platoon_tw.empty())
    {
        for(int i = 0; i < platoon_config_on_arc.size(); i++)
        {
            int rid = platoon_config_on_arc[i].first;
            int arc_start_pos = platoon_config_on_arc[i].second;
            //! only update the departure time windows of the routes used in the platoon
            TimeWindowUpdater twupdater(sol_config_copy[rid]->getExtendedRoute(), sol_config_copy[rid]->getNodeLables(), sol_config_copy[rid]->getRouteWaitTimeLimitPerNode(), sol_config_copy[rid]->getRouteWaitMaxLimit(), *nodeset);
            twupdater.CalibRouteTW(arc_start_pos, maximum_platoon_tw);
            sol_config_copy[rid]->updateRouteExpectedArrTW(twupdater.getRouteArrTW());
            sol_config_copy[rid]->updateRouteExpectedDepTW(twupdater.getRouteDepTW());
        }
    }
}

vector<APlatoon*> PlatoonMaker::findAllPlatoons(vector<pair<int, int>>& graph_nodes, vector<vector<bool>>& pair_feas_graph)
{
    vector<APlatoon*> all_platoons; //! exclude the platoons with length of 1
    // vector<ARoute*> sol_config_copy = cur_sol->getAllRoutes();
    while(!graph_nodes.empty())
    {
        vector<APlatoon*> all_maximal_cliques = findMaximalCliquesNodeset(graph_nodes, pair_feas_graph);
        APlatoon* maximum_clique = findMaxCliqueSavingNodeset(all_maximal_cliques);
        //! output the overlapped departure time windows of each platoon
        vector<int> maximum_platoon_tw = calOverlapDeptwOnePlatoon(maximum_clique->getPlatoonConfig());
        vector<int> updated_max_platoon_tw = shrinkOverlapDeptwOnePlatoon(maximum_clique, maximum_platoon_tw);
        all_platoons.push_back(maximum_clique);
        //！ update time windows with the maximum platoon
        vector<pair<int, int>> maxclique_config = maximum_clique->getPlatoonConfig();
        updateArrDeptwAllCouplingRoutes(maxclique_config, updated_max_platoon_tw, cur_sol->getAllRoutes());
        //！ remove all the nodes corresponding to the nodes in the maximum clique
        process_intersections(graph_nodes, maxclique_config);
        //！ remove all the links adjacent to the nodes in the maximum clique
        if(!graph_nodes.empty())
        {
            pair_feas_graph = buildPairwiseFeasGraph(graph_nodes, cur_sol->getAllRoutes()); //sol_config_copy
        }
    }
    return all_platoons;
}

double PlatoonMaker::calEnergySavingAllPlatoonsOneArc(vector<int> thisarc, vector<pair<int, int>>& graph_nodes, vector<vector<pair<int, int>>>& all_cliques_one_arc)
{
    int nodes_size = graph_nodes.size();
    int platoon_size = all_cliques_one_arc.size();  //calculate the number of platoons formed on the given arc
    double arc_len = nodeset->getInitialDist()[thisarc[0]][thisarc[1]];
    return 0.1*(nodes_size - platoon_size)*arc_len;
}

vector<APlatoon*> PlatoonMaker::CouplingModuleOneArc(OccuranceOneArc input_arc_config)
{
    vector<pair<int, int>> graph_nodes_init = buildFullGraphNodesSet(input_arc_config);
    vector<pair<int, int>> graph_nodes = graph_nodes_init;
    vector<vector<bool>> pair_feas_graph = buildPairwiseFeasGraph(graph_nodes_init, cur_sol->getAllRoutes());
    //all platoons configuration
    vector<APlatoon*> all_platoons_one_arc = findAllPlatoons(graph_nodes, pair_feas_graph); //graph nodes will change
    return all_platoons_one_arc;
}

void PlatoonMaker::CouplingHeuristic()
{
    vector<pair<int, int>> all_graph_nodes;
    for(int i = 0; i < unique_arcs_num; i++)
    {
        all_graph_nodes.insert(all_graph_nodes.end(), unique_arcs_occurance[i].pairwise_feas_graph_nodeset.begin(), unique_arcs_occurance[i].pairwise_feas_graph_nodeset.end());
    }
    vector<vector<bool>> pair_feas_graph = buildPairwiseFeasGraph(all_graph_nodes, cur_sol->getAllRoutes());
    //all platoons configuration
    all_valid_platoons = findAllPlatoons(all_graph_nodes, pair_feas_graph);
}

//before setArrDepTimeAllRoutes(), do CouplingHeuristic() to get the updated route_arrtw and route_deptw
void PlatoonMaker::setArrDepTimeAllRoutes()
{
    for(int i = 0; i < routes_num; i++)
    {
        cur_sol->getOneRoute(i)->calArrDepTime();
    }
}

vector<APlatoon*> PlatoonMaker::getAllPlatoons()
{
    CouplingHeuristic();
    return all_valid_platoons;
}

vector<APlatoon*> PlatoonMaker::getAllPlatoonsOneArc(vector<int> input_arc)
{
    vector<APlatoon*> all_platoons_one_arc;
    for(int i = 0; i < all_valid_platoons.size(); i++)
    {
        if(all_valid_platoons[i]->getArc() == input_arc)
        {
            all_platoons_one_arc.push_back(all_valid_platoons[i]);
        }
    }
    return all_platoons_one_arc;
}


vector<vector<int>> PlatoonMaker::getArrtwAfterPlatooning(int rid)
{
    return cur_sol->getOneRoute(rid)->getRouteExpectedArrTW();
}

vector<vector<int>> PlatoonMaker::getDeptwAfterPlatooning(int rid)
{
    return cur_sol->getOneRoute(rid)->getRouteExpectedDepTW();
}

vector<int> PlatoonMaker::getArrTimeAfterPlatooning(int rid)
{
    return cur_sol->getOneRoute(rid)->getFinalArrTime();
}

vector<int> PlatoonMaker::getDepTimeAfterPlatooning(int rid)
{
    return cur_sol->getOneRoute(rid)->getFinalDepTime();
}

vector<vector<int>> PlatoonMaker::getUniqueArcsSet()
{
    return unique_arcs_set;
}

int PlatoonMaker::getArcShowTimes(vector<int> input_arc)
{
    auto iter_config = find_if(unique_arcs_occurance.begin(), unique_arcs_occurance.end(), [&](OccuranceOneArc x){return x.thisarc == input_arc;});
    return (*iter_config).pairwise_feas_graph_nodeset.size();
}

double PlatoonMaker::calSolTotalEnergySaving()
{
    double total_energy_saving = 0;
    for(int i = 0; i < all_valid_platoons.size(); i++)
    {
        total_energy_saving += all_valid_platoons[i]->getEnergySaving();
    }
    return total_energy_saving;
}

//get the total length of the solution -> including the total number of vehicles and total number of positions within each vehicle route
double PlatoonMaker::calSolTotalEnergyDist()
{
    return cur_sol->calTotalDist() - calSolTotalEnergySaving();
}
