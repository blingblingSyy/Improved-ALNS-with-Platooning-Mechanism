#ifndef VRPSOLUTION_H_
#define VRPSOLUTION_H_

#include <vector>
#include <tuple>
#include <set>
#include "src/improvedALNS/ISolution.h"
#include "couplingVRP/model/basic/Nodes.h"
using namespace std;

class ARoute;
class ISolution;
class APlatoon;
class Nodes;
class Vehicles;

/// @brief a class to define and modify a VRP solution with platooning mechanism
class VRPSolution: public ISolution 
{
    public:
        //! Enumeration representing the various kind of operations on the route.
        enum SolOperationKind
        {
            Initialization,
            InsertOneNode,
            RemoveOneNode,
            ModifyOnePath,
            DeleteOneRoute,
            NoChange
        };

        //! Constructor
        VRPSolution(Nodes& nodeset, Vehicles& vehset);

        //! Destructor.
        virtual ~VRPSolution();

        //! whether the solution is empty or not
        bool isSolEmpty();

        //! build a new route with randomly selected node from the set of uninserted customers
        ARoute* buildNewRoute();

        //! build an initial solution from scratch
        void buildInitialSol();

        //! A getter for the value of the objective function.
        virtual double getObjectiveValue() {return totalObjValueAfterPlatooning;};

        //! \return a penalized version of the objective value if the solution is infeasible. 
        //! But in this algorithm, all solutions must be feasible. Therefore, the penalized objective value is the same as the objective value.
        virtual double getPenalizedObjectiveValue(bool modified = false);

        //! A getter for the feasibility of the current solution.
        //! \return true if the solution is feasible, false otherwise.
        virtual bool isFeasible();

        //! A comparator.
        //! \return true if this solution is "better" than the solution it is compared to.
        virtual bool operator<(ISolution&);

        //! Compute the "distance" between solution.
        //! This feature can be used as part of the ALNS to favor the
        //! diversification process. If you do not plan to use this feature
        //! just implement a method returning 0.
        virtual int distance(ISolution&);

        //! This method create a copy of the solution.
        virtual ISolution* getCopy();

        //! Compute a hash key of the solution.
        virtual long long getHash();

        //! insert a node into one route at a position
        void insertNode(int insert_pos, int insert_nodeid, int routeid);

        //! remove a node from one route at a position
        void removeNode(int remove_pos, int routeid);

        //! modify the used path at a position in one route
        void repairPath(int modified_arcpos, int modified_pathid, int routeid);
        
        //! evaluate the costs of inserting a node 
        double evaluateInsertNode(int insert_pos, int insert_nodeid, int routeid);

        //! evaluate the costs of removing a node
        double evaluateRemoveNode(int remove_pos, int routeid);

        //! evaluate the costs of modifying the used path in a route
        double evaluateModifyPath(int modified_arcpos, int modified_pathid, int routeid);

        //! calculate the ordered insertion costs of all routes for a given node
        vector<tuple<double, int, int>> VRPSolution::calOrderedInsertCostsAllRoutes(int insert_node_id);

        //! calculate the cheapest insertion costs of all routes for a given node <insert_cost, insert_pos, insert_route>
        tuple<double, int, int> calCheapestInsertCostsAllRoutes(int insert_node_id, int k_cheapest); 

        //! delete one route
        void deleteOneRoute(int rid);
        
        //! make platoons for the current solution
        void makePlatoons();

        //! set the final arrival and departure time for all routes in the current solution
        void setFinalArrDepTime();

        //! calculate the total distance of the solution
        double calTotalDist();

        //! calculate the total energy-related distance of the solution
        double calTotalEnergyDist();

        //! calculat ethe total trip duration of the solution
        int calTotalTripDuration();

        //! calculate the total unserved requests of the solution
        int calTotalUnservedRequests();

        //! calculate the objective value after platooning
        double calTotalObjectiveValue();

        //! recompute the costs for a new solution
        void recomputeCost(bool recouple = false);

        //! a simple getter
        double& getTotalDistBeforePlatooning() {return totalDistCostsBeforePlatooning;};

        //! a simple getter
        double& getTotalDistAfterPlatooning() {return totalDistCostsAfterPlatooning;};

        //! a simple getter
        int& getTotalTripDuration() {return totalTripDurationAllRoutes;};

        //! a simple getter
        int& getTotalUnservedRequests() {return totalUnservedRequests;};

        //! a simple getter
        vector<int>& getNonInsertedNodes() {return nonInsertedNodes;};

        //! a simple getter
        int getTotalServedCusNum() {return static_cast<int>(nodeset->getCusNum() - nonInsertedNodes.size());};

        //! a simple getter
        vector<int>& getNonUsedVehs() {return nonUsedVehs;};

        //! a simple getter
        vector<tuple<int, int, int>>& getDestroyedArcsPos() {return destroyedArcs;};
        
        //! a simple getter
        int getRoutesNum() {return sol_config.size();};

        //! a simple getter
        int getPlatoonNum() {return platoons_config.size();};

        //! a simmple getter to get a route
        ARoute* getOneRoute(int rid) {return sol_config[rid];};

        //! a simple getter
        vector<ARoute*>& getAllRoutes() {return sol_config;};

        //! a simple gettere
        vector<APlatoon*>& getAllPlatoons() {return platoons_config;};

        //! the cpu time before making platoons
        double getCpuBeforePlatooning() {return cpuBeforePlatooning;};

        //! the cpu time after making platoons
        double getCpuAfterPlatooning() {return cpuAfterPlatooning;};

        //! a simple getter: the actual destroyble arc in the current solution
        vector<tuple<int, int, int>> getDestroyableArcPos() {return destroyableArcPos;};

        //! a simple getter
        vector<vector<int>> getDestroyableArcConfig() {return destroyableArcConifg;};

        //! a simple getter
        int getNodeShowTimes(int nodeid);

        //! get positions of all customers <routeid, arcpos>
        vector<pair<int, int>> getAllCusPos();

        /* old version */
        // //! calculate the maximum arrival time of all nodes in all routes
        // int getMaxCusArrTime();

        /* old version */
        // //! calculate the minimum arrival time of all nodes in all routes
        // int getMinCusArrTime();

        //! get the current operation on the solution
        SolOperationKind getSolCurOperation() {return solCurOpt;};

        //! set the current operation on the solution
        void setSolCurOperation(SolOperationKind input_opt) {solCurOpt = input_opt;};

    private:
        //! the pointer to the set of nodes
        Nodes* nodeset;

        //! the pointer to the set of vehicles
        Vehicles* vehset;

        //! the current operation to the route
        SolOperationKind solCurOpt;

        //! the customers that has not been inserted
        vector<int> nonInsertedNodes;

        //! the vehicles that has not been used
        vector<int> nonUsedVehs;

        //! the set of positions of the arcs in the compact route of the solution that can be destroyed
        vector<tuple<int, int, int>> destroyableArcPos; //! {{destroyed_arcpos, destroyed_pathid, routeid}, ...}
        
        //! the set of configuration of arcs that can be destroyed
        //! as each served node only appears in the solution once, the destroyableArcConfig is in the same order as the destroyableArcPos
        vector<vector<int>> destroyableArcConifg; //! {{destroyed_arcpos, destroyed_pathid, routeid}, ...}

        //! the selected paths to be modified later
        vector<tuple<int, int, int>> destroyedArcs; //{{destroyed_arcpos, destroyed_pathid, routeid}, ...};
        
        //! the route configuration of the solution
        vector<ARoute*> sol_config;

        //! the configuration of all platoons
        vector<APlatoon*> platoons_config;

        //! the total energy saving due to platoons
        double totalEnergySaving;

        //! the total distance before making platoons
        double totalDistCostsBeforePlatooning;

        //! the total distance after making platoons
        double totalDistCostsAfterPlatooning;

        //! the total trip duration of the sol
        int totalTripDurationAllRoutes;

        //! the total unserved requests
        int totalUnservedRequests;

        //! the total objective value after platooning
        double totalObjValueAfterPlatooning;
        
        //! the cpu time spent before platooning
        double cpuBeforePlatooning;

        //! the cpu time spent after platooning
        double cpuAfterPlatooning;

        //! find the destroyable paths in the solution
        void findDestroyablePaths();

};

#endif /* TSPSOLUTION_H_ */