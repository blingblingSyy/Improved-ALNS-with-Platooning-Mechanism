#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <cstring>
#include <algorithm>
#include <math.h>
#include "config.h"
#include "utility.h"

using namespace std;


//generate random number with RandomNumber class
RandomNumber::RandomNumber()
{
    srand((unsigned)time(NULL));
}

int RandomNumber::get_rint(int begin, int end)
{
    return rand() % (end - begin + 1) + begin;
}

float RandomNumber::get_rflt(int begin, int end)
{
    return begin + float(rand()) / float(RAND_MAX) * (end - begin);
}

//calculate the distance reduction factor for a platoon of specific length
double pl_factor(int length)
{
    if(length <= 0)
    {
        throw "Division by zero or negative condition!";
    }
    return double(1+0.90*(length-1)) / double(length);
}

//calculate Euclidean distance between any two nodes
double cal_euclid_dist(vector<double> x1_coord, vector<double> x2_coord)
{
    return sqrt((x1_coord[0] - x2_coord[0])*(x1_coord[0] - x2_coord[0]) + (x1_coord[1] - x2_coord[1])*(x1_coord[1] - x2_coord[1]));
}

//calculate the distance between two nodes in an extended route
double cal_path_dist(vector<int> route, vector<vector<double>> init_dist, int start_id, int end_id)
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
int cal_path_tvltime(vector<int> route, vector<vector<int>> init_timemat, int start_id, int end_id)
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

// //remove the intersection part of two vectors
// void remove_intersection(vector<int>& a, vector<int>& b){
//     unordered_multiset<int> st;
//     st.insert(a.begin(), a.end());
//     st.insert(b.begin(), b.end());
//     auto predicate = [&st](const int& k){return st.count(k) > 1; };
//     a.erase(remove_if(a.begin(), a.end(), predicate), a.end());
//     b.erase(remove_if(b.begin(), b.end(), predicate), b.end());
// }

// //find the common path segments of two routes
// vector<vector<vector<int>>> intersect_arcs(vector<vector<vector<int>>> sort_arcset1, vector<vector<vector<int>>> sort_arcset2)
// {
//     vector<vector<vector<int>>> common_set;
//     set_intersection(sort_arcset1.begin(), sort_arcset1.end(), sort_arcset2.begin(), sort_arcset2.end(), back_inserter(common_set));
//     return common_set;
// }


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