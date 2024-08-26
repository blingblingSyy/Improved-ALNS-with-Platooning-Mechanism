#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <cstring>
#include <algorithm>
#include <math.h>
#include <numeric>
#include <functional>
#include "couplingVRP/model/basic/config.h"
#include "utility.h"

using namespace std;

RandomNumber::RandomNumber()
{
    srand((unsigned)time(NULL));
}

int RandomNumber::get_rint(int begin, int end) //[begin, end]
{
    return rand() % (end - begin + 1) + begin;
}

float RandomNumber::get_rflt(int begin, int end) //[begin, end]
{
    return begin + float(rand()) / float(RAND_MAX) * (end - begin); //[begin, end]
}

double calEuclidDist(vector<double> x1_coord, vector<double> x2_coord)
{
    return sqrt((x1_coord[0] - x2_coord[0])*(x1_coord[0] - x2_coord[0]) + (x1_coord[1] - x2_coord[1])*(x1_coord[1] - x2_coord[1]));
}

double calPathDist(vector<int> route, vector<vector<double>> init_dist, int start_id, int end_id)
{
    double dist_path = 0.0;
    for(int i = start_id; i < end_id; i++)
    {
        int arc_start = route[i];
        int arc_end = route[i+1];
        dist_path += init_dist[arc_start][arc_end];
    }
    return dist_path;
}


//calculate the travel time between two nodes in an extended route
int calPathTvlTime(vector<int> route, vector<vector<int>> init_timemat, int start_id, int end_id)
{
    int tvltime_path = 0.0;
    for(int i = start_id; i < end_id; i++)
    {
        int arc_start = route[i];
        int arc_end = route[i+1];
        tvltime_path += init_timemat[arc_start][arc_end];
    }
    return tvltime_path;
}

// //find the position of elements of vector 2 in vector 1
// vector<int> intersect_pos(vector<vector<vector<int>>> sort_arcset, vector<vector<vector<int>>> sort_arcsubset)
// {
//     vector<int> pos_in_arc1;
//     vector<vector<vector<int>>>::iterator first1 = sort_arcset.begin();
//     vector<vector<vector<int>>>::iterator last1 = sort_arcset.end();
//     vector<vector<vector<int>>>::iterator first2 = sort_arcsubset.begin();
//     vector<vector<vector<int>>>::iterator last2 = sort_arcsubset.end();
//     while(first1 != last1 && first2 != last2)
//     {
//         if(*first1 < *first2)
//             ++first1;
//         else
//         {
//             if(!(*first2 < *first1))
//             {
//                 pos_in_arc1.push_back(static_cast<int>(first1 - sort_arcset.begin()));
//                 ++first1;
//             }
//             ++first2;
//         }
//     }
//     return pos_in_arc1;
// }