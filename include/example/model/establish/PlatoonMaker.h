// PlatoonMaker.h

#ifndef _PLATOONMAKER_H_    //include guards
#define _PLATOONMAKER_H_

#include <vector>
#include <string>
#include <tuple>
using namespace std;

class TimeWindowUpdater;
class APlatoon;
class VRPSolution;
class Nodes;
class ARoute;

/// @brief a class to systematically make platoons for the current solution and update the arrival and departure time for all routes
class PlatoonMaker
{
    private: 
        //! the pointer to the current input solution
        VRPSolution& cur_sol;

        //! the pointer to the input nodes set
        Nodes* nodeset;

        //! the number of routes invovled in the solution
        int routes_num; 
        
        //! all the extended routes in the solution that consist of arcs instead of nodes
        vector<vector<vector<int>>> arcs_based_extended_routes;

        //! the set of unique arcs in the solution
        vector<vector<int>> unique_arcs_set;
        
        //! the number of unique arcs in the solution
        int unique_arcs_num;
        
        //! the occurance information of an arc in the solution
        struct OccuranceOneArc
        {
            vector<int> thisarc; // {1,2}
            vector<int> common_vehs; // {0,1,2}
            vector<int> common_routes; // {3,4,5}
            vector<vector<int>> arc_all_pos; // {{}, {}, {}}
            vector<pair<int, int>> pairwise_feas_graph_nodeset; // <route_id, arcpos>

            bool operator ==(const OccuranceOneArc& other) const 
            {
                return thisarc == other.thisarc;
            }
        };

        //! the set of occurance information for each unique arc - OccuranceOneArc
        vector<OccuranceOneArc> unique_arcs_occurance;

        //! the set of all valid platooning solutions
        vector<APlatoon*> all_valid_platoons;

        //! set the pivot arc position in all routes in order to minimize the total trip duration
        vector<int> pivotArc_all_routes;
        
        //! build an arc-based route
        vector<vector<int>> buildArcBasedRoute(vector<int> input_extended_route);

        //! set all extended routes in the solution to be arc-based routes
        void setAllArcBasedExtendedRoutes();

        //! find all unique arcs in the solution
        void findAllUniqueArcs();

        //! find the common vehicles with each arc: Set the OccuranceOneArc.common_vehs and OccuranceOneArc.common_routes
        void findCommonVehs();
        
        //! find the positions of each arc in all routes: Set the OccuranceOneArc.arc_all_pos and OccuranceOneArc.pairwise_feas_graph_nodeset
        void findArcPosAllRoutes();
        
        //! check the connectivity of two nodes in the pairwise feasibility graph based on the overlapping departure time windows
        bool checkPairConnectivity(pair<int, int> route_pos1, pair<int, int> route_pos2, vector<ARoute*>& sol_config_copy);
        
        //! build the set of nodes in the pairwise feasibility graph for each unique arc
        vector<pair<int, int>> buildFullGraphNodesSet(OccuranceOneArc input_arc_config); 

        //! build the pairwise feasibility graph for a given arc
        vector<vector<bool>> buildPairwiseFeasGraph(vector<pair<int, int>>& graph_nodes, vector<ARoute*>& sol_config_copy);
        
        //! process the neighbor (keep or remove) for an input graph node in the BronKerbosch algorithm
        vector<int> processNodeNeighborsInSet(int input_u, vector<int> input_set, vector<vector<bool>>& pair_feas_graph, bool keep);
        
        //! the main body of BronKerbosch algorithm to find all maximal cliques based on a pairwise feasibility graph
        void BronKerbosch(vector<int> Rset, vector<int> Pset, vector<int> Xset, vector<vector<bool>>& pair_feas_graph, vector<vector<int>>& maximal_cliques_set);
        
        //! transform the cliques of ID set to cliques of graph nodes
        vector<pair<int, int>> transformIDsetToNodeset(vector<int>& clique_idset, vector<pair<int, int>>& graph_nodes);
        
        //! find all maximal cliques of ID set with the BronKerbosch algorithm 
        vector<vector<int>> findMaximalCliquesIDset(vector<pair<int, int>>& graph_nodes, vector<vector<bool>>& pair_feas_graph);
        
        //! find the maximum clique of node set based on the BronKerbosch algorithm
        vector<APlatoon*> findMaximalCliquesNodeset(vector<pair<int, int>>& graph_nodes, vector<vector<bool>>& pair_feas_graph);
        
        //! find the clique with the maximum size of ID set based on all the maximal cliques for a given arc
        vector<int> findMaxCliqueSizeIDset(vector<vector<int>> all_maximal_cliques_idset_one_arc);

        //! find the clique with the maximum energy saving of node set based on all the maximal cliques for a given arc
        APlatoon* findMaxCliqueSavingNodeset(vector<APlatoon*>& all_maximal_cliques);
        
        //! calculate the overlapping departure time windows for one platoon on a given arc
        vector<int> calOverlapDeptwOnePlatoon(vector<pair<int, int>> platoon_config_on_arc);

        //! update the overlapping departure time windows for the selected platoon on a given arc
        vector<int> shrinkOverlapDeptwOnePlatoon(APlatoon* input_platoon, vector<int> overlap_deptw);
        
        //! update the arrival and departure time windows for all routes with the platoon, given the platoon's updated departure time windows
        void updateArrDeptwAllCouplingRoutes(vector<pair<int, int>>& platoon_config_on_arc, vector<int> maximum_platoon_tw, vector<ARoute*>& sol_config_copy);
        
        //!  find all platoons on one arc based on the current pairwise feasibility graph (until no nodes are left)
        vector<APlatoon*> findAllPlatoons(vector<pair<int, int>>& graph_nodes, vector<vector<bool>>& pair_feas_graph);
        // vector<vector<pair<int, int>>> findAllPlatoons(vector<pair<int, int>>& graph_nodes, vector<vector<bool>>& pair_feas_graph, vector<vector<int>>& overlapped_deptw_each_platoon);

        //! calculate the total energy saving for all platoons on one arc
        double calEnergySavingAllPlatoonsOneArc(vector<int> thisarc, vector<pair<int, int>>& graph_nodes, vector<vector<pair<int, int>>>& all_cliques_one_arc);
        
        //! systematically and iteratively find all valid platoons for one arc
        vector<APlatoon*> CouplingModuleOneArc(OccuranceOneArc input_arc_config);

        //! systematically and iteratively find all platoons for all arcs in the solution
        void CouplingHeuristic(); 
    
    public:
        //! constructor
        PlatoonMaker(VRPSolution& sol, Nodes& nodes);

        //! default constructor
        PlatoonMaker(VRPSolution& sol) : cur_sol(sol) {};

        //! destructor
        ~PlatoonMaker();

        //! end()
        // void end();

        //! set the final arrival and departure time for all routes in the solution
        void setArrDepTimeAllRoutes(); //set the arrival time of all nodes within all routes in the solution -> &cur_sol.sol_config[r]
        
        //! a simple getter to get all platoons for a solution
        vector<APlatoon*> getAllPlatoons();

        //! a simple getter to get all platoons on a given arc
        vector<APlatoon*> getAllPlatoonsOneArc(vector<int> input_arc);

        //! a simple getter to get the updated arrival time windows for a given route after making all platoons
        vector<vector<int>> getArrtwAfterPlatooning(int rid);

        //! a simple getter to get the updated departure time windows for a given route after making all platoons
        vector<vector<int>> getDeptwAfterPlatooning(int rid);

        //! a simple getter to get the final arrival time for a given route after making all platoons
        vector<int> getArrTimeAfterPlatooning(int rid);

        //! a simple getter to get the final departure time for a given route after making all platoons
        vector<int> getDepTimeAfterPlatooning(int rid);

        //! a simple getter to get the set of unique arcs
        vector<vector<int>> getUniqueArcsSet();

        //! a simple getter to get the total number of times that the arc appears in a solution
        int getArcShowTimes(vector<int> input_arc);

        //! calculate the total energy-related distance costs for the solution
        double calSolTotalEnergyDist(); //get the total length of the solution -> including the total number of vehicles and total number of positions within each vehicle route

        //! calculate the total energy-saving for the solution
        double calSolTotalEnergySaving(); 

};

#endif