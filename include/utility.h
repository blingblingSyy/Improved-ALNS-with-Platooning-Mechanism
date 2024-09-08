// utility.h

#ifndef _UTILITY_H_    //include guards
#define _UTILITY_H_

#include <vector>
#include <algorithm>
// #include <bits/stdc++.h>
using namespace std;

/// @brief a class to generate random number of given type from a specific interval
class RandomNumber  
{  
     public:  
          RandomNumber();
          int get_rint(int begin = 0, int end = 1);
          float get_rflt(int begin = 0, int end = 1);
}; 

//! calculate the average number of an numeric vector
template<typename T>
double calAverage(vector<T> vec)
{
    double average = accumulate(vec.begin(), vec.end(), 0.0) / vec.size();
    return average;
}

//! calculate the euclidean distance between two coordinates
double calEuclidDist(vector<double> x1_coord, vector<double> x2_coord);

//! calculate the distance of a path from a specific start node and end node position
double calPathDist(vector<int> route, vector<vector<double>> init_dist, int start_id, int end_id);

//! calculate the travel time of a path from a specific start node and end node position
int calPathTvlTime(vector<int> route, vector<vector<int>> init_timemat, int start_id, int end_id);

//! Erasing multiple elements with specified positions from a vector 
//! (but no need to retain the original order of the elements)
//! Assign the current last element of the vector to the element to erase, then erase the last element. 
//! If you start erasing from the back, all precomputed indexes will be correct.
template<typename T>
void quickDelete(vector<T> &vec, int idx)
{
    vec[idx] = vec.back();
    vec.pop_back();
}

//! remove (keep == false) or retain (keep == true) the intersections of both vectors
//! update both vector a and vector b
template<typename T>
void process_intersections(vector<T>& a, vector<T>& b, bool keep = false)
{
    // unordered_multiset<T> st;
    vector<T> ab = a;
    ab.insert(ab.end(), b.begin(), b.end());
    if(!keep) //remove
    {
        auto predicate = [&](const T &k){ return count(ab.begin(), ab.end(), k) > 1; };
        a.erase(std::remove_if(a.begin(), a.end(), predicate), a.end());
        b.erase(std::remove_if(b.begin(), b.end(), predicate), b.end());
    }
    else //keep
    {
        auto predicate = [&](const T &k){ return count(ab.begin(), ab.end(), k) <= 1; };
        a.erase(std::remove_if(a.begin(), a.end(), predicate), a.end());
        b.erase(std::remove_if(b.begin(), b.end(), predicate), b.end());
    }
}

#endif
