#ifndef NODES_H_
#define NODES_H_

#include <vector>
#include "couplingVRP/model/basic/config.h"
using namespace std;

class RawInstance;
class ADijkstraSol;
class KSPBuilder;

/// @brief the class is to define the features of all nodes and the methods to set, modify and get the nodes information
class Nodes
{
    private:
        //! a copy of raw data of input instance
        RawInstance* rawInstance;

        //! whether to re-design the demands instead of using the raw demands from the instance
        bool reset_demands;

        //! whether to re-design the service time instead of using the raw service time from the instance
        bool reset_sertime;

        //! whether to re-design the service time window instead of using the raw service time window from the instance
        bool reset_sertw;

        //! whether to add intersections in the input nodes or not
        bool add_intersects;
        
        //! whether to shrink the passenger-type demands to meet the problem setting
        bool shrink_pasdmd;
        
        //! whether to modify the connectivity between some nodes or not
        bool modify_connectivity;

        //! the number of nodes including the depot, customers, and intersections
        int nodenum;

        //! the type for each node: 0 for passenger and 1 for freight
        vector<int> nodetype;

        //! the demands for each node: >0 for pickup and <0 for delivery
        vector<int> demands;

        //! the coordinate for each node: (x, y)
        vector<vector<double>> coordinates;
        
        //! the set of adjacent nodes of a given node
        vector<vector<int>> neighbours;
        
        //! initial distance matrix
        vector<vector<double>> initial_distmat;

        //! modified distance matrix by setting some links to be disconnected
        vector<vector<double>> modified_distmat;

        //! initial travel time matrix (round up to integer)
        vector<vector<int>> initial_timemat;
        
        //! the shortest path distance matrix
        vector<vector<double>> SP_distmat; //shortest path distance matrix between any two nodes
        
        //! the shortest path distance matrix
        vector<vector<int>> SP_timemat;  //shortest path travel time matrix between any two nodes
        
        //! the set of all valid available path sets between any two nodes
        vector<vector<vector<ADijkstraSol*>>> avail_path_set;

        //! the service time window for each node; for depot and intersections, the time windows are the travel time windows
        vector<vector<int>> service_tw;
        
        //! the travel time window for each node (possible time to bypass a node)
        vector<vector<int>> travel_tw;

        //! the service time for each node; either constant or dependent on the customer demands
        vector<int> service_time;

        //! the service rate for each node, usually different for passegners and freight
        vector<double> service_rate;

        //! set the types of all nodes: 0 for passenger and 1 for freight
        vector<int> set_nodetype(int node_num, bool add_intersects, double prob = PAS_REQ_PROP);
        
        //! randomly generate demands based on the node types for all nodes
        vector<int> randdemand(vector<int> nodetype);
        
        //! modify the demands for intersections and passengers
        void modify_demands(vector<int> &initial_dmd, vector<int> nodetype, bool shrink_pasdmd);
        
        //! set different constant service time for different types of nodes
        vector<int> set_servetime(vector<int> node_type);
        
        //! set different service rate for different types of nodes
        vector<double> set_serverate(vector<int> node_type);
        
        //! set the travelable time windows for all nodes
        vector<vector<int>> cal_tvltw(vector<double> source_dist, int plan_horizon, double speed);
        
        //! calibrate the service time windows for all nodes based on the travel time windows
        void calibrate_sertw(vector<vector<int>>& initial_sertw, vector<int> servetime, vector<vector<int>> tvl_tw);
        
        //! set the service time windows for all nodes from top given their different node types
        vector<vector<int>> set_sertw(vector<vector<int>> tvl_tw, vector<int> nodetype);

        //! get the set of adjacent nodes for all nodes given the initial distance matrix
        vector<vector<int>> find_adjacent_nodes(vector<vector<double>> init_dist, int node_num);
        
        //! calculate the initial distance matrix for all pairs of nodes
        vector<vector<double>> cal_init_distmat(vector<vector<double>> coordinates);
        
        //! modify the initial distance matrix by changing connectivity between nodes
        vector<vector<double>> modify_init_distmat(vector<vector<double>> init_dist);
        
        //! get the initial travel time matrix based on the (modified) initial distance matrix
        vector<vector<int>> cal_init_tvltime(vector<vector<double>> init_dist, int node_num, double speed);

        //! build the complete information of all nodes
        void buildNodesStruct(int veh_speed);

        //! max SP dist
        double maxSPdist;

        //! min SP dist
        double minSPdist;

        //! max demands
        int maxDmd;

        //! min demands
        int minDmd;

    public:
        //! constructor
        Nodes(RawInstance& inputInstance, bool reset_demands = false, bool reset_sertime = false, bool reset_sertw = false, bool add_intersects = true, bool shrink_pasdmd = true, bool modify_connectivity = true, int veh_speed = 1);
        
        //! destructor
        ~Nodes();

        //! a simple getter
        int getNodeNum() {return nodenum;}; 

        //! a simple getter
        int getCusNum() {return nodenum-1;}

        //! a simple getter
        int getNodeType(int nodeid) {return nodetype[nodeid];};

        //! a simple getter
        int getDemands(int nodeid) {return demands[nodeid];};

        //! a simple getter
        vector<double> getCoordinate(int nodeid) {return coordinates[nodeid];};

        //! a simple getter
        vector<vector<double>> getInitialDist() {return initial_distmat;};

        //! a simple getter
        vector<vector<int>> getInitialTime() {return initial_timemat;};

        //! a simple getter
        vector<vector<double>> getSPdist() {return SP_distmat;};

        //! a simple getter
        vector<vector<int>> getSPtime() {return SP_timemat;};

        //! a simple getter
        vector<ADijkstraSol*> getAvailPathSet(int nodeid1, int nodeid2) {return avail_path_set[nodeid1][nodeid2];};

        //! a simple getter
        ADijkstraSol* getOnePath(int nodeid1, int nodeid2, int pathid);

        //! a simple getter
        vector<vector<int>> getAllAvailPathSize();

        //! a simple getter
        vector<int> getNeighbour(int nodeid) {return neighbours[nodeid];};

        //! a simple getter
        vector<int> getTravelTW(int nodeid) {return travel_tw[nodeid];};

        //! a simple getter
        vector<int> getServiceTW(int nodeid) {return service_tw[nodeid];};

        //! a simple getter 
        int getServiceTime(int nodeid) {return service_time[nodeid];};

        //! a simple getter
        double getServiceRate(int nodeid) {return service_rate[nodeid];};

        //! calculate the max and min shortest path distance 
        void calMaxMinSPDist();

        //! calculate the max and min sdemands
        void calMaxMinDmd();

        //! a simple getter
        double getMaxSPDist() {return maxSPdist;};

        //! a simple getter
        double getMinSPDist() {return minSPdist;};

        //! a simple getter
        int getMaxDmd() {return maxDmd;};

        //! a simple getter
        int getMinDmd() {return minDmd;};
};


#endif