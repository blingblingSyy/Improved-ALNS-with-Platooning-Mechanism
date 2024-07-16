// utility.h

#ifndef _UTILITY_H_    //include guards
#define _UTILITY_H_

#include <vector>
#include <bits/stdc++.h>
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

// void remove_intersection(vector<int>& a, vector<int>& b); //remove the intersection part of two vectors

template<typename T>
void remove_intersection(vector<T>& a, vector<T>& b)
{
    unordered_multiset<T> st;
    st.insert(a.begin(), a.end());
    st.insert(b.begin(), b.end());
    auto predicate = [&st](const T& k){ return st.count(k) > 1; };
    a.erase(std::remove_if(a.begin(), a.end(), predicate), a.end());
    b.erase(std::remove_if(b.begin(), b.end(), predicate), b.end());
}

#endif
