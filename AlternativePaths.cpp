#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include "AlternativePaths.h"
#include "config.h"
using namespace std;  

/*AlternativePaths: create alternative path sets for every pair of nodes*/
AlternativePaths::AlternativePaths(vector<vector<double>> distmat, int nodenum, int ksp_limit)
{
    init_distmat = distmat;
    node_num = nodenum;
    k_limit = ksp_limit;
    AlternativeSetSize_PerPath.resize(node_num);
    SP_AllPaths.resize(node_num);
    KSP_AllPaths.resize(node_num);
    Dijkstra_AllPaths(); //generate SP_AllPaths
    ModifiedYen_AllPaths(); //generate KSP_AllPaths
}

//the main body of Dijkstra algorithm
void AlternativePaths::Dijsktra_body(vector<DijkstraOneSol> &SPset_fromstart, vector<int> &pred_fromstart, vector<vector<double>> input_distmat)
{
    //unvisited node
    vector<int> unvisited_nodes(node_num); //temporary list
    for(int i = 0; i < node_num; i++)
    {
        unvisited_nodes[i] = i;
    }
    
    auto compare = [&](int s, int r) {return SPset_fromstart[s].SP_Dist < SPset_fromstart[r].SP_Dist;};

    //the main loop
    while(!unvisited_nodes.empty())
    {
        vector<int>::iterator iter = min_element(unvisited_nodes.begin(), unvisited_nodes.end(), compare);
        auto min_pos = iter - unvisited_nodes.begin();
        int min_val = unvisited_nodes[min_pos];
        unvisited_nodes.erase(iter);

        for(int r = 0; r < unvisited_nodes.size(); r++)
        {
            double alt_dist = SPset_fromstart[min_val].SP_Dist + input_distmat[min_val][unvisited_nodes[r]]; //label + actual distance for two adjacent 
            if(alt_dist < SPset_fromstart[unvisited_nodes[r]].SP_Dist)
            {
                SPset_fromstart[unvisited_nodes[r]].SP_Dist = alt_dist; //update labels for neighbourhood nodes
                pred_fromstart[unvisited_nodes[r]] = min_val;  //update predecessors
            }
        }
    }
}

//use Dijkstra algorithm to calculate the shortest path distance between any two given nodes
DijkstraOneSol AlternativePaths::Dijkstra_OnePath(int start, int end, vector<vector<double>> input_distmat)
{
    DijkstraOneSol SP_Start_End;
    //initialize predecessors and labels for SP tree from start node i
    vector<int> predecessors(node_num, -1);
    vector<DijkstraOneSol> initial_SPsol(node_num);  //shortest path structure from node i
    for(int j = 0; j < node_num; j++)
    {
        if(j != start)
            initial_SPsol[j].SP_Dist = INF;
        else //j == start
            initial_SPsol[j].SP_Dist = 0;
    }
    
    Dijsktra_body(initial_SPsol, predecessors, input_distmat);
 
    SP_Start_End.SP_Dist = initial_SPsol[end].SP_Dist;
    SP_Start_End.SP_Path = generate_onepath(end, predecessors);

    return SP_Start_End;
}

//use Dijkstra algorithm to calculate the shortest path distance between any two nodes
void AlternativePaths::Dijkstra_AllPaths()
{
    for(int i = 0; i < node_num; i++)
    {
        //initialize predecessors and labels for SP tree from start node i
        vector<int> predecessors(node_num, -1);
        //vector<vector<int>> SP_from_i;  //shortest path from node i to all other nodes
        SP_AllPaths[i].resize(node_num);  //shortest path structure from node i
        for(int j = 0; j < node_num; j++)
        {
            if(j != i)
                SP_AllPaths[i][j].SP_Dist = INF;
            else //j == i
                SP_AllPaths[i][j].SP_Dist = 0;
        }
        
        Dijsktra_body(SP_AllPaths[i], predecessors, init_distmat);
        
        //SP_from_i = generate_paths_tree(predecessors);
        for(int z = 0; z < node_num; z++)
        {
            //SP_AllPaths[i][z].SP_Path = SP_from_i[z];
            SP_AllPaths[i][z].SP_Path = generate_onepath(z, predecessors);
        }
    }
}

//generate a shortest path starting from a given source node to a given end node
vector<int> AlternativePaths::generate_onepath(int end, vector<int> pred_vecs)
{
    vector<int> path_track;
    if(pred_vecs[end] != -1)
    {
        while(end != -1)
        {
            path_track.push_back(end);
            end = pred_vecs[end];
        }
        reverse(path_track.begin(), path_track.end());
    }
    else //pred_vecs[end] == -1
    {
        path_track = {};
    }
    return path_track;
}

//generate all shortest paths starting from a given node according to the predecessor vector
vector<vector<int>> AlternativePaths::generate_paths_tree(vector<int> pred_vecs)
{
    vector<vector<int>> SPT_pathset; //store the shortest paths starting from start node to all the other nodes
    auto paths_size = pred_vecs.size();
    SPT_pathset.resize(paths_size);

    // vector<int>::iterator start_iter = find(pred_vecs.begin(), pred_vecs.end(), -1);
    // int start_pos = start_iter - pred_vecs.begin();
    
    for(int i = 0; i < paths_size; i++) //for each end node
    {
        SPT_pathset[i] = generate_onepath(i, pred_vecs);
    }
    return SPT_pathset;
}

//Find K's shortest path with Yen's algorithm given a start and end node
vector<DijkstraOneSol> AlternativePaths::ModifiedYen_OnePath(int start_node, int end_node)
{
    vector<DijkstraOneSol> KShortestPath_Start_End;  //A[] for Yen
    vector<DijkstraOneSol> KDeviationPath_Start_End; //B[] for Yen
    DijkstraOneSol SP1_Start_End;
    SP1_Start_End = SP_AllPaths[start_node][end_node];
    KShortestPath_Start_End.push_back(SP1_Start_End); //the shortest path is put into A[]
    //define a compare operator to find the solution with the smallest distance in KDeviationPath_Start_End
    auto compare = [&](int s, int r) {return KDeviationPath_Start_End[s].SP_Dist < KDeviationPath_Start_End[r].SP_Dist;};
    int k = 1;
    while(k < k_limit && KShortestPath_Start_End.size() == k) 
    //if KShortestPath_Start_End.size() < k, it means the last step does not generate any available k-shortest path, then the algorithm stops 
    {
        //generate the current SP by modifying the last SP
        for(int i = 0; i < KShortestPath_Start_End[k-1].SP_Path.size()-1; i++) //for every path segment (deviation node) in the shortest path
        {
            DijkstraOneSol tempSP_Start_End;
            //find the disconnected path segment in the current Dijsktra solution
            vector<vector<double>> copy_distmat = init_distmat;
            int start_seg = SP1_Start_End.SP_Path[i];
            int end_seg = SP1_Start_End.SP_Path[i+1];
            //initialize the current path segment
            vector<int> old_path = {};
            double old_dist = 0;
            for(int s = 0; s < i; s++)
            {
                old_path.push_back(s); //the fixed path segment
            }
            for(int r = 0; r < old_path.size()-1; r++)
            {
                old_dist += init_distmat[r][r+1]; //the total distance for the fixed path segment
            }
            //modify the distance matrix by disconnecting specific links from the deviation node
            copy_distmat[start_seg][end_seg] = INF;
            //find whether there are common deviation node in other solutions in A[]
            for(int j = 0; j < KShortestPath_Start_End.size() && j != k-1; j++)
            {
                for(int p = 0; p < KShortestPath_Start_End[j].SP_Path.size()-1; p++)
                {
                    if(KShortestPath_Start_End[j].SP_Path[p] == start_seg)
                    {
                        int anther_end_seg = KShortestPath_Start_End[j].SP_Path[p+1];
                        copy_distmat[start_seg][anther_end_seg] = INF;
                    }
                }
            }
            //use Dijsktra to find a shortest path between start_seg and end_node based on the modified distance matrix
            DijkstraOneSol SPk_Start_End = Dijkstra_OnePath(start_seg, end_node, copy_distmat);
            vector<int> new_path = SPk_Start_End.SP_Path;
            //check whether there are subtours in the path
            bool subtour = 0;
            for(int w = 0; w < new_path.size(); w++)
            {
                if(find(old_path.begin(), old_path.end(), new_path[w]) != old_path.end())
                {
                    //have subtours in the new path, discard it
                    subtour += 1;
                }
            }
            if(!subtour) //if no subtour
            {
                //path reconnection
                old_path.insert(old_path.end(), new_path.begin(), new_path.end());
                tempSP_Start_End.SP_Path = old_path;
                //calculate distance
                tempSP_Start_End.SP_Dist = old_dist + SPk_Start_End.SP_Dist;
                //evaluate distance - whether falls into coupling range
                if(tempSP_Start_End.SP_Dist > SP1_Start_End.SP_Dist*10*1.0/9)
                {
                    continue; //jump out the current "for" cycle
                }
                //else: put the temporary solution in B[] if the solution did not appear before
                if(find(KDeviationPath_Start_End.begin(), KDeviationPath_Start_End.end(), tempSP_Start_End) == KDeviationPath_Start_End.end())
                {
                    KDeviationPath_Start_End.push_back(tempSP_Start_End);
                }
                // else
                // {
                //     continue;
                // }
            }
        }
        //find the Dijsktra solution with the smallest distance in B[] and put it in A[]. Ties are broken for most intermediate nodes.
        if(!KDeviationPath_Start_End.empty())
        {
            vector<DijkstraOneSol>::iterator iter = min_element(KDeviationPath_Start_End.begin(), KDeviationPath_Start_End.end(), compare);
            auto min_pos = iter - KDeviationPath_Start_End.begin();
            DijkstraOneSol selected_KSPsol = KDeviationPath_Start_End[min_pos];
            KShortestPath_Start_End.push_back(selected_KSPsol);
            KDeviationPath_Start_End.erase(iter);
        }
        k += 1;
    }
    return KShortestPath_Start_End;
}

void AlternativePaths::ModifiedYen_AllPaths()
{
    DijkstraOneSol empty_DijkstraSol;
    empty_DijkstraSol.SP_Dist = 0;
    empty_DijkstraSol.SP_Path = {};
    for(int i = 0; i < node_num; i++) //start_node
    {
        KSP_AllPaths[i].resize(node_num);
        for(int j = 0; j < node_num && j != i; j++) //end_node
        {
            KSP_AllPaths[i][j] = ModifiedYen_OnePath(i, j); //check when j == i
        }
        KSP_AllPaths[i][i].push_back(empty_DijkstraSol);
    }
}

vector<vector<DijkstraOneSol>> AlternativePaths::get_Dijkstra_allpaths()
{
    return SP_AllPaths;
}

vector<vector<double>> AlternativePaths::get_Dijkstra_Dist_allpaths()
{
    vector<vector<double>> SPdist_allpaths(node_num);
    for(int i = 0; i < node_num; i++)
    {
        SPdist_allpaths[i].resize(node_num);
        for(int j = 0; j < node_num; j++)
        {
            SPdist_allpaths[i][j] = SP_AllPaths[i][j].SP_Dist;
        }
    }
}

DijkstraOneSol AlternativePaths::get_Dijkstra_onepath(int start_id, int end_id)
{
    return SP_AllPaths[start_id][end_id];
}

double AlternativePaths::get_Dijkstra_Dist_onepath(int start_id, int end_id)
{
    return SP_AllPaths[start_id][end_id].SP_Dist;
}

vector<vector<vector<DijkstraOneSol>>> AlternativePaths::get_alternatives_allpaths()
{
    return KSP_AllPaths;
}

vector<DijkstraOneSol> AlternativePaths::get_alternatives_onepath(int start_id, int end_id)
{
    return KSP_AllPaths[start_id][end_id];
}

vector<double> AlternativePaths::get_alternatives_dist_onepath(int start_id, int end_id)
{
    vector<double> altdist_start_end;
    for(int i = 0; i < KSP_AllPaths[start_id][end_id].size(); i++)
    {
        altdist_start_end.push_back(KSP_AllPaths[start_id][end_id][i].SP_Dist);
    }
    return altdist_start_end;
}

vector<vector<int>> AlternativePaths::get_altsize_allpaths()
{
    vector<vector<int>> altsize_vecs(node_num);
    for(int i = 0; i < node_num; i++)
    {
        altsize_vecs[i].resize(node_num);
        for(int j = 0; j < node_num; j++)
        {
            altsize_vecs[i][j] = KSP_AllPaths[i][j].size();
        }
    }
    return altsize_vecs;
}

int AlternativePaths::get_altsize_onepath(int start_id, int end_id)
{
    return KSP_AllPaths[start_id][end_id].size();
}