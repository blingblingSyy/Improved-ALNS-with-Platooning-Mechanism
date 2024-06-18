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

//if backward = 0, used to calculate DT2; if backward = 1, used to calculate AT1
int TimeWindowUpdater::cal_nectime(int start_pos, int end_pos, bool backward) //start_pos corresponds to the position of the start node of a path
{
    int nectime = 0;
    int nec_servetime = 0;
    int nec_tvltime = cal_path_tvltime(route.extended_route, nodeset.initial_timemat, start_pos, end_pos);
    for(int i = start_pos; i < end_pos; i++)   //backward == 1, used to calculate AT1
    {
        nec_servetime += st_inroute[i];
    }
    if(!backward) //backward == 0, used to calculate DT2 -> need to add the service time of the end node
    {
        nec_servetime += st_inroute[end_pos];
    }
    nectime = nec_servetime + nec_tvltime;
    return nectime;
}

//calculate the maximum allowable time between two given nodes in a route
int TimeWindowUpdater::cal_maxtime(int start_pos, int end_pos, bool backward)
{
    int maxtime = 0;
    int nectime = cal_nectime(start_pos, end_pos, backward);
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
        // int case3_AT1 = max(lastnode_AT1, startserve_last) + cal_nectime(node_pos-1, node_pos, true); //still backward -> no need to add the service time of this node
        AT1 = case3_AT1;
        //case 1:
        for(int i = routelen-1; i > node_pos; i--)  //case1: tracking backward from the last node to calculate AT1
        {
            // int node_id = route.extended_route[i];
            if(route.node_labels[i]) //label == 1: the referred node is a served node; only care about the start service time of the served nodes
            {
                int startserve_i = sertw_inroute[i][0]; //get the earliest allowable service start time 
                int max_time = cal_maxtime(i, node_pos, true); //to calculate AT1, backward is set to be true
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
                int max_time = cal_maxtime(i, node_pos, false); //to calculate DT2, backward is set to be false
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

//a simple getter
vector<vector<int>> TimeWindowUpdater::get_route_arrtw()
{
    return route.route_arrtw;
}

//a simple getter
vector<vector<int>> TimeWindowUpdater::get_route_deptw()
{
    return route.route_deptw;
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
    else
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

