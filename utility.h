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
vector<int> set_mavtype(int mav_num, double prob);
vector<int> set_mavcap(vector<int> mav_type);
vector<int> set_mavwlim(vector<int> mav_type);
double pl_factor(int length);
vector<int> set_nodetype(int node_num, double prob, bool add_intersects);
vector<int> randdemand(vector<int> nodetype);
void modify_demand(vector<int> &initial_dmd, vector<int> nodetype);
vector<int> set_demand_type(vector<int> demands, vector<int> nodetype);
vector<int> set_sertime_const(vector<int> node_type);
vector<double> set_serverate(vector<int> node_type);
vector<vector<int>> set_sertw(vector<vector<int>> tvl_tw, vector<int> nodetype);
vector<vector<int>> set_matchmavs(vector<int> nodetype, vector<int> mavtype);
double cal_euclid_dist(vector<double> x1_coord, vector<double> x2_coord);
vector<vector<double>> get_init_distmat(vector<vector<double>> coordinates);
vector<vector<double>> modify_init_distmat(vector<vector<double>> init_dist);
vector<vector<int>> get_neighbours(vector<vector<double>> init_dist, int node_num);
vector<vector<int>> get_init_tvltime(vector<vector<double>> init_dist, int node_num, double speed);
vector<vector<int>> get_tvltw(vector<double> source_dist, int plan_horizon, double speed);
double cal_path_dist(vector<int> route, vector<vector<double>> init_dist, int start_id, int end_id);
int cal_path_tvltime(vector<int> route, vector<vector<int>> init_timemat, int start_id, int end_id);
vector<int> find_servednode_id(vector<int> labels); //find the indices for the served nodes

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
