// PlatoonMaker.h

#ifndef _PLATOONMAKER_H_    //include guards
#define _PLATOONMAKER_H_

#include <vector>
#include <string>
#include <tuple>
using namespace std;

class TimeWindowUpdater;
class VRPSolution;
class Nodes;

//update the arrival and departure time windows within a route
class PlatoonMaker
{
    private: 
        VRPSolution* cur_sol;
        Nodes* nodeset;
        int routes_num; //the total number of vehicles/routes of the solution
        vector<vector<vector<int>>> arcs_based_extended_routes;
        vector<vector<int>> unique_arcs_set; //the set of unique arcs in the solution
        int unique_arcs_num; //the number of unique arcs
        struct CouplingArc
        {
            vector<int> thisarc; //{1,2}
            vector<int> common_vehs; //{0,1,2}
            vector<int> routes_id; //{3,4,5}
            vector<vector<int>> arc_all_pos; //{{}, {}, {}}

            bool operator ==(const CouplingArc& other) const 
            {
                return thisarc == other.thisarc;
            }
        };

        //calculate the distance reduction factor for a platoon of specific length
double pl_factor(int length)
{
    if(length <= 0)
    {
        throw "Division by zero or negative condition!";
    }
    return double(1+0.90*(length-1)) / double(length);
}


        vector<CouplingArc> unique_arcs_config;
        vector<CouplingArcSol> coupling_arcs_sol; //all coupling cases for all arcs of the current solution
        
        /*construct the structure of the platooning solution*/
        vector<vector<int>> make_arc_based_route(vector<int> input_extended_route); //make the route consist of arcs
        void set_arcs_based_extended_routes();
        void find_unqiue_arcs(); //find the set of unique arcs in the given solution
        void find_vehs_on_all_arcs();    //set the CouplingArc.common_vehs
        void find_all_arcpos_each_veh();    //set the CouplingArc.arc_pos
        
        /*platooning on one arc*/
        // bool check_pair_connectivity(int routeid1, int routeid2, int arc_pos1, int arc_pos2);
        bool check_pair_connectivity(pair<int, int> route_pos1, pair<int, int> route_pos2, vector<Route> sol_config_copy);
        vector<pair<int, int>> build_graph_node_set(CouplingArc input_arc_config); //build the node set for the pairwise feasibility graph for a given arc
        // vector<vector<pair<int, int>>> build_pairwise_feasibility_graph(vector<int> input_arc, vector<int> used_routes, vector<int> correspond_arc_pos); //build pairwise feasibility graph for a given arc
        vector<vector<bool>> build_pairwise_feasibility_graph(vector<pair<int, int>> &graph_nodes, vector<Route> &sol_config_copy); //build pairwise feasibility graph for a given arc
        vector<int> process_neighbor_in_set(int input_u, vector<int> input_set, vector<vector<bool>> &pair_feas_graph, bool keep_or_not);
        void BronKerbosch(vector<int> Rset, vector<int> Pset, vector<int> Xset, vector<vector<bool>> &pair_feas_graph, vector<vector<int>> &maximal_cliques_set); //Bron-Kerbosch algorithm -> find all maximal cliques
        vector<pair<int, int>> transform_id_graphnode(vector<int> &clique_idset, vector<pair<int, int>> &graph_nodes);
        vector<vector<int>> BK_maximal_cliques_idset(vector<pair<int, int>> &graph_nodes, vector<vector<bool>> &pair_feas_graph);
        vector<vector<pair<int, int>>> BK_maximal_cliques(vector<pair<int, int>> &graph_nodes, vector<vector<bool>> &pair_feas_graph); //Bron-Kerbosch algorithm -> find all maximal cliques
        vector<int> BK_maximum_clique_idset(vector<vector<int>> all_maximal_cliques_idset_one_arc); //find the maximum clique
        vector<pair<int, int>> BK_maximum_clique(vector<vector<pair<int, int>>> all_maximal_cliques_one_arc); //find the maximum clique
        
        vector<int> cal_overlap_deptw_on_arc_per_platoon(vector<pair<int, int>> platoon_config_on_arc); //calcualate the overlapped departure time window of the start node on the given arc
        void update_arrdeptw_on_route(vector<pair<int, int>> &platoon_config_on_arc, vector<int> maximum_platoon_tw, vector<Route> &sol_config_copy); //update the arrival time windows of all nodes within a given route
        vector<vector<pair<int, int>>> find_platoons_one_arc(vector<pair<int, int>> &graph_nodes, vector<vector<bool>> &pair_feas_graph, vector<vector<int>> &overlapped_deptw_each_platoon); //until no nodes are left
        // int cal_platoon_num_one_arc(vector<vector<pair<int, int>>> &all_pair_feas_graph); //calculate the number of platoons formed on the given arc
        double cal_energy_saving(vector<int> thisarc, vector<pair<int, int>> &graph_nodes, vector<vector<pair<int, int>>> &all_cliques_one_arc); //calculate the energy saving amount based on the platoons formed on the given arc
        CouplingArcSol arc_coupling_module(CouplingArc input_arc_config); //integrate all procedures of coupling on an arc

        /*platooning on all arcs*/
        void coupling_heuristic_all_arcs(); //systematically decide the sequence of coupling arcs of all routes
    
    public:
        PlatoonMaker(VRPSolution& sol, Nodes& nodes);
        void set_arrdep_time_all_routes(); //set the arrival time of all nodes within all routes in the solution -> &cur_sol.sol_config[r]
        vector<CouplingArcSol> get_coupling_sol();  //a simple getter -> vector<CouplingArcSol>
        vector<vector<int>> get_arrtw_after_platoon(int rid);
        vector<vector<int>> get_deptw_after_platoon(int rid);
        vector<int> get_arrtime_after_platoon(int rid);
        vector<int> get_deptime_after_platoon(int rid);
        vector<vector<int>> get_unique_arcs_set(); //get the set of unique arcs in the solution
        int get_arc_appear_times(vector<int> input_arc); //get the total number of times that the arc appears in a solution
        double cal_arc_total_energy_len(vector<int> input_arc); //get the total length of a given arc -> including the total number of vehicles and total number of positions within each vehicle route
        double cal_sol_total_energy_dist(); //get the total length of the solution -> including the total number of vehicles and total number of positions within each vehicle route
};

#endif