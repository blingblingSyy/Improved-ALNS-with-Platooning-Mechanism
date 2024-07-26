// utility.h

#ifndef _UTILITY_H_    //include guards
#define _UTILITY_H_

#include <vector>
// #include <bits/stdc++.h>
using namespace std;

class RandomNumber  
{  
     public:  
          RandomNumber();
          int get_rint(int begin = 0, int end = 1);
          float get_rflt(int begin = 0, int end = 1);
}; 

double pl_factor(int length);
double cal_euclid_dist(vector<double> x1_coord, vector<double> x2_coord);
double cal_path_dist(vector<int> route, vector<vector<double>> init_dist, int start_id, int end_id);
int cal_path_tvltime(vector<int> route, vector<vector<int>> init_timemat, int start_id, int end_id);


//Erasing multiple elements with specified positions from a vector (but no need to retain the original order of the elements)
//Assign the current last element of the vector to the element to erase, then erase the last element. 
//This will avoid big moves and all indexes except the last will remain constant.
//If you start erasing from the back, all precomputed indexes will be correct.
template<typename T>
void quickDelete(vector<T> &vec, int idx)
{
    vec[idx] = vec.back();
    vec.pop_back();
}


//remove the common elements of vector b from vector a -> retain the intersections of both vectors
//update both vector a and vector b
template<typename T>
void remove_intersection(vector<T>& a, vector<T>& b)
{
    // unordered_multiset<T> st;
    vector<T> ab = a;
    ab.insert(ab.end(), b.begin(), b.end());
    auto predicate = [&](const T &k){ return count(ab.begin(), ab.end(), k) > 1; };
    a.erase(std::remove_if(a.begin(), a.end(), predicate), a.end());
    b.erase(std::remove_if(b.begin(), b.end(), predicate), b.end());
}

#endif
