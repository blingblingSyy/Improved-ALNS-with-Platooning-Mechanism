// AlternativePaths.h

#ifndef _ALTERNATIVEPATHS_H_    //include guards
#define _ALTERNATIVEPATHS_H_

#include <vector>
#include "config.h"
using namespace std;

//Read data from the instances
class AlternativePaths
{
    private: 
        vector<vector<double>> init_distmat;
        int node_num;
        int k_limit;
        vector<vector<DijkstraOneSol>> SP_AllPaths; 
        vector<vector<vector<DijkstraOneSol>>> KSP_AllPaths;  //A[] for Yen
        vector<vector<int>> AlternativeSetSize_PerPath;  //the size of alternative paths for each path
        void Dijsktra_body(vector<DijkstraOneSol> &SPset_fromstart, vector<int> &pred_fromstart, vector<vector<double>> input_distmat);
        DijkstraOneSol Dijkstra_OnePath(int start, int end, vector<vector<double>> input_distmat); //a Shortest Path tree from start node
        void Dijkstra_AllPaths(); //Shortest Path Matrix for all paths
        vector<vector<int>> generate_paths_tree(vector<int> pred_vecs);
        vector<int> generate_onepath(int end, vector<int> pred_vecs);
        
    public:
        AlternativePaths(vector<vector<double>> distmat, int nodenum, int ksp_limit);
        vector<DijkstraOneSol> ModifiedYen_OnePath(int start_node, int end_node); 
        void ModifiedYen_AllPaths(); 
        vector<vector<DijkstraOneSol>> get_Dijkstra_allpaths();
        vector<vector<double>> get_Dijkstra_Dist_allpaths();
        DijkstraOneSol get_Dijkstra_onepath(int start_id, int end_id);
        double get_Dijkstra_Dist_onepath(int start_id, int end_id);
        vector<vector<vector<DijkstraOneSol>>> get_alternatives_allpaths();
        vector<DijkstraOneSol> get_alternatives_onepath(int start_id, int end_id);
        vector<double> get_alternatives_dist_onepath(int start_id, int end_id);
        vector<vector<int>> get_altsize_allpaths();
        int get_altsize_onepath(int start_id, int end_id);
};

#endif
