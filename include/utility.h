// utility.h

#ifndef _UTILITY_H_    //include guards
#define _UTILITY_H_

#include <vector>
#include <algorithm>
#include <string>
// #include <bits/stdc++.h>
using namespace std;

/*file paths*/
// const string ins100_dir = "C:\\Users\\SYY\\Improved_ALNS_Git\\Li_Lim_Benchmark\\pdp_100";  // the directory for benchmark instances
const string base_dir = "C:/Users/SYY/Improved_ALNS_Git/";
const string result_dir = "C:/Users/SYY/Improved_ALNS_Git/result/";   // the directory for result files
const string dataset_dir = "C:/Users/SYY/Improved_ALNS_Git/dataset/";   // the directory for dataset files

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

//! transform a double-type vector to int-type vector
vector<int> convert_vectype_to_int(vector<double> double_vec);

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

// Function to generate a normal distribution
template<typename T>
vector<double> generateNormalData(int numSamples, T mean, T stddev, double min_bound = -INF, double max_bound = INF)
{
    vector<double> data;
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> dist(mean, stddev);

    for (int i = 0; i < numSamples; ++i) 
    {
        double x;
        do {
            x = dist(gen);
        } while (x < min_bound || x > max_bound);
        data.push_back(x);
    }

    return data;
}

// Function to generate a multi-modal dataset
template<typename T>
vector<double> generateMultiModalData(vector<int> numSamplesPerMode, const vector<T>& means, const vector<T>& stddevs, double min_bound = -INF, double max_bound = INF)
{
    vector<double> data;

    // Generate data for each mode
    for (size_t i = 0; i < means.size(); ++i) 
    {
        vector<double> modeData = generateNormalData(numSamplesPerMode[i], means[i], stddevs[i], min_bound, max_bound);
        data.insert(data.end(), modeData.begin(), modeData.end());
    }

    // Shuffle the dataset to interleave data points from different modes
    random_device rd;
    mt19937 g(rd());
    shuffle(data.begin(), data.end(), g);

    return data;
}

//! sort one vector (vec_tosort) based on another vector (vec_fromsort)
template<typename T1, typename T2>
void sortVec(vector<T1>& vec_tosort, vector<T2>& vec_fromsort, bool greater)
{
    vector<int> indices(vec_tosort.size());
    iota(indices.begin(), indices.end(), 0);
    if(greater)
    {
        sort(indices.begin(), indices.end(), [&](int a, int b)->bool {return vec_fromsort[a] > vec_fromsort[b];});
    }
    else
    {
        sort(indices.begin(), indices.end(), [&](int a, int b)->bool {return vec_fromsort[a] < vec_fromsort[b];});
    }
    for(int id: indices)
    {
        vec_tosort.push_back(vec_tosort[id]);
        // vec_fromsort.push_back(vec_fromsort[id]);
    }
    vec_tosort.erase(vec_tosort.begin(), vec_tosort.begin()+indices.size());
    // vec_fromsort.erase(vec_fromsort.begin(), vec_fromsort.begin()+indices.size());
}

#endif
