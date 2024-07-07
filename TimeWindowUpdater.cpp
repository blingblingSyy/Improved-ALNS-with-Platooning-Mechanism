#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include "TimeWindowUpdater.h"
#include "config.h"
#include "utility.h"
using namespace std;

/*TimeWindowUpdater: calculate and store the arrival and departure time windows for a given route*/
TimeWindowUpdater::TimeWindowUpdater(Route &input_route, Nodes &nodes)
{
    route = input_route;
    nodeset = nodes;
    routelen = route.extended_route.size();  //need to generate the extended route and the corresponding labels for each node in advance
    set_sertw_route();  //get the service time window for each node in the extended route
    set_st_route(); //get the service time for each node in the extended route
    set_waiting_lim(); //get the maximum waiting time limit and the waiting limit per node
    cal_threshold_nodenum(); //get the threshold number to calculate the correct waiting time limit for a path
    //resize the arrival time window and departure time window vectors 
    route.route_arrtw.resize(routelen);
    route.route_deptw.resize(routelen);
    for(int i = 0; i < routelen; i++)
    {
        route.route_arrtw[i].resize(2);
        route.route_deptw[i].resize(2);
    }
    //cal_route_tw();
}

void TimeWindowUpdater::set_sertw_route()
{
    for(int i = 0; i < routelen; i++)
    {
        int node_id = route.extended_route[i];  //label == 1: served; label == 0: bypass
        sertw_inroute[i] = (route.node_labels[i] == 1) ? nodeset.service_tw[node_id] : nodeset.travel_tw[node_id];  //nodeset.service_tw[0]
    }
}

void TimeWindowUpdater::set_st_route()
{
    for(int i = 0; i < routelen; i++)
    {
        int node_id = route.extended_route[i];
        st_inroute[i] = (route.node_labels[i] == 1) ? nodeset.service_time[node_id] : 0;  //label == 1: served; label == 0: bypass
    }
}

//pay attention to the case when the maximum waiting time is smaller than the original waiting time limit per node
void TimeWindowUpdater::set_waiting_lim()
{
    if(route.veh_type == 0)  //0 represents passenger type
    {
        wait_pernode = (WAIT_MAX < WAIT_0) ? WAIT_MAX : WAIT_0;
        wait_max = WAIT_MAX;
    }
    else    //1 represents freight type
    {
        wait_pernode = WAIT_1;
        wait_max = wait_pernode * routelen;
    }
}

//this is the threshold to decide when the waiting time per node multiplying by the number of nodes invovled is larger than the maximum waiting time limit given
//at the threshold, the waiting time per node multiplying by the number of nodes invovled is still larger than the maximum waiting time limit given;
//but when the number of nodes is smaller than the threshold, the maximum waiting time limit is larger than the waiating time limit per node multiplying by the number of nodes
//when the number of nodes on a path is larger than or equal to the threshold number, the total waiting time limit should be the maximum waiting time limit for the entire route
//when the number of nodes on a path is less than the threshold number (=threshold-1), the total waiting time limit should be the waiting time limit per node multiplying by the number of nodes involved
void TimeWindowUpdater::cal_threshold_nodenum()  
{
    threshold_nodenum = int(ceil(wait_max * 1.0 / wait_pernode)); 
}

//if not_serve_end_node = 0, used to calculate DT2; if not_serve_end_node = 1, used to calculate AT1, start from the arrival time
int TimeWindowUpdater::cal_nectime(int start_pos, int end_pos, bool not_serve_end_node) //start_pos corresponds to the position of the start node of a path
{
    int nectime = 0;
    int nec_servetime = 0;
    int nec_tvltime = cal_path_tvltime(route.extended_route, nodeset.initial_timemat, start_pos, end_pos);
    for(int i = start_pos; i < end_pos; i++)   //not_serve_end_node == 1, used to calculate AT1
    {
        nec_servetime += st_inroute[i];
    }
    if(!not_serve_end_node) //not_serve_end_node == 0, used to calculate DT2 -> need to add the service time of the end node
    {
        nec_servetime += st_inroute[end_pos];
    }
    nectime = nec_servetime + nec_tvltime;
    return nectime;
}

//calculate the maximum allowable time between two given nodes in a route
int TimeWindowUpdater::cal_maxtime(int start_pos, int end_pos, bool not_serve_end_node)
{
    int maxtime = 0;
    int nectime = cal_nectime(start_pos, end_pos, not_serve_end_node);
    int nodes_involved = end_pos - start_pos + 1;
    if(nodes_involved > threshold_nodenum)
    {
        maxtime = nectime + wait_max;
    }
    else //nodes_involved <= threshold_num
    {
        int max_waiting = nodes_involved * wait_pernode;
        maxtime = nectime + max_waiting;
    }
    return maxtime;
}

//calculate AT1 for a node in a route
int TimeWindowUpdater::cal_AT1(int node_pos, int lastnode_AT1)  
{
    int AT1;
    if(node_pos == 0)   //the starting depot
    {
        AT1 = sertw_inroute[node_pos][0];  //the starting depot does not have AT1:  nodeset.service_tw[0][0];
    }
    // else if(node_pos != routelen-1) //nodes in the middle
    else if(node_pos > 0 && node_pos <= routelen-1) //nodes in the middle, including the ending depot
    {
        //case 3:
        int startserve_last = sertw_inroute[node_pos-1][0];
        int servetime_lastnode = st_inroute[node_pos-1];
        int tvltime_lastnode = cal_path_tvltime(route.extended_route, nodeset.initial_timemat, node_pos-1, node_pos);
        int case3_AT1 = max(lastnode_AT1, startserve_last) + servetime_lastnode + tvltime_lastnode;
        // int startserve_last = st_inroute[node_pos-1];
        // int case3_AT1 = max(lastnode_AT1, startserve_last) + cal_nectime(node_pos-1, node_pos, true); //still not_serve_end_node -> no need to add the service time of this node
        AT1 = case3_AT1;
        //case 1:
        for(int i = routelen-1; i > node_pos; i--)  //case1: tracking backward from the last node to calculate AT1
        {
            // int node_id = route.extended_route[i];
            if(route.node_labels[i]) //label == 1: the referred node is a served node; only care about the start service time of the served nodes
            {
                int startserve_i = sertw_inroute[i][0]; //get the earliest allowable service start time 
                int max_time = cal_maxtime(i, node_pos, true); //to calculate AT1, not_serve_end_node is set to be true
                int case1_AT1 = startserve_i - max_time;
                if(case1_AT1 > AT1)
                {
                    AT1 = case1_AT1;
                }
            }
        }
        //case 2 (only for the served nodes):
        // int thisnode = route.extended_route[node_pos];
        if(route.node_labels[node_pos])  //label == 1: the node is a served node
        {
            int startserve_this = sertw_inroute[node_pos][0];
            int case2_AT1 = startserve_this - wait_pernode;
            if(case2_AT1 > AT1)
            {
                AT1 = case2_AT1;
            }
        }
        // else //label == 0: intermediate nodes
        // {
        //     int startpass_this = sertw_inroute[node_pos][0];
        //     if(startpass_this > AT1)
        //     {
        //         AT1 = startpass_this;
        //     }
        // }
    }
    // else    //node_pos == routelen-1 => the ending depot
    return AT1;
}

//calculate DT1 for a node in a route
int TimeWindowUpdater::cal_DT1(int node_pos, int thisnode_AT1, int nextnode_AT1)
{
    int DT1;
    // if(node_pos = 0)
    // {
    //     DT1 = nextnode_AT1 - cal_nectime(node_pos, node_pos+1, true);
    // }
    // else if(node_pos != routelen-1)
    if(node_pos <routelen-1 && node_pos >= 0) //including the starting depot
    {
        //case 1 (can be ignored)
        // int thisnode = route.extended_route[node_pos];
        int startserve_thisnode = sertw_inroute[node_pos][0];
        int servetime_thisnode = st_inroute[node_pos];
        DT1 = max(thisnode_AT1, startserve_thisnode) + servetime_thisnode;

        //case 2: ///need to check the service time windows for the intermediate nodes and the depots
        // int nextnode = route.extended_route[node_pos+1];
        int tvltime_nextnode = cal_path_tvltime(route.extended_route, nodeset.initial_timemat, node_pos, node_pos+1);
        if(nextnode_AT1 - tvltime_nextnode > DT1) //take the maximum of the two cases
        {
            DT1 = nextnode_AT1 - tvltime_nextnode;
        }
    }
    else //node_pos == routelen-1 => the ending depot
    {
        DT1 = sertw_inroute[node_pos][0];
    }
    return DT1;
}

//calculate DT2 for D node in a route
int TimeWindowUpdater::cal_DT2(int node_pos, int nextnode_DT2)
{
    int DT2;
    // if(node_pos == 0)   //the starting depot
    // else if(node_pos != routelen-1) //nodes in the middle
    if(node_pos < routelen-1 && node_pos >= 0) //nodes in the middle, including the starting depot
    {
        //case 3:
        // int thisnode = route.extended_route[node_pos];
        // int nextnode = route.extended_route[node_pos+1];
        int lateserve_next = sertw_inroute[node_pos+1][1];
        int servetime_nextnode = st_inroute[node_pos+1];
        int tvltime_nextnode = cal_path_tvltime(route.extended_route, nodeset.initial_timemat, node_pos, node_pos+1);
        int case3_DT2 = min(nextnode_DT2 - servetime_nextnode, lateserve_next) - tvltime_nextnode;
        DT2 = case3_DT2;
        //case 1:
        for(int i = 0; i < node_pos; i++)  //case1: tracking backward from the last node to calculate AT1
        {
            // int node_id = route.extended_route[i];
            if(route.node_labels[i]) //label == 1: the referred node is a served node; only care about the start service time of the served nodes
            {
                int lateserve_i = sertw_inroute[i][1]; //get the earliest allowable service start time 
                int max_time = cal_maxtime(i, node_pos, false); //to calculate DT2, not_serve_end_node is set to be false
                int case1_DT2 = lateserve_i + max_time;
                if(case1_DT2 < DT2)
                {
                    DT2 = case1_DT2;
                }
            }
        }
        //case 2 (only for the served nodes):
        // int thisnode = route.extended_route[node_pos];
        if(route.node_labels[node_pos])  //label == 1: the node is a served node
        {
            int lateserve_this = sertw_inroute[node_pos][1];
            int servetime_this = st_inroute[node_pos];
            int case2_DT2 = lateserve_this + servetime_this + wait_pernode;
            if(case2_DT2 < DT2)
            {
                DT2 = case2_DT2;
            }
        }
        // else //label == 0: intermediate nodes
        // {
        //     int latepass_this = sertw_inroute[node_pos][1];
        //     if(latepass_this < AT1)
        //     {
        //         DT2 = latepass_this;
        //     }
        // }
    }
    else    //node_pos == routelen-1 => the ending depot
    {
        DT2 = sertw_inroute[node_pos][1];
    }
    return DT2;
}

//calculate AT2 for a node in a route
int TimeWindowUpdater::cal_AT2(int node_pos, int thisnode_DT2, int lastnode_DT2)
{
    int AT2;
    if(node_pos == 0)
    {
        AT2 = sertw_inroute[node_pos][1];
    }
    else if(node_pos > 0 && node_pos <= routelen-1)
    {
        //case 1 (can be ignored):
        // int thisnode = route.extended_route[node_pos];
        int lateserve_thisnode = sertw_inroute[node_pos][0];
        int servetime_thisnode = st_inroute[node_pos];
        AT2 = min(thisnode_DT2 - servetime_thisnode, lateserve_thisnode);

        //case 2: ///need to check the service time windows for the intermediate nodes and the depots
        // int lastnode = route.extended_route[node_pos-1];
        int tvltime_lastnode = cal_path_tvltime(route.extended_route, nodeset.initial_timemat, node_pos-1, node_pos);
        if(lastnode_DT2 + tvltime_lastnode < AT2) //take the minimum of the two cases
        {
            AT2 = lastnode_DT2 + tvltime_lastnode;
        }
    }
    return AT2;
}

void TimeWindowUpdater::cal_route_tw()
{
    //AT1
    int lastnode_AT1 = cal_AT1(0, 0);
    route.route_arrtw[0][0] = lastnode_AT1;
    for(int i = 1; i < routelen; i++) //calculate AT1 and DT1
    {
        int AT1_i = cal_AT1(i, lastnode_AT1);
        route.route_arrtw[i][0] = AT1_i;
        lastnode_AT1 = AT1_i;
    }
    //DT1
    for(int i = 0; i < routelen-1; i++)
    {
        route.route_deptw[i][0] = cal_DT1(i, route.route_arrtw[i][0], route.route_arrtw[i+1][0]);
    }
    route.route_deptw[routelen-1][0] = cal_DT1(routelen-1, 0, 0);
    //DT2
    int nextnode_DT2 = cal_DT2(routelen-1, 0);
    route.route_deptw[routelen-1][1] = nextnode_DT2;
    for(int i = routelen-2; i >= 0; i--) //calculate AT1 and DT1
    {
        int DT2_i = cal_DT2(i, nextnode_DT2);
        route.route_deptw[i][1] = DT2_i;
        nextnode_DT2 = DT2_i;
    }    
    //AT2
    for(int i = routelen-1; i > 0; i--)
    {
        route.route_arrtw[i][1] = cal_AT2(i, route.route_deptw[i][1], route.route_deptw[i-1][1]);
    }
    route.route_arrtw[0][0] = cal_AT2(0, 0, 0); 
}

//set the time windows for the route
void TimeWindowUpdater::set_route_tw(vector<vector<int>> &route_deptw, vector<vector<int>> &route_arrtw)
{
    route_deptw = this->route.route_deptw;
    route_arrtw = this->route.route_arrtw;
}


//a simple getter
vector<vector<int>> TimeWindowUpdater::get_route_arrtw()
{
    return this->route.route_arrtw;
}

//a simple getter
vector<vector<int>> TimeWindowUpdater::get_route_deptw()
{
    return this->route.route_deptw;
}

//a simple getter
vector<int> TimeWindowUpdater::get_node_arrtw(int node_pos)
{
    return route.route_arrtw[node_pos];
}

//a simple getter
vector<int> TimeWindowUpdater::get_node_deptw(int node_pos)
{
    return route.route_deptw[node_pos];
}

bool TimeWindowUpdater::check_node_feasibility(int node_pos)
{
    //case 1: AT1 > AT2
    if(route.route_arrtw[node_pos][0] > route.route_arrtw[node_pos][1])
        return false;
    //case 2: DT1 > DT2
    else if(route.route_deptw[node_pos][0] > route.route_deptw[node_pos][1])
        return false;
    //case 3: AT1 > the latest start-service time (for intermediate nodes, AT1 > the latest travelable time)
    else if(route.route_arrtw[node_pos][0] > sertw_inroute[node_pos][1])
        return false;
    //case 4: DT2 < the earliest start-service time + service time (for intermediate nodes, DT2 < the earlist travelable time)
    else if(route.route_deptw[node_pos][1] < sertw_inroute[node_pos][0] + st_inroute[node_pos])
        return false;
    return true;
}

bool TimeWindowUpdater::check_route_feasibility()
{
    //check whether all nodes have feasible time windows
    for(int i = 0; i < routelen; i++)
    {
        if(!check_node_feasibility(i))
            return false;
    }
    return true;
}

vector<int> TimeWindowUpdater::get_infeasible_nodepos()
{
    //get the nodes with infeasible time windows
    vector<int> infeasible_nodepos;
    for(int i = 0; i < routelen; i++)
    {
        if(!check_node_feasibility(i))
            infeasible_nodepos.push_back(i);
    }
    return infeasible_nodepos;
}

//calculate the actual waiting time spent at a given node in a route
int TimeWindowUpdater::cal_waiting_pernode(int nodepos, int arrtime_pos, int deptime_pos)
{
    int wait_thisnode;
    if(route.node_labels[nodepos] == 1) //served node with service time window
    {
        int thisnode = route.extended_route[nodepos];
        int earliest_servicetime = nodeset.service_tw[thisnode][0];
        int servetime_thisnode = nodeset.service_time[thisnode];
        int wait_beforeserve = max(0, earliest_servicetime - arrtime_pos);
        int wait_afterserve = deptime_pos - (max(arrtime_pos, earliest_servicetime) + servetime_thisnode);
        wait_thisnode = wait_afterserve + wait_afterserve;
    }
    else //bypassed node without service time window and service time
    {
        wait_thisnode = deptime_pos - arrtime_pos;
    }
    return wait_thisnode;
}

//calibrate the waiting time for a given number of nodes provided that some waiting time has been used in other nodes
int TimeWindowUpdater::calib_wait_max(int used_waiting, int uncal_nodes)
{
    int modified_waitmax = wait_max - used_waiting;
    int total_wait = uncal_nodes * this->wait_pernode;
    if(total_wait > modified_waitmax)
    {
        total_wait = modified_waitmax;
    }
    return total_wait;
}

//modify the AT1 and DT1 for a route given a modified departure time window for a node //..., vector<vector<int>> &route_deptw, vector<vector<int>> &route_arrtw
void TimeWindowUpdater::modify_route_tw_AT1_DT1(int arc_start_pos_input, int arc_end_pos_input, vector<vector<int>> original_arrtw_input)
{
    //first calculate AT1 forward from the start position -> move as fast as possible based on the overlapped earliest departure time
    for(int i = arc_end_pos_input+1; i < routelen; i++)
    {
        int startserve_lastnode = sertw_inroute[i-1][0];
        int arrtime_lastnode = route.route_arrtw[i-1][0];
        route.route_arrtw[i][0] = max(max(startserve_lastnode, arrtime_lastnode) + cal_nectime(i-1, i, true), original_arrtw_input[i][0]);
    }
    //then calculate DT1 based on AT1 for the nodes after the arc_end_pos (including the arc_end_pos)
    for(int i = routelen-1; i > arc_end_pos_input; i--)
    {
        int arc_dist_i_AT1 = cal_path_tvltime(route.extended_route, nodeset.initial_timemat, i-1, i);
        route.route_deptw[i-1][0] = route.route_arrtw[i][0] - arc_dist_i_AT1;
    }
    //next calculate the remaining waiting time for the nodes after the arc_end_pos (including the arc_end_pos)
    int waiting_after_arc_end_pos = 0;
    for(int i = arc_end_pos_input; i < routelen-1; i++) //exclude the ending depot
    {
        waiting_after_arc_end_pos += cal_waiting_pernode(i, route.route_arrtw[i][0], route.route_deptw[i][0]);
    }
    //then calculate AT1 backward using the remaining waiting time limit
    for(int i = arc_start_pos_input; i > 0; i--) //arc_start_pos = arc_end_pos-1
    {
        int total_wait_max_AT1 = calib_wait_max(wait_max - waiting_after_arc_end_pos, arc_end_pos_input - i);
        route.route_arrtw[i][0] = max(route.route_arrtw[arc_end_pos_input][0] - cal_nectime(i, arc_end_pos_input, true) - total_wait_max_AT1, original_arrtw_input[i][0]);
    }
    //next calculate DT1 based on AT1 for the nodes before the arc_end_pos (excluding the arc_end_pos)
    for(int i = arc_start_pos_input-1; i >= 0; i--) //arc_start_pos = arc_end_pos-1
    {
        int arc_dist_i_DT1 = cal_path_tvltime(route.extended_route, nodeset.initial_timemat, i, i+1);
        route.route_deptw[i][0] = route.route_arrtw[i+1][0] - arc_dist_i_DT1;
    }
}

//modify the DT2 and AT2 for a route given a modified departure time window for a node //..., vector<vector<int>> &route_deptw, vector<vector<int>> &route_arrtw
void TimeWindowUpdater::modify_route_tw_DT2_AT2(int arc_start_pos_input, int arc_end_pos_input, vector<vector<int>> original_deptw_input)
{
    //first calculate DT2 backward from the start position -> move reversely as fast as possible based on the overlapped latest departure time
    for(int i = arc_start_pos_input-1; i >= 0; i--)
    {
        int lateserve_nextnode = sertw_inroute[i+1][1];
        int servetime_nextnode = st_inroute[i+1];
        int deptime_nextnode = route.route_deptw[i+1][1];
        int arc_dist_i_DT2 = cal_path_tvltime(route.extended_route, nodeset.initial_timemat, i, i+1);
        route.route_deptw[i][1] = min(min(deptime_nextnode - servetime_nextnode, lateserve_nextnode) - arc_dist_i_DT2, original_deptw_input[i][1]);
    }
    //then calculate AT2 based on DT2 for the nodes before the arc_start_pos (including the arc_start_pos)
    for(int i = 0; i < arc_start_pos_input; i++)
    {
        int arc_dist_i_AT2 = cal_path_tvltime(route.extended_route, nodeset.initial_timemat, i, i+1);
        route.route_arrtw[i+1][1] = route.route_deptw[i][1] + arc_dist_i_AT2;
    }
    //next calculate the remaining waiting time for the nodes before the arc_start_pos (including the arc_start_pos)
    int waiting_before_arc_start_pos = 0;
    for(int i = 1; i <= arc_start_pos_input; i++) //exclude the ending depot
    {
        waiting_before_arc_start_pos += cal_waiting_pernode(i, route.route_arrtw[i][1], route.route_deptw[i][1]);
    }
    //then calculate DT2 forward using the remaining waiting time limit
    for(int i = arc_end_pos_input; i < routelen-1; i++) //arc_end_pos = arc_start_pos + 1
    {
        int total_wait_max_DT2 = calib_wait_max(wait_max - waiting_before_arc_start_pos, i - arc_start_pos_input);
        route.route_deptw[i][1] = min(route.route_arrtw[arc_end_pos_input][1] + cal_nectime(arc_end_pos_input, i, false) + total_wait_max_DT2, original_deptw_input[i][1]);  //not_serve_end_node = false -> include the service time for the ending node of a given path
    }
    //next calculate AT2 based on DT2 for the nodes after the arc_end_pos (excluding the arc_end_pos)
    for(int i = arc_end_pos_input+1; i < routelen; i++) //arc_start_pos = arc_end_pos-1
    {
        int arc_dist_i_AT2 = cal_path_tvltime(route.extended_route, nodeset.initial_timemat, i-1, i);
        route.route_arrtw[i][1] = route.route_deptw[i-1][1] + arc_dist_i_AT2;
    }
}


//modify the arrival and departure time windows for a route given a modified departure time window for a node
void TimeWindowUpdater::modify_route_tw(int arc_start_pos, vector<int> overlap_dep_tw) //int arc_end_pos
{
    int arc_end_pos = arc_start_pos + 1;
    //1. first store the original arrival and departure time windows and clear the arrival and departure time windows
    vector<vector<int>> original_arrtw = route.route_arrtw;
    vector<vector<int>> original_deptw = route.route_deptw;
    clear_route_tw();
    
    //2. assign the overlapped departure time windows and arrival time windows of the given arc
    route.route_deptw[arc_start_pos] = overlap_dep_tw;
    int arc_tvltime = cal_path_tvltime(route.extended_route, nodeset.initial_timemat, arc_start_pos, arc_end_pos);
    route.route_arrtw[arc_end_pos] = {overlap_dep_tw[0] + arc_tvltime, overlap_dep_tw[1] + arc_tvltime};
    route.route_arrtw[0] = sertw_inroute[0]; //arrival time windows for the starting depot
    route.route_deptw[routelen-1] = sertw_inroute[routelen-1]; //departure time windows for the ending depot
    
    //3. AT1 & DT1 -> void modify_route_tw_AT1_DT1()
    modify_route_tw_AT1_DT1(arc_start_pos, arc_end_pos, original_arrtw);

    //4. AT2 & DT2 -> void modify_route_tw_DT2_AT2()
    modify_route_tw_DT2_AT2(arc_start_pos, arc_end_pos, original_deptw);
}

void TimeWindowUpdater::clear_route_tw()
{
    this->route.route_arrtw.clear();
    this->route.route_deptw.clear();
}

//calculate the actual arrival and departure time for a route
//the purpose is to find a set of arrival and departure time with the minimum trip duration
void TimeWindowUpdater::set_arrdep_time_one_route(vector<vector<int>> final_arrtw, vector<vector<int>> final_deptw, vector<int> &arrtime_vec, vector<int> &deptime_vec)
{
    // vector<vector<int>> arrdep_time_thisroute(final_arrtw.size()); //{{arrtime1, arrtime2, ...}, {deptime1, deptime2, ...}}

    //rule 1
    arrtime_vec[0] = -1;
    deptime_vec[0] = final_deptw[0][1]; //the latest departure time of the first node
    for(int i = 1; i < final_arrtw.size(); i++)
    {
        arrtime_vec[i] = deptime_vec[i-1] + nodeset.initial_timemat[i-1][i]; //arrival time of the node
        deptime_vec[i] = max(arrtime_vec[i] + st_inroute[i], final_deptw[i][0]); //departure time of the node
    }
    deptime_vec[final_arrtw.size()-1] = -1;
    
    // //rule 2
    // deptime_vec[routelen-1] = -1;
    // arrtime_vec[routelen-1] = final_arrtw[routelen-1][0]; //the earliest arrival time of the last node
    // for(int i = routelen-2; i >= 0; i--)
    // {
    //     deptime_vec[i] = arrtime_vec[i+1] - nodeset.initial_timemat[i][i+1]; //departure time of the node
    //     arrtime_vec[i] = min(deptime_vec[i] - st_inroute[i], final_arrtw[i][1]); //arrival time of the node
    // }
    // arrtime_vec[0] = -1;

    // return arrdep_time_thisroute;
}

int TimeWindowUpdater::route_total_trip_duration(int arrtime_lastnode, int deptime_firstnode)
{
    return arrtime_lastnode - deptime_firstnode;
}

int TimeWindowUpdater::route_total_wait_time(int arrtime_lastnode, int deptime_firstnode)
{
    int route_total_tripdur = route_total_trip_duration(arrtime_lastnode, deptime_firstnode);
    int nec_tripdur = cal_nectime(0, routelen-1, true);
    int route_wait_time = route_total_tripdur - nec_tripdur;
    return route_wait_time;
}