// HeuristicFramework.h

#ifndef _HEURISTICFRAMEWORK_H_    //include guards
#define _HEURISTICFRAMEWORK_H_

#include <vector>
#include <string>
#include "ALNS_Parameters.h"
#include "TimeWindowUpdater.h"
#include "config.h"
using namespace std;

//set up the framework for ALNS heuristics
class HeuristicFramework  
{
    protected: 
        //variables
        Nodes nodeset;
        Vehicles vehset;
        double cpu_time_before_platoon;
        double cpu_time_after_platoon;
        Solution best_sol;

        //build struct data
        //struct OneCus; struct OneVeh;
        //for struct defined within each class, can only describe feature of a variable, cannot be output of a func

        //for nodes and routes operations
        int tranform_vehid_to_routeid(vector<Route> sol_config, int input_vehid);
        int tranform_nodepos_extend_to_compact(Route &route, int node_pos_extend);

        /*building a route*/
        Route build_new_route_struct(int &input_veh_id, int &input_veh_type, vector<int> &input_compact_route);
        void init_used_paths_in_compact_route(Route &route);
        void set_used_paths(Route &route, int compact_nodepos, int used_path_id);
        void set_extended_route(Route &route);
        void set_node_labels(Route &route);
        void set_route_arrdep_tw(Route &route);
        void set_route_arrdep_time(Route &route);
        void set_route_loads(Route &route);
        void set_route_mileage(Route &route);
        void insert_used_path(vector<int> &used_paths_vec, int insert_nodepos_compact, int used_path_before, int used_path_after);
        void remove_used_path(vector<int> &used_paths_vec, int remove_nodepos_compact, int changed_used_path);
        void modify_route_insert_node(Route &route, int node_pos_compact, int node_id);
        void modify_route_remove_node(Route &route, int node_pos_compact);
        bool modify_route_used_path(Route &route, int modify_arcpos_compact, int modified_usedpath_id);
        // bool insert_check_module(Route &route, int node_pos_compact, int node_id);
        // bool remove_check_module(Route &route, int node_pos_compact);
        // void complete_modified_route(Route &route);

        /*calculating a route*/
        double cal_insertion_cost(vector<int> compact_route, int node, int pos, int orig_usedpath_pos);
        // auto cal_cheapest_insert_cost(vector<int> compact_route, vector<int> orig_usedpath_compact, int node); //calculate the cheapest insertion cost of a customer into a route
        pair<double, int> cal_cheapest_insert_cost(Route &route, int node); //calculate the cheapest insertion cost of a customer into a route
        double cal_removal_cost(vector<int> compact_route, vector<int> orig_usedpath_compact, int pos);
        // int cal_nodepos_loads(Route &route, int node_pos_in_compact_route);
        int cal_route_pickup_dmd(Route &route);
        int cal_route_delivery_dmd(Route &route);
        int cal_route_trip_dur(Route &route);
        double cal_route_total_dist(Route &route);
        // vector<int> find_servednode_id(vector<int> labels); //find the indices for the served nodes


        /*route feasibility check*/
        bool check_load_insert_feas(Route &route, int insert_nodepos_compact, int insert_nodeid);
        bool check_mileage_insert_feas(Route &route, int insert_nodepos_compact, int insert_nodeid);
        bool check_tw_insert_feas(Route &route, int insert_nodepos_compact, int insert_nodeid);
        bool check_tw_remove_feas(Route &route, int remove_nodepos_compact);
        bool check_route_insert_feas(Route &route, int insert_nodepos_compact, int insert_nodeid);
        bool check_route_remove_feas(Route &route, int remove_nodepos_compact);
        bool check_route_subtour(Route &route); //check whether the route contains subtours
        bool check_route_modify_usedpath_feas(Route &route, int modify_arcpos_compact, int modified_usedpath_id);

        /*building a solution*/
        void find_platoons_all_arcs(Solution &sol);
        void build_complete_sol(Solution &sol);

        /*calculating a solution*/
        int get_sol_route_num(Solution &sol); //get the number of routes in the initial solution
        Route get_route_config(Solution &sol, int route_id);
        double cal_sol_total_dist(Solution &sol);
        double cal_sol_total_energy_dist_cost(Solution &sol);
        // double cal_route_energy_dist(Solution &sol, int route_id);
        int cal_sol_total_trip_dur(Solution &sol);
        int cal_sol_total_unserved_requests(Solution &sol);
        double cal_sol_total_obj_val(Solution &sol);

    public:
        HeuristicFramework(Nodes &nodes, Vehicles &vehs);

        //for algorithm running
        virtual void run_algorithm() = 0; //run the heuristic and record the cpu time
        Solution get_best_solution(); //get the best solution
        vector<double> get_cputime();
};

#endif