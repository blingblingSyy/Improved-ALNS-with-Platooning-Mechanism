#ifndef NODES_H_
#define NODES_H_

#include <vector>
#include "couplingVRP/model/RawInstance.h"
#include "couplingVRP/model/NodesManager.h"
#include "couplingVRP/model/ADijkstraSol.h"
#include "couplingVRP/model/AlternativePaths.h"
using namespace std;

class RawInstance;
class NodesManager;
class ADijkstraSol;
class AlternativePaths;

/// @brief the class is to define the features of all nodes and the methods to set, modify and get the nodes information
class Nodes
{
    private:
        //! a copy of raw data of input instance
        RawInstance rawInstance;

        //! a manager of processing nodes data
        NodesManager NodesMan;

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

        //! initial travel time matrix (round up to integer)
        vector<vector<int>> initial_timemat;
        
        //! the shortest path distance matrix
        vector<vector<double>> SP_distmat; //shortest path distance matrix between any two nodes
        
        //! the shortest path distance matrix
        vector<vector<int>> SP_timemat;  //shortest path travel time matrix between any two nodes
        
        //! the set of all valid available path sets between any two nodes
        vector<vector<vector<ADijkstraSol>>> avail_path_set;

        //! the service time window for each node; for depot and intersections, the time windows are the travel time windows
        vector<vector<int>> service_tw;
        
        //! the travel time window for each node (possible time to bypass a node)
        vector<vector<int>> travel_tw;

        //! the service time for each node; either constant or dependent on the customer demands
        vector<int> service_time;

        //! the service rate for each node, usually different for passegners and freight
        vector<double> service_rate;

    public:
        Nodes(RawInstance inputInstance, bool modify_connectivity);
        ~Nodes() {};

        //! a simple getter
        int getNodeNum() {return nodenum;}; 

        //! a simple getter
        int getNodeType(int nodeid) {return nodetype[nodeid];};

        //! a simple getter
        int getDemands(int nodeid) {return demands[nodeid];};

        //! a simple getter
        vector<double> getCoordinate(int nodeid) {return coordinates[nodeid];};

        //! a simple getter
        double getInitialDist(int nodeid1, int nodeid2) {return initial_distmat[nodeid1][nodeid2];};

        //! a simple getter
        int getSPtime(int nodeid1, int nodeid2) {return SP_timemat[nodeid1][nodeid2];};

        //! a simple getter
        vector<ADijkstraSol> getAvailPathSet(int nodeid1, int nodeid2) {return avail_path_set[nodeid1][nodeid2];};

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
};


#endif