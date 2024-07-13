// HeuristicFramework.h

#ifndef _HEURISTICFRAMEWORK_H_    //include guards
#define _HEURISTICFRAMEWORK_H_

#include <vector>
#include <string>
#include "ALNS_Parameters.h"
#include "config.h"
using namespace std;

//set up the framework for ALNS heuristics
class HeuristicFramework  
{
    protected: 
        //variables
        Nodes nodeset;
        Vehicles vehset;
        double cpu_time;
        Solution best_sol;

        //build struct data
        //struct OneCus; struct OneVeh;
        //for struct defined within each class, can only describe feature of a variable, cannot be output of a func

        //for routes
        Route build_new_route_struct(int &input_veh_id, int &input_veh_type, vector<int> &input_compact_route);
        void init_used_paths_in_compact_route(Route &route);
        void set_used_paths(Route &route, int compact_nodepos, int used_path_id);
        void set_extended_route(Route &route);
        void set_node_labels(Route &route);
        void set_route_arrdep_tw(Route &route);
        void set_route_arrdep_time(Route &route);
        void set_route_loads(Route &route);
        void set_route_mileage(Route &route);
        int cal_route_pickup_dmd(Route &route);
        int cal_route_delivery_dmd(Route &route);
        int cal_route_trip_dur(Route &route);
        double cal_route_total_dist(Route &route);

        //for solutions
        int get_sol_route_num(Solution &sol); //get the number of routes in the initial solution
        Route get_route_config(Solution &sol, int route_id);
        void find_platoons_all_arcs(Solution &sol);
        double cal_sol_total_dist(Solution &sol);
        double cal_sol_total_energy_dist_cost(Solution &sol);
        // double cal_route_energy_dist(Solution &sol, int route_id);
        int cal_sol_total_trip_dur(Solution &sol);
        int cal_sol_total_unserved_requests(Solution &sol);
        double cal_sol_total_obj_val(Solution &sol);

        //for nodes and routes operations
        int tranform_vehid_to_routeid(vector<Route> sol_config, int input_vehid);
        int tranform_nodepos_extend_to_compact(Route &route, int node_pos_extend);
        void insert_used_path(vector<int> &used_paths_vec, int insert_nodepos_compact, int used_path_before, int used_path_after);
        void remove_used_path(vector<int> &used_paths_vec, int remove_nodepos_compact, int changed_used_path);
        void insert_a_node(Route &route, int node_pos_compact, int node_id);
        void remove_a_node(Route &route, int node_pos_compact);
        // int cal_nodepos_loads(Route &route, int node_pos_in_compact_route);
        bool check_load_insert_feas(Route &route, int insert_nodepos_compact, int insert_nodeid);
        bool check_mileage_insert_feas(Route &route, int insert_nodepos_compact, int insert_nodeid);
        bool check_tw_insert_feas(Route &route, int insert_nodepos_compact, int insert_nodeid);
        double cal_insertion_cost(vector<int> compact_route, int node, int pos, int orig_usedpath_pos);
        auto cal_cheapest_insert_cost(vector<int> compact_route, vector<int> orig_usedpath_compact, int node); //calculate the cheapest insertion cost of a customer into a route
        double cal_removal_cost(vector<int> compact_route, vector<int> orig_usedpath_compact, int pos);

    public:
        HeuristicFramework(Nodes &nodes, Vehicles &vehs);

        //for algorithm running
        virtual void run_algorithm() = 0; //run the heuristic and record the cpu time
        Solution get_best_solution(); //get the best solution
        double get_cputime();
};

#endif