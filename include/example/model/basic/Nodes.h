#ifndef NODES_H_
#define NODES_H_

#include <vector>
#include "example/model/basic/config.h"
#include "example/model/basic/RawInstance.h"
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

        //! set the travelable time windows for all nodes
        vector<vector<int>> calTravelTW(vector<int> source_timemat, vector<int> plan_horizon, double speed);
        
        //! calibrate the service time windows for all nodes based on the travel time windows
        void calibrateServiceTW(vector<vector<int>>& initial_sertw, vector<int> servetime, vector<vector<int>> tvl_tw);

        //! get the set of adjacent nodes for all nodes given the initial distance matrix
        vector<vector<int>> findAdjacentNodes(vector<vector<double>> init_dist, int node_num);
        
        //! calculate the initial distance matrix for all pairs of nodes
        vector<vector<double>> calInitialDistMat(vector<vector<double>> coordinates);
        
        //! modify the initial distance matrix by changing connectivity between nodes
        vector<vector<double>> modifyInitialDistMat(vector<vector<double>> init_dist);
        
        //! get the initial travel time matrix based on the (modified) initial distance matrix
        vector<vector<int>> calInitialTravelTime(vector<vector<double>> init_dist, int node_num, double speed);

    public:
        //! constructor
        Nodes(RawInstance& inputInstance); //! automatically set the disconnected links
        
        //! a third constructor
        Nodes(RawInstance& inputInstance, vector<vector<int>> disconnected_links);
        
        //! default constructor
        Nodes() = default;

        //! destructor
        ~Nodes();
        
        //! a simple getter
        int getNodeNum() {return rawInstance->getAllNodesNum();}; 

        //! a simple getter
        int getCusNum() {return rawInstance->getCusNum();};

        //! a simple getter
        int getNodeType(int nodeid) {return nodetype[nodeid];};

        //! a simple getter
        int getNodeDemand(int nodeid) {return demands[nodeid];};

        //! a simple getter
        vector<int> getAllDemands() {return demands;};

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

        //! a simple setter
        void setInitialDistMat(vector<vector<double>> input_distmat) {initial_distmat = input_distmat;};

        //! a simple setter
        void setModifiedDistMat(vector<vector<double>> input_distmat) {modified_distmat = input_distmat;};

        //! a simple setter
        void disconnectLinks(int start_node, int end_node);
        
};


#endif