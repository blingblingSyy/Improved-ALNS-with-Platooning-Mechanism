#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include "example/model/basic/KSPBuilder.h"
#include "example/model/basic/ADijkstraSol.h"
#include "example/model/basic/config.h"
using namespace std;  

KSPBuilder::KSPBuilder(vector<vector<double>> distmat, int nodenum, int ksp_limit)
{
    init_distmat = distmat;
    node_num = nodenum;
    k_limit = ksp_limit;
    // AlternativeSetSize_PerPath.resize(node_num);
    SP_AllPaths.resize(node_num);
    KSP_AllPaths.resize(node_num);
    Dijkstra_AllPaths(); //generate SP_AllPaths
    ModifiedYen_AllPaths(); //generate KSP_AllPaths
}

KSPBuilder::~KSPBuilder()
{
    //! cannot delete the KSP_AllPaths memory, otherwise the Nodes cannot use the pointers
    // for(int i = 0; i < KSP_AllPaths.size(); i++)  //KSP_AllPaths will always include the shortest path solutions
    // {
    //     for(int j = 0; j < KSP_AllPaths[i].size(); j++)
    //     {
    //         if(i != j)
    //         {
    //             for(int z = 0; z < KSP_AllPaths[i][j].size(); z++)
    //             {
    //                 delete KSP_AllPaths[i][j][z];
    //             }
    //         }
    //     }
    // }
    // delete KSP_AllPaths[0][0][0];
}

void KSPBuilder::end()
{
    for(int i = 0; i < KSP_AllPaths.size(); i++)  //! KSP_AllPaths will always include the shortest path solutions
    {
        for(int j = 0; j < KSP_AllPaths[i].size(); j++)
        {
            if(i != j)
            {
                for(int z = 0; z < KSP_AllPaths[i][j].size(); z++)
                {
                    delete KSP_AllPaths[i][j][z];
                }
            }
        }
    }
    delete KSP_AllPaths[0][0][0];
}

void KSPBuilder::Dijsktra_body(vector<ADijkstraSol*> &SPset_fromstart, vector<int> &pred_fromstart, vector<vector<double>> input_distmat)
{
    //unvisited node
    vector<int> unvisited_nodes(node_num); //temporary list
    for(int i = 0; i < node_num; i++)
    {
        unvisited_nodes[i] = i;
    }
    
    auto compare = [&](int s, int r) {return SPset_fromstart[s]->getDist() < SPset_fromstart[r]->getDist();}; //! initial distance from the starting node

    //the main loop
    while(!unvisited_nodes.empty())
    {
        vector<int>::iterator iter = min_element(unvisited_nodes.begin(), unvisited_nodes.end(), compare);
        auto min_pos = iter - unvisited_nodes.begin();
        int min_val = unvisited_nodes[min_pos];
        // if(unvisited_nodes.size() < node_num && unvisited_nodes.size() > 1 && min_val == 0)
        unvisited_nodes.erase(iter);

        for(int r = 0; r < unvisited_nodes.size(); r++)
        {
            double alt_dist = SPset_fromstart[min_val]->getDist() + input_distmat[min_val][unvisited_nodes[r]]; //label + actual distance for two adjacent 
            if(alt_dist < SPset_fromstart[unvisited_nodes[r]]->getDist())
            {
                SPset_fromstart[unvisited_nodes[r]]->setDist(alt_dist); //update labels for neighbourhood nodes
                pred_fromstart[unvisited_nodes[r]] = min_val;  //update predecessors
            }
        }
    }
}

ADijkstraSol* KSPBuilder::Dijkstra_OnePath(int start, int end, vector<vector<double>> input_distmat) //calculate the shortest path distance between any two given nodes
{
    ADijkstraSol* SP_Start_End = new ADijkstraSol;
    //initialize predecessors and labels for SP tree from start node i
    vector<int> predecessors(node_num, -1);
    vector<ADijkstraSol*> initial_SPsol(node_num);  //shortest path structure from node i
    for(int j = 0; j < node_num; j++)
    {
        initial_SPsol[j] = new ADijkstraSol;
        if(j != start)
            initial_SPsol[j]->setDist(INF);
        else //j == start
            initial_SPsol[j]->setDist(0); //this is defaulted
    }
    
    Dijsktra_body(initial_SPsol, predecessors, input_distmat);
 
    SP_Start_End->setDist(initial_SPsol[end]->getDist());
    SP_Start_End->setPath(generate_onepath(end, predecessors));

    // for(int j = 0; j < node_num; j++)
    // {
    //     delete initial_SPsol[j];
    // }

    return SP_Start_End;
}

void KSPBuilder::Dijkstra_AllPaths()  //calculate the shortest path distance between any two nodes
{
    for(int i = 0; i < node_num; i++)
    {
        //initialize predecessors and labels for SP tree from start node i
        vector<int> predecessors(node_num, -1);
        //vector<vector<int>> SP_from_i;  //shortest path from node i to all other nodes
        SP_AllPaths[i].resize(node_num);  //shortest path structure from node i
        for(int j = 0; j < node_num; j++)
        {
            SP_AllPaths[i][j] = new ADijkstraSol;
            if(j != i)
                SP_AllPaths[i][j]->setDist(INF);
            else //j == i
                SP_AllPaths[i][j]->setDist(0);
        }
        
        Dijsktra_body(SP_AllPaths[i], predecessors, init_distmat);

        //SP_from_i = generate_paths_tree(predecessors);
        for(int z = 0; z < node_num; z++)
        {
            SP_AllPaths[i][z]->setPath(generate_onepath(z, predecessors));
            //! to eliminate the path containing the subtours with the depot
            if(i != z)
            {
                vector<int> temp_path = SP_AllPaths[i][z]->getPath();
                if(find(temp_path.begin()+1, temp_path.end()-1, 0) != temp_path.end()-1)
                {
                    SP_AllPaths[i][z]->setPath({});
                    SP_AllPaths[i][z]->setDist(INF);
                }
            }
        }
    }
}

vector<int> KSPBuilder::generate_onepath(int end, vector<int> pred_vecs)
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
vector<vector<int>> KSPBuilder::generate_paths_tree(vector<int> pred_vecs)
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

vector<ADijkstraSol*> KSPBuilder::ModifiedYen_OnePath(int start_node, int end_node)
{
    vector<ADijkstraSol*> KShortestPath_Start_End;  //A[] for Yen
    vector<ADijkstraSol*> KDeviationPath_Start_End; //B[] for Yen
    ADijkstraSol* SP1_Start_End = SP_AllPaths[start_node][end_node]; //start with the shortest path between the start node and the end node
    // if(SP1_Start_End.getPath().empty())
    // {
    //     return {};
    // }
    KDeviationPath_Start_End.push_back(SP1_Start_End); //the shortest path is put into B[]
    //! if KDeviationPath_Start_End is empty, it means the no valid candidate KSP solution is generated by deviating the current solution
    while(KShortestPath_Start_End.size() < k_limit && !KDeviationPath_Start_End.empty())
    {
        //! find the Dijsktra solution with the smallest distance in B[] and put it in A[]
        vector<ADijkstraSol*>::iterator iter = min_element(KDeviationPath_Start_End.begin(), KDeviationPath_Start_End.end()); //operator < is defined in ADijkstraSol
        ADijkstraSol* selected_KSPsol = *iter;
        KShortestPath_Start_End.push_back(selected_KSPsol);
        KDeviationPath_Start_End.erase(iter);

        int k = KShortestPath_Start_End.size();
        //! generate the current SP by modifying the last SP
        vector<int> last_SP = KShortestPath_Start_End[k-1]->getPath();
        for(int i = 0; i < last_SP.size()-1; i++) //for every deviation node in the shortest path except the end_node
        {
            //! find the disconnected path segment in the current Dijsktra solution
            vector<vector<double>> copy_distmat = init_distmat;
            int start_seg = last_SP[i]; //SP1_Start_End.KSP_Path[i];
            int end_seg = last_SP[i+1]; //SP1_Start_End.KSP_Path[i+1];
            //! initialize the current path segment
            vector<int> old_path = {};
            double old_dist = 0;
            for(int s = 0; s < i+1; s++) //when i = 2, the fixed part are the elements from i = 0 to i = 2 of the positions in the SP solution of the last ieteration
            {
                old_path.push_back(last_SP[s]); //the fixed path segment
            }
            for(int r = 0; r < old_path.size()-1; r++) //bug: old_path is empty with 0 size
            {
                old_dist += init_distmat[old_path[r]][old_path[r+1]]; //the total distance for the fixed path segment
            }
            //! modify the distance matrix by disconnecting specific links from the deviation node
            copy_distmat[start_seg][end_seg] = INF;
            //! find whether there are common deviation node in other solutions in A[] -> debug: need to ensure that the fixed part before the deviation node is the same
            for(int j = 0; j < k-1; j++) //A[]; k = KShortestPath_Start_End.size();
            {
                //for each path in A[] other than the current path
                vector<int> previous_SP = KShortestPath_Start_End[j]->getPath();
                auto iter = search(previous_SP.begin(), previous_SP.end(), old_path.begin(), old_path.end());
                if(iter != previous_SP.end()) //you can find the fixed part in other solutions in A[]
                {
                    int endnode_pos = iter + old_path.size() - previous_SP.begin();
                    if(previous_SP[endnode_pos] == start_seg && previous_SP[endnode_pos+1] != end_seg)
                    {
                        copy_distmat[start_seg][previous_SP[endnode_pos+1]] = INF;
                    }
                }
            }
            //! use Dijsktra to find a shortest path between start_seg and end_node based on the modified distance matrix
            ADijkstraSol* SPk_Start_End = Dijkstra_OnePath(start_seg, end_node, copy_distmat);
            vector<int> new_path = SPk_Start_End->getPath();
            //check 1: whether a new path can be generated
            if(new_path.empty()) //if cannot generate the new path because of node disconnectivity, do the next iteration (i+=1)
            {
                continue;
            }
            // //check 2: check if the new path contains depot(s) in the middle of the route -> but this will make even some shortest path solution infeasible
            // for(auto it = new_path.begin()+1; it != new_path.end()-1; it++)
            // {
            //     if((*it) == 0)
            //     {
            //         continue;
            //     }
            // }
            //! check 2: whether there are subtours in the  -> revise: subtour is allowed
            bool subtour = 0; //subtour is allowed
            // for(int w = 1; w < new_path.size(); w++) //w starts from 1 because the start node in new_path is the same as the end node in old_path
            // {
            //     if(find(old_path.begin(), old_path.end(), new_path[w]) != old_path.end())
            //     {
            //         //have subtours in the new path, discard it
            //         subtour += 1;
            //         break;
            //     }
            // }
            if(!subtour) //if no subtour
            {
                ADijkstraSol* tempSP_Start_End = new ADijkstraSol; 
                //! path reconnection
                old_path.insert(old_path.end(), new_path.begin()+1, new_path.end()); //bug: new_path is empty because of unconnected nodes
                tempSP_Start_End->setPath(old_path);
                //! calculate distance
                tempSP_Start_End->setDist(old_dist + SPk_Start_End->getDist());
                //! evaluate distance - whether falls into coupling range
                if(tempSP_Start_End->getDist() > SP1_Start_End->getDist()*10*1.0/9)
                {
                    continue; //jump out the current "for" cycle -> visit the next deviation node
                }
                //! else: put the temporary solution in B[] if the solution did not appear in both A[] and B[] before
                //! need to define == operator in struct Dijkstra Solution
                //! the pointers are different, but the contents pointed by the pointers may be the same
                bool appear_in_A = find_if(KShortestPath_Start_End.begin(), KShortestPath_Start_End.end(), [&](ADijkstraSol* sol){return *sol == *tempSP_Start_End;}) != KShortestPath_Start_End.end();
                bool appear_in_B = find_if(KDeviationPath_Start_End.begin(), KDeviationPath_Start_End.end(), [&](ADijkstraSol* sol){return *sol == *tempSP_Start_End;}) != KDeviationPath_Start_End.end();
                if(!appear_in_A && !appear_in_B)
                {
                    KDeviationPath_Start_End.push_back(tempSP_Start_End);
                }
            }
        }
    }

    for(int i = 0; i < KDeviationPath_Start_End.size(); i++)
    {
        delete KDeviationPath_Start_End[i];
    }

    return KShortestPath_Start_End;
}

void KSPBuilder::ModifiedYen_AllPaths()
{
    ADijkstraSol* empty_DijkstraSol = new ADijkstraSol; //defaulted: dist = 0; path = {}
    for(int i = 0; i < node_num; i++) //start_node
    {
        KSP_AllPaths[i].resize(node_num);
        KSP_AllPaths[i][i].push_back(empty_DijkstraSol); //! points to the same empty_DijkstraSol
        for(int j = 0; j < node_num; j++) //end_node
        {
            if(j != i)
            {
                KSP_AllPaths[i][j] = ModifiedYen_OnePath(i, j);
            }
        }
    }
}

vector<vector<ADijkstraSol*>> KSPBuilder::getAllShortestPaths()
{
    return SP_AllPaths;
}

vector<vector<double>> KSPBuilder::getAllShortestPathDistance()
{
    vector<vector<double>> SPdist_allpaths(node_num);
    for(int i = 0; i < node_num; i++)
    {
        SPdist_allpaths[i].resize(node_num);
        for(int j = 0; j < node_num; j++)
        {
            SPdist_allpaths[i][j] = SP_AllPaths[i][j]->getDist();
        }
    }
    return SPdist_allpaths;
}

vector<vector<int>> KSPBuilder::getAllShortestPathTimeMat(double speed)
{
    vector<vector<int>> SPtime_allpaths(node_num);
    for(int i = 0; i < node_num; i++)
    {
        SPtime_allpaths[i].resize(node_num);
        for(int j = 0; j < node_num; j++)
        {
            SPtime_allpaths[i][j] = int(round(SP_AllPaths[i][j]->getDist() / speed));
        }
    }
    return SPtime_allpaths;
}

ADijkstraSol* KSPBuilder::getOneShortestPath(int start_id, int end_id)
{
    return SP_AllPaths[start_id][end_id];
}

double KSPBuilder::getOneShortestPathDistance(int start_id, int end_id)
{
    return SP_AllPaths[start_id][end_id]->getDist();
}

vector<vector<vector<ADijkstraSol*>>> KSPBuilder::getAllAvailablePathSet()
{
    return KSP_AllPaths;
}

vector<ADijkstraSol*> KSPBuilder::getOneAvailablePathSet(int start_id, int end_id)
{
    return KSP_AllPaths[start_id][end_id];
}

int KSPBuilder::get_altsize_onepath(int start_id, int end_id)
{
    return KSP_AllPaths[start_id][end_id].size();
}