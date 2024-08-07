#ifndef AROUTE_H_
#define AROUTE_H_

#include <vector>
using namespace std;

class Nodes;
class Vehicles;
class ADijkstraSol;
class TimeWindowUpdater;

/// @brief a class to store the configuration of a route and the methods to modify and update a route
class ARoute
{
    public:
        //! Enumeration representing the various kind of operations on the route.
        enum RouteOperationKind
        {
            Initialization,
            InsertOneNode,
            RemoveOneNode,
            ModifyOnePath,
            Reset,
            NoChange
        };

        //! constructor
        ARoute(Nodes& nodeset, Vehicles& vehset);

        //! copy constructor
        ARoute(ARoute& route);
        
        //! default constructor
        ARoute() = default;

        //! destructor
        ~ARoute() {};

        //! set the vehicle index of a route
        void setVehIdType(int input_vehid);

        //! set the operation kind being used of the route
        void setRouteOperator(RouteOperationKind operation) {routeOperator = operation;};

        //! initialie an empty route
        void initEmptyRoute();

        //! check whether the route is empty
        bool isEmpty();

        //! reset a route based on a new compact route
        void resetRoute(int input_vehid, vector<int> new_compact_route);

        //! copy another route
        void copyFromOtherRoute(ARoute& input_route);

        //! evaluate the cost of inserting a node
        void evaluateRouteByInsertNode(int insert_pos, int insert_nodeid);

        //! actually insert a node (link with the shortest path)
        void setRouteByInsertNode(int insert_pos, int insert_nodeid);

        //! evaluate the cost of removing a node
        void evaluateRouteByRemoveNode(int remove_pos);

        //! actually remove a node (link with the shortest path)
        void setRouteByRemoveNode(int remove_pos);

        //! evaluate the cost of modifying a path
        void evaluateRouteByModifyUsedPath(int modified_arcpos, int used_path_id);

        //! modify a used path: also modify the extended path and the vector of node labels
        void setRouteByModifyUsedPath(int modified_arcpos, int used_path_id); //the arcpos is the position of the start node of the arc
    	
        //! check the time feasibility of the route
        bool isTimeFeas(vector<int> extended_route1, vector<int> node_labels1);

        //! check the load feasibility of the route
        bool isLoadFeas(vector<int> input_load_vec);

        //! check the mileage feasibility of the route
        inline bool isDistFeas(int total_dist);

        //! check the linkage feasibility of the route
        bool isLinkFeas(int arcpos, vector<int> compactRoute);

        //! check the feasibility of the route
        bool isRouteFeas();

        //! calculate the insertion costs of a node into a position
        double calInsertionCosts(int insert_pos, int insert_nodeid);

        //! calculate the cheapest insertion costs of a node into the current compact route
        pair<double, int> calCheapestInsertionCosts(int insert_node_id);

        //! calculate the removal costs of a node poition
        double calRemovalCosts(int removal_pos);

        //! calculate the total trip duration within the route
        int getRouteDuration() {return actual_arrtime[actual_arrtime.size()-1] - actual_deptime[0];};

        //! calculate the total distance of the route without platooning
        double getRouteDistance() {return route_mileage[route_mileage.size()-1];};

        //! a simple getter
        int getVehID() {return vehid;};

        //! a simple getter
        int getVehType() {return vehtype;};

        //! a simple getter
        RouteOperationKind getCurrentRouteOperator() {return routeOperator;};

        //! a simple getter
        vector<int> getCompactRoute() {return compact_route;};

        //! a simple getter
        vector<int> getExtendedRoute() {return extended_route;};

        //! a simple getter
        vector<int> getNodeLables() {return node_labels;};

        //! a simple getter
        vector<int> getNodeExpectedArrTW(int nodepos) {return expected_arrtw[nodepos];};

        //! a simple getter
        vector<vector<int>> getRouteExpectedArrTW() {return expected_arrtw;};

        //! a simple getter 
        vector<int> getExpectedDepTW(int nodepos) {return expected_deptw[nodepos];};

        //! a simple getter
        vector<vector<int>> getRouteExpectedDepTW() {return expected_deptw;};

        //! a simple getter
        vector<int> getFinalArrTime() {return actual_arrtime;};

        //! a simple getter
        vector<int> getFinalDepTime() {return actual_deptime;};

        //! a simple getter
        vector<int> getRouteLoad() {return route_load;};

        //! a simple getter
        vector<int> getRouteMileage() {return route_mileage;};

        //! a simple getter
        int getRouteWaitTimeLimitPerNode();

        //! a simple getter
        int getRouteWaitMaxLimit();

        //! a simple getter
        double getRouteOperatorCosts(RouteOperationKind routeOperator);

        //! a simple updater
        void updateRouteExpectedArrTW(vector<vector<int>> input_arrtw) {expected_arrtw = input_arrtw;};

        //! a simple updater
        void updateRouteExpectedDepTW(vector<vector<int>> input_deptw) {expected_deptw = input_deptw;};

        //! set the final arrival and departure time of the extended route
        void calArrDepTime();

    private:
        //! the pointer to the nodeset
        Nodes* nodeset;

        //! the pointer to the vehicle set
        Vehicles* vehset;

        //! which route operation is used
        RouteOperationKind routeOperator;

        //! the node insertion costs for one operation
        double nodeInsertionCosts;

        //! the node removal costs for one operation
        double nodeRemovalCosts;

        //! the used paths modification costs for one operation
        double pathModificationCosts;

        //! the vehicle id
        int vehid;

        //! the vehicle type: 0 for passenger and 1 for freight
        int vehtype;

        //! the compact form of the route
        vector<int> compact_route;

        //! the index of the used paths between adjacent nodes in the compact route;
        vector<int> used_paths_in_compact_route;

        //! the extended form of the route
        vector<int> extended_route;

        //! the node labels of each node in the extended route
        vector<int> node_labels;

        //! the expected arrival time windows of each node in the extended route
        vector<vector<int>> expected_arrtw;

        //! the expected departure time windows of each node in the extended route
        vector<vector<int>> expected_deptw;
        
        //! the final arrival time of each node in the extended route
        vector<int> actual_arrtime;

        //! the final departure time of each node in the extended route
        vector<int> actual_deptime;

        //! the change of load in this vehicle on the compact rouote
        vector<int> route_load;

        //! the change of travelling mileage distance for this vehicle on the compact route
        vector<int> route_mileage;

        //! transform the position of a node in the extended route to the closet position in the compact route
        int transform_nodepos_extend_to_compact(int node_pos_extend);

        //! transform the position of a node in the compact route to the position in the extended route
        int transform_nodepos_compact_to_extend(int node_pos_compact);

        //! find the expanded path between two adjacent served nodes in the compact route 
        inline vector<int> findExtendPath(int node1, int node2, int used_path_in_between);

        //! initialize the used paths in the compact route
        void initUsedPaths();

        //! set the complete extended path based on the compact route and the used paths
        void buildExtendedRoute();

        //! set the complete node labels based on the compoact route and the used paths
        void buildNodeLabels();

        //! set the initial expected time windows of the extended route
        void InitExpectedArrDepTW();

        //! calculate the load of the route
        void calRouteLoad();

        //! calculate the mileage of the route
        void calRouteMileage();

        //! insert a node into the compact route
        vector<int> insertNodeIntoCompactRoute(int insert_pos, int insert_nodeid);

        //! set the used paths by inserting a node
        vector<int> setUsedPathsByInsertNode(int insert_pos, int insert_nodeid);

        //! set the extended route by inserting a node
        vector<int> setExtendRouteByInsertNode(int insert_pos, int insert_nodeid);

        //! set the node labels by inserting a node
        vector<int> setNodeLabelsByInsertNode(int insert_pos, int insert_nodeid);

        //! set the route load by insering a node
        vector<int> setRouteLoadByInsertNode(int insert_pos, int insert_nodeid);

        //! set the route mileage by inserting a node
        vector<int> setRouteMileageByInsertNode(int insert_pos, int insert_nodeid);

        //! remove a node from the compact route
        vector<int> removeNodeFromCompactRoute(int remove_pos);

        //! set the used paths by removing a node
        vector<int> setUsedPathsByRemoveNode(int remove_pos);

        //! set the extended route by removing a node
        vector<int> setExtendRouteByRemoveNode(int remove_pos);

        //! set the node labels by removing a node
        vector<int> setNodeLabelsByRemoveNode(int remove_pos);

        //! set the route load by insering a node
        vector<int> setRouteLoadByRemoveNode(int remove_pos);

        //! set the route mileage by inserting a node
        vector<int> setRouteMileageByRemoveNode(int remove_pos);

        //! modify a used path
        vector<int> modifyUsedPath(int modified_arcpos, int used_path_id);

        //! set the extended route by removing a node
        vector<int> setExtendRouteByModifyUsedPath(int modified_arcpos, int used_path_id);

        //! set the node labels by removing a node
        vector<int> setNodeLabelsByModifyUsedPath(int modified_arcpos, int used_path_id);

        //! set the route mileage by inserting a node
        vector<int> setRouteMileageByModifyUsedPath(int modified_arcpos, int used_path_id);

};

#endif