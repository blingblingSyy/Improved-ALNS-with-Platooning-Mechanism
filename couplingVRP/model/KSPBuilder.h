// KSPBuilder.h

#ifndef _AVAILABLEPATHS_H_    //include guards
#define _AVAILABLEPATHS_H_

#include <vector>
#include "couplingVRP/model/config.h"
using namespace std;

class ADijkstraSol;

/// @brief a class to find the K's shortest path for all pairs of nodes
class KSPBuilder
{
    private: 
        //! the input distance matrix (modified)
        vector<vector<double>> init_distmat;
        
        //! the total number of nodes in the network
        int node_num;

        //! the maximum number of available paths for given two nodes
        int k_limit;

        //! the shortest path solution for all pairs of nodes (matrix)
        vector<vector<ADijkstraSol*>> SP_AllPaths; 

        //! the available path set for all pairs of nodes (matrix)
        vector<vector<vector<ADijkstraSol*>>> KSP_AllPaths; 

        //! the size of available path set for each pair of nodes
        vector<vector<int>> AlternativeSetSize_PerPath; 

        //! the main body of Dijkstra algorithm
        void Dijsktra_body(vector<ADijkstraSol*> &SPset_fromstart, vector<int> &pred_fromstart, vector<vector<double>> input_distmat);

        //! find a Dijkstra solution for a given start and end node (a shortest path tree from the start node)
        ADijkstraSol* Dijkstra_OnePath(int start, int end, vector<vector<double>> input_distmat);

        //! build the shortest path matrix for all pairs of nodes
        void Dijkstra_AllPaths();

        //! generate the node sequence of the shortest path from a start node to an end node
        vector<int> generate_onepath(int end, vector<int> pred_vecs);

        //! generate the node sequence of the shortest path tree from a start node to all the other nodes
        vector<vector<int>> generate_paths_tree(vector<int> pred_vecs);
        
        //! find the K's shortest path from the start node to the end node
        vector<ADijkstraSol*> ModifiedYen_OnePath(int start_node, int end_node); 

        //! build the K's shortest path matrix for all pairs of nodes
        void ModifiedYen_AllPaths(); 

    public:
        KSPBuilder(vector<vector<double>> distmat, int nodenum, int ksp_limit = AVAIL_PATHSET_SIZE_K);
        ~KSPBuilder() {};
        
        //! a simple getter of all shortest path solution
        vector<vector<ADijkstraSol*>> getAllShortestPaths();

        //! a simple getter of all shortest path distance
        vector<vector<double>> getAllShortestPathDistance();

        //! a simple getter of the shortest path solution of a given pair of nodes
        ADijkstraSol* getOneShortestPath(int start_id, int end_id);

        //! a simple getter of the shortest path distance of a given pair of nodes
        double getOneShortestPathDistance(int start_id, int end_id);

        //! a simple getter of the available path set for all nodes
        vector<vector<vector<ADijkstraSol*>>> getAllAvailablePathSet();

        //! a simple getter of the available path set between a start node and an end node
        vector<ADijkstraSol*> getOneAvailablePathSet(int start_id, int end_id);

        //! a simple getter of the size of available path set of a given start node and an end node
        int get_altsize_onepath(int start_id, int end_id);
};

#endif
