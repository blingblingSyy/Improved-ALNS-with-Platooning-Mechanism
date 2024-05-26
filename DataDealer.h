// DataReader.h

#ifndef _DATAREADER_H_    //include guards
#define _DATAREADER_H_

#include <vector>
#include <string>
#include <fstream>
#include "config.h"
using namespace std;

//Read data from the instances
class BenchmarkInitializer  //deal with both nodes and vehicles and make modification based on the benchmark dataset
{
    private: 
        string filename;
        int file_row;
        vector<vector<int>> data_vec;
        int veh_num;
        vector<int> veh_type;
        vector<int> veh_cap;
        vector<int> veh_waitlim;
        int veh_speed;
        int max_dist;  //need to be set
        int pl_max; //need to be set
        int node_num;
        vector<int> node_type;
        vector<int> demands;    //negative if the request is a delivery request
        vector<int> dmd_type;
        vector<vector<int>> match_mavset;
        vector<vector<double>> coordinates;
        vector<vector<double>> initial_distmat;
        vector<vector<double>> modified_distmat;
        vector<vector<double>> SP_distmat;
        vector<vector<vector<DijkstraOneSol>>> altpath_sets;
        vector<vector<int>> neighbours;
        vector<vector<int>> travel_tw;
        vector<vector<int>> service_tw;
        vector<int> servetime_const; 
        void countrows();
        void readdata();
        void extract_demands(vector<int> nodetype);
        void extract_coordinates();
        void extract_servetw();
        void extract_servicetime();
    public:
        BenchmarkInitializer(string filepath);
        int get_filerow();
        int get_veh_num();
        vector<int> get_veh_type();
        vector<int> get_veh_cap();
        vector<int> get_veh_waitlim();
        int get_veh_speed();
        int get_max_dist();
        int get_plmax();
        int get_node_num();
        vector<int> get_nodetype();
        vector<int> get_demands();
        vector<int> get_dmdtype();
        vector<vector<int>> get_matchmavs();
        vector<vector<double>> get_coordinates();
        vector<vector<double>> get_initial_distmat();
        vector<vector<double>> get_modified_distmat();
        vector<vector<double>> get_SP_distmat();
        vector<vector<vector<DijkstraOneSol>>> get_altpath_set();
        vector<vector<int>> get_ajacent_nodes();
        vector<vector<int>> get_travel_tw();
        vector<vector<int>> get_service_tw();
        vector<int> get_service_time();
        void build_mav_struct(struct Vehicles &mavs);
        void build_node_struct(struct Nodes &nodes, vector<int> mav_type);
};


// //Write algorithm results to files
// class ResultWriter
// {
//     private: 
//         ofstream outfile;
//         string file_name;
//         string file_path;
//         vector<Solution> input_sols;
//         vector<string> heu_names;
//     public:
//         ResultWriter(string resultpath, string filename, vector<string> heuristic_names, vector<Solution> solution);
//         void write_result();
// };

#endif
