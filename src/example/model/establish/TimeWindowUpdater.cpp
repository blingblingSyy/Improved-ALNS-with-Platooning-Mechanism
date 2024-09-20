#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include <assert.h>
#include "example/model/establish/TimeWindowUpdater.h"
#include "example/model/basic/Nodes.h"
#include "example/model/basic/config.h"
#include "utility.h"

#define NDEBUG

using namespace std;

TimeWindowUpdater::TimeWindowUpdater(vector<int> extended_route, vector<int> node_labels, int WaitTimePerNode, int WaitMaxRoute, Nodes& nodes)
{
    this->extended_route = extended_route;
    this->node_labels = node_labels;
    this->wait_max = WaitMaxRoute;
    this->wait_pernode = (WaitMaxRoute < WaitTimePerNode) ? WaitMaxRoute : WaitTimePerNode;
    this->nodeset = &nodes;
    this->initial_timemat = nodeset->getInitialTime();
    extendroutelen = extended_route.size();
    setServiceTW();
    setServiceTime();
}

TimeWindowUpdater::~TimeWindowUpdater()
{
    clear_route_tw();
    clear_route_time();
    // delete nodeset;
}

void TimeWindowUpdater::initTimeElements()
{
    route_arrtw.resize(extended_route.size());
    route_deptw.resize(extended_route.size());
    route_arrtime.resize(extended_route.size());
    route_deptime.resize(extended_route.size());
    for(int i = 0; i < extended_route.size(); i++)
    {
        route_arrtw[i].resize(2, -1);
        route_deptw[i].resize(2, -1);
        route_arrtime[i] = -1;
        route_deptime[i] = -1;
    }
}

void TimeWindowUpdater::setServiceTW()
{
    sertw_inroute.resize(extendroutelen);
    for(int i = 0; i < extendroutelen; i++)
    {
        int node_id = extended_route[i];  //label == 1: served; label == 0: bypass
        sertw_inroute[i] = (node_labels[i] == 1) ? nodeset->getServiceTW(node_id) : nodeset->getTravelTW(node_id);  //nodeset->service_tw[0]
    }
}

void TimeWindowUpdater::setServiceTime()
{
    st_inroute.resize(extendroutelen);
    for(int i = 0; i < extendroutelen; i++)
    {
        int node_id = extended_route[i];
        st_inroute[i] = (node_labels[i] == 1) ? nodeset->getServiceTime(node_id) : 0;  //label == 1: served; label == 0: bypass
    }
}

int TimeWindowUpdater::calWaitTimeBudget(int nodes_involved, int used_waiting) //including the starting and ending node
{
    int modified_waitmax = wait_max - used_waiting;
    return min(modified_waitmax, nodes_involved * wait_pernode);
}

int TimeWindowUpdater::calNecessaryTime(int start_pos, int end_pos, bool serve_start_pos, bool serve_end_pos) //start_pos corresponds to the position of the start node of a path
{
    assert(start_pos <= end_pos);
    int nectime = 0;
    int nec_servetime = 0;
    int nec_tvltime = calPathTvlTime(extended_route, initial_timemat, start_pos, end_pos);
    if(serve_start_pos) nec_servetime += st_inroute[start_pos];
    if(serve_end_pos) nec_servetime += st_inroute[end_pos];
    for(int i = start_pos+1; i < end_pos; i++)
    {
        nec_servetime += st_inroute[i];
    }
    nectime = nec_servetime + nec_tvltime;
    return nectime;
}

int TimeWindowUpdater::calMaximumTime(int start_pos, int end_pos, bool serve_start_pos, bool serve_end_pos, int used_waiting)
{
    assert(start_pos <= end_pos);
    int maxtime = 0;
    int nectime = calNecessaryTime(start_pos, end_pos, serve_start_pos, serve_end_pos);
    int nodes_involved;
    if(serve_start_pos && serve_end_pos)
    {
        nodes_involved = end_pos - start_pos + 1;
    }
    else if(!serve_start_pos && !serve_end_pos)
    {
        nodes_involved = end_pos - start_pos - 1;
    }
    else 
    {
        nodes_involved = end_pos - start_pos;
    }
    int wait_time_budget = calWaitTimeBudget(nodes_involved, used_waiting);
    maxtime = nectime + wait_time_budget;
    return maxtime;
}

int TimeWindowUpdater::calExpectedAT1(int node_pos, int lastnode_AT1)  
{
    int AT1;
    if(node_pos == 0)   //the starting depot
    {
        AT1 = sertw_inroute[node_pos][0];  //the starting depot does not have AT1:  nodeset->service_tw[0][0];
    }
    else //nodes in the middle, including the ending depot
    {
        //case 3:
        int startserve_last = sertw_inroute[node_pos-1][0]; //if the node is a bypassed node, then the service time window takes its travel time window
        int servetime_lastnode = st_inroute[node_pos-1];
        int tvltime_lastnode = calPathTvlTime(extended_route, initial_timemat, node_pos-1, node_pos); //start_nodepos (nodepos-1) needs to be smaller than end_nodepos (node_pos)
        int case3_AT1 = max(lastnode_AT1, startserve_last) + servetime_lastnode + tvltime_lastnode;
        AT1 = case3_AT1;
        //case 1:
        for(int i = extendroutelen-1; i > node_pos; i--)  //case1: tracking backward from the last node to calculate AT1
        {
            if(node_labels[i]) //label == 1: the referred node is a served node; only care about the start service time of the served nodes
            {
                int startserve_i = sertw_inroute[i][0]; //get the earliest allowable service start time
                bool serve_end_pos = false;//serve i, the end node
                bool serve_start_pos = true;//serve node_pos, the start node
                int max_time = calMaximumTime(node_pos, i, serve_start_pos, serve_end_pos, 0);
                int case1_AT1 = startserve_i - max_time;
                if(case1_AT1 > AT1)
                {
                    AT1 = case1_AT1;
                }
            }
        }
        //case 2 (only for the served nodes):
        if(node_labels[node_pos])  //label == 1: the node is a served node
        {
            int startserve_this = sertw_inroute[node_pos][0];
            int nodes_involved = 1;
            int case2_AT1 = startserve_this - calWaitTimeBudget(nodes_involved, 0);
            if(case2_AT1 > AT1)
            {
                AT1 = case2_AT1;
            }
        }
    }
    return AT1;
}

int TimeWindowUpdater::calExpectedDT1(int node_pos, int thisnode_AT1, int nextnode_AT1)
{
    int DT1;
    if(node_pos < extendroutelen-1) // && node_pos >= 0) //including the starting depot
    {
        int tvltime_nextnode = calPathTvlTime(extended_route, initial_timemat, node_pos, node_pos+1);
        DT1 = nextnode_AT1 - tvltime_nextnode;
    }
    else //node_pos == extendroutelen-1 => the ending depot
    {
        DT1 = sertw_inroute[node_pos][0];
    }
    return DT1;
}

int TimeWindowUpdater::calExpectedDT2(int node_pos, int nextnode_DT2)
{
    int DT2;
    if(node_pos == extendroutelen - 1)
    {
        DT2 = sertw_inroute[node_pos][1];
    }
    else //nodes in the middle, including the starting depot
    {
        //case 3:
        int lateserve_next = sertw_inroute[node_pos+1][1];
        int servetime_nextnode = st_inroute[node_pos+1];
        int tvltime_nextnode = calPathTvlTime(extended_route, initial_timemat, node_pos, node_pos+1);
        int case3_DT2 = min(nextnode_DT2 - servetime_nextnode, lateserve_next) - tvltime_nextnode;
        DT2 = case3_DT2;
        //case 1:
        for(int i = 0; i < node_pos; i++)  //case1: tracking forward from the first served node to calculate DT2
        {
            if(node_labels[i]) //label == 1: the referred node is a served node; only care about the start service time of the served nodes
            {
                int lateserve_i = sertw_inroute[i][1]; //get the earliest allowable service start time 
                bool serve_start_pos = true; //serve i, the start node
                bool serve_end_pos = true; //serve node_pos, the end node
                int max_time = calMaximumTime(i, node_pos, serve_start_pos, serve_end_pos, 0);
                int case1_DT2 = lateserve_i + max_time;
                if(case1_DT2 < DT2)
                {
                    DT2 = case1_DT2;
                }
            }
        }
        //case 2 (only for the served nodes):
        if(node_labels[node_pos])  //label == 1: the node is a served node
        {
            int lateserve_this = sertw_inroute[node_pos][1];
            int servetime_this = st_inroute[node_pos];
            int nodes_invovled = 1;
            int case2_DT2 = lateserve_this + servetime_this + calWaitTimeBudget(nodes_invovled, 0);
            if(case2_DT2 < DT2)
            {
                DT2 = case2_DT2;
            }
        }
    }
    return DT2;
}

int TimeWindowUpdater::calExpectedAT2(int node_pos, int thisnode_DT2, int lastnode_DT2)
{
    int AT2;
    if(node_pos == 0)
    {
        AT2 = sertw_inroute[node_pos][1];
    }
    else //if(node_pos > 0 && node_pos <= extendroutelen-1)
    {
        int tvltime_lastnode = calPathTvlTime(extended_route, initial_timemat, node_pos-1, node_pos);
        AT2 = lastnode_DT2 + tvltime_lastnode;
    }
    return AT2;
}

void TimeWindowUpdater::calRouteExpectedTW()
{
    clear_route_tw();
    initTimeElements();
    //AT1
    int lastnode_AT1 = calExpectedAT1(0, 0);
    route_arrtw[0][0] = lastnode_AT1;
    for(int i = 1; i < extendroutelen; i++)
    {
        int AT1_i = calExpectedAT1(i, lastnode_AT1);
        route_arrtw[i][0] = AT1_i;
        lastnode_AT1 = AT1_i;
    }
    //DT1
    for(int i = 0; i < extendroutelen-1; i++)
    {
        route_deptw[i][0] = calExpectedDT1(i, route_arrtw[i][0], route_arrtw[i+1][0]);
    }
    route_deptw[extendroutelen-1][0] = calExpectedDT1(extendroutelen-1, 0, 0);
    //DT2
    int nextnode_DT2 = calExpectedDT2(extendroutelen-1, 0);
    route_deptw[extendroutelen-1][1] = nextnode_DT2;
    for(int i = extendroutelen-2; i >= 0; i--)
    {
        int DT2_i = calExpectedDT2(i, nextnode_DT2);
        route_deptw[i][1] = DT2_i;
        nextnode_DT2 = DT2_i;
    }    
    //AT2
    for(int i = extendroutelen-1; i > 0; i--)
    {
        route_arrtw[i][1] = calExpectedAT2(i, route_deptw[i][1], route_deptw[i-1][1]);
    }
    route_arrtw[0][1] = calExpectedAT2(0, 0, 0); 
}

bool TimeWindowUpdater::checkNodeTimeFeas(int node_pos)
{
    //case 1: AT1 > AT2
    if(route_arrtw[node_pos][0] > route_arrtw[node_pos][1])
        return false;
    // //case 2: DT1 > DT2
    // else if(route_deptw[node_pos][0] > route_deptw[node_pos][1])
    //     return false;
    // //case 3: AT1 > the latest start-service time (for intermediate nodes, AT1 > the latest travelable time)
    // else if(route_arrtw[node_pos][0] > sertw_inroute[node_pos][1])
    //     return false;
    // //case 4: DT2 < the earliest start-service time + service time (for intermediate nodes, DT2 < the earlist travelable time)
    // else if(route_deptw[node_pos][1] < sertw_inroute[node_pos][0] + st_inroute[node_pos])
    //     return false;
    return true;
}

bool TimeWindowUpdater::isRouteTimeFeas()
{
    //check whether all nodes have feasible time windows
    for(int i = 0; i < extendroutelen; i++)
    {
        if(!checkNodeTimeFeas(i))
            return false;
    }
    return true;
}

vector<int> TimeWindowUpdater::getTimeInfeasNodePos()
{
    //get the nodes with infeasible time windows
    vector<int> infeasible_nodepos;
    for(int i = 0; i < extendroutelen; i++)
    {
        if(!checkNodeTimeFeas(i))
            infeasible_nodepos.push_back(i);
    }
    return infeasible_nodepos;
}

int TimeWindowUpdater::calActualWaitTimePerNode(int nodepos, int arrtime_pos, int deptime_pos)
{
    int wait_thisnode;
    if(node_labels[nodepos] == 1) //served node with service time window
    {
        int earliest_servicetime = sertw_inroute[nodepos][0];
        int servetime_thisnode = st_inroute[nodepos];
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

vector<int> TimeWindowUpdater::getActualWaitTimeAllNodes()
{
    vector<int> waittime_allnodes(this->extended_route.size());
    waittime_allnodes[0] = 0;
    waittime_allnodes[waittime_allnodes.size()-1] = 0;
    for(int i = 1; i < waittime_allnodes.size()-1; i++)
    {
        waittime_allnodes[i] = calActualWaitTimePerNode(i, this->route_arrtime[i], this->route_deptime[i]);
    }
    return waittime_allnodes;
}

void TimeWindowUpdater::Calib_AT1_DT1(int fixed_deptw_arcpos, vector<vector<int>> original_arrtw)
{
    int fixed_arrtw_next_arcpos = fixed_deptw_arcpos + 1;
    //first calculate AT1 forward from the start position -> move as fast as possible based on the overlapped earliest departure time
    for(int i = fixed_arrtw_next_arcpos+1; i < extendroutelen; i++)
    {
        int startserve_lastnode = sertw_inroute[i-1][0];
        int arrtime_lastnode = route_arrtw[i-1][0];
        bool serve_start_pos = true;
        bool serve_end_pos = false;
        route_arrtw[i][0] = max(max(startserve_lastnode, arrtime_lastnode) + calNecessaryTime(i-1, i, serve_start_pos, serve_end_pos), original_arrtw[i][0]);
    }
    //then calculate DT1 based on AT1 for the nodes after the fixed_arrtw_next_arcpos (including the fixed_arrtw_next_arcpos)
    for(int i = extendroutelen-1; i > fixed_arrtw_next_arcpos; i--)
    {
        int arc_dist_i_AT1 = calPathTvlTime(extended_route, initial_timemat, i-1, i);
        route_deptw[i-1][0] = route_arrtw[i][0] - arc_dist_i_AT1;
    }
    //next calculate the remaining waiting time for the nodes after the fixed_arrtw_next_arcpos (including the fixed_arrtw_next_arcpos)
    int waiting_after_arc_end_pos = 0;
    for(int i = fixed_arrtw_next_arcpos; i < extendroutelen-1; i++) //exclude the ending depot
    {
        waiting_after_arc_end_pos += calActualWaitTimePerNode(i, route_arrtw[i][0], route_deptw[i][0]);
    }
    //then calculate AT1 backward using the remaining waiting time limit
    for(int i = fixed_deptw_arcpos; i > 0; i--)
    {
        int total_wait_max_AT1 = calWaitTimeBudget(fixed_arrtw_next_arcpos - i, waiting_after_arc_end_pos);
        bool serve_start_pos = true;
        bool serve_end_pos = false;
        route_arrtw[i][0] = max(route_arrtw[fixed_arrtw_next_arcpos][0] - calNecessaryTime(i, fixed_arrtw_next_arcpos, serve_start_pos, serve_end_pos) - total_wait_max_AT1, original_arrtw[i][0]);
    }
    //next calculate DT1 based on AT1 for the nodes before the fixed_arrtw_next_arcpos (excluding the fixed_arrtw_next_arcpos)
    for(int i = fixed_deptw_arcpos-1; i >= 0; i--)
    {
        int arc_dist_i_DT1 = calPathTvlTime(extended_route, initial_timemat, i, i+1);
        route_deptw[i][0] = route_arrtw[i+1][0] - arc_dist_i_DT1;
    }
}

void TimeWindowUpdater::Calib_DT2_AT2(int fixed_deptw_arcpos, vector<vector<int>> original_deptw_input)
{
    int fixed_arrtw_next_arcpos = fixed_deptw_arcpos + 1;
    //first calculate DT2 backward from the start position -> move reversely as fast as possible based on the overlapped latest departure time
    for(int i = fixed_deptw_arcpos-1; i >= 0; i--)
    {
        int lateserve_nextnode = sertw_inroute[i+1][1];
        int servetime_nextnode = st_inroute[i+1];
        int deptime_nextnode = route_deptw[i+1][1];
        int arc_dist_i_DT2 = calPathTvlTime(extended_route, initial_timemat, i, i+1);
        route_deptw[i][1] = min(min(deptime_nextnode - servetime_nextnode, lateserve_nextnode) - arc_dist_i_DT2, original_deptw_input[i][1]);
    }
    //then calculate AT2 based on DT2 for the nodes before the fixed_deptw_arcpos (including the fixed_deptw_arcpos)
    for(int i = 1; i < fixed_arrtw_next_arcpos; i++)
    {
        int arc_dist_i_AT2 = calPathTvlTime(extended_route, initial_timemat, i-1, i);
        route_arrtw[i][1] = route_deptw[i-1][1] + arc_dist_i_AT2;
    }
    //next calculate the remaining waiting time for the nodes before the fixed_deptw_arcpos (including the fixed_deptw_arcpos)
    int waiting_before_arc_start_pos = 0;
    for(int i = 1; i <= fixed_deptw_arcpos; i++) //exclude the ending depot
    {
        waiting_before_arc_start_pos += calActualWaitTimePerNode(i, route_arrtw[i][1], route_deptw[i][1]);
    }
    //then calculate DT2 forward using the remaining waiting time limit
    for(int i = fixed_arrtw_next_arcpos; i < extendroutelen-1; i++) //fixed_arrtw_next_arcpos = fixed_deptw_arcpos + 1
    {
        int total_wait_max_DT2 = calWaitTimeBudget(i - fixed_deptw_arcpos, waiting_before_arc_start_pos);
        bool serve_start_pos = true;
        bool serve_end_pos = false;
        route_deptw[i][1] = min(route_arrtw[fixed_arrtw_next_arcpos][1] + calNecessaryTime(fixed_arrtw_next_arcpos, i, serve_start_pos, serve_end_pos) + total_wait_max_DT2, original_deptw_input[i][1]);
    }
    //next calculate AT2 based on DT2 for the nodes after the fixed_arrtw_next_arcpos (excluding the fixed_arrtw_next_arcpos)
    for(int i = fixed_arrtw_next_arcpos+1; i < extendroutelen; i++) 
    {
        int arc_dist_i_AT2 = calPathTvlTime(extended_route, initial_timemat, i-1, i);
        route_arrtw[i][1] = route_deptw[i-1][1] + arc_dist_i_AT2;
    }
}

void TimeWindowUpdater::CalibRouteTW(int fixed_deptw_arcpos, vector<int> overlap_dep_tw) //int fixed_arrtw_next_arcpos
{
    int fixed_arrtw_next_arcpos = fixed_deptw_arcpos + 1;
    //1. first store the original arrival and departure time windows and clear the arrival and departure time windows
    vector<vector<int>> original_arrtw = route_arrtw;
    vector<vector<int>> original_deptw = route_deptw;
    
    //2. assign the overlapped departure time windows and arrival time windows of the given arc
    route_deptw[fixed_deptw_arcpos] = overlap_dep_tw;
    int arc_tvltime = calPathTvlTime(extended_route, initial_timemat, fixed_deptw_arcpos, fixed_arrtw_next_arcpos);
    route_arrtw[fixed_arrtw_next_arcpos] = {overlap_dep_tw[0] + arc_tvltime, overlap_dep_tw[1] + arc_tvltime};
    route_arrtw[0] = sertw_inroute[0]; //arrival time windows for the starting depot
    route_deptw[extendroutelen-1] = sertw_inroute[extendroutelen-1]; //departure time windows for the ending depot
    
    //3. AT1 & DT1 -> void Calib_AT1_DT1()
    Calib_AT1_DT1(fixed_deptw_arcpos, original_arrtw);

    //4. AT2 & DT2 -> void Calib_DT2_AT2()
    Calib_DT2_AT2(fixed_deptw_arcpos, original_deptw);
}

void TimeWindowUpdater::clear_route_tw()
{
    this->route_arrtw.clear();
    this->route_deptw.clear();
}

void TimeWindowUpdater::clear_route_time()
{
    this->route_arrtime.clear();
    this->route_deptime.clear();
}

//calculate the actual arrival and departure time for a route
//the purpose is to find a set of arrival and departure time with the minimum trip duration
void TimeWindowUpdater::setFinalArrDepTime()
{
    route_arrtime.resize(route_arrtw.size());
    route_deptime.resize(route_deptw.size());

    route_arrtime[0] = 0;
    route_deptime[0] = route_deptw[0][1]; //the latest departure time of the first node
    for(int i = 1; i < route_arrtw.size(); i++)
    {
        route_arrtime[i] = route_deptime[i-1] + initial_timemat[extended_route[i-1]][extended_route[i]]; //arrival time of the node
        route_deptime[i] = max(route_arrtime[i] + st_inroute[i], route_deptw[i][0]); //departure time of the node
    }
    route_deptime[route_arrtw.size()-1] = 0;
}

int TimeWindowUpdater::calRouteTotalTripDuration()
{
    return route_arrtime[extendroutelen-1] - route_deptime[0];
}

int TimeWindowUpdater::calTotalWaitTime()
{
    int route_total_tripdur = calRouteTotalTripDuration();
    bool serve_start_pos = false;
    bool serve_end_pos = false;
    int nec_tripdur = calNecessaryTime(0, extendroutelen-1, serve_start_pos, serve_end_pos);
    int route_wait_time = route_total_tripdur - nec_tripdur;
    return route_wait_time;
}