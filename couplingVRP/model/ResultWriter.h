// DataReader.h

#ifndef _DATAREADER_H_    //include guards
#define _DATAREADER_H_

#include <vector>
#include <string>
#include <fstream>
#include "config.h"
using namespace std;

class NodesManager
{
    protected:
    //setting mavs
    vector<int> set_mavtype(int mav_num, double prob);
    vector<int> set_mavcap(vector<int> mav_type);
    vector<int> set_mavwlim(vector<int> mav_type);

    //setting nodes
    vector<int> set_nodetype(int node_num, double prob, bool add_intersects);
    vector<int> randdemand(vector<int> nodetype);
    void modify_demand(vector<int> &initial_dmd, vector<int> nodetype, bool modify_pasdmd);
    vector<int> set_demand_type(vector<int> demands, vector<int> nodetype);
    vector<int> set_sertime_const(vector<int> node_type);
    vector<double> set_serverate(vector<int> node_type);
    vector<vector<int>> set_sertw(vector<vector<int>> tvl_tw, vector<int> nodetype);
    vector<vector<int>> set_matchmavs(vector<int> nodetype, vector<int> mavtype);
    vector<vector<int>> get_tvltw(vector<double> source_dist, int plan_horizon, double speed);
    vector<vector<int>> get_neighbours(vector<vector<double>> init_dist, int node_num);
    
    //setting matrices
    vector<vector<double>> get_init_distmat(vector<vector<double>> coordinates);
    vector<vector<double>> modify_init_distmat(vector<vector<double>> init_dist);
    vector<vector<int>> get_init_tvltime(vector<vector<double>> init_dist, int node_num, double speed);
    
    public:
    NodesManager();

};

//Read data from the instances
class BenchmarkInitializer: public NodesManager  //deal with both nodes and vehicles and make modification based on the benchmark dataset
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
        vector<vector<vector<ADijkstraSol>>> altpath_sets;
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
        BenchmarkInitializer(string filepath, bool add_intersects = true, bool modify_dist = true, bool modify_pasdmd = true);
        int getRowNum();
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
        vector<vector<vector<ADijkstraSol>>> get_altpath_set();
        vector<vector<int>> get_ajacent_nodes();
        vector<vector<int>> get_travel_tw();
        vector<vector<int>> get_service_tw();
        vector<int> get_service_time();
        Vehicles get_mav_struct();
        Nodes get_node_struct();
};


//Write algorithm results to files
class ResultWriter
{
    private: 
        ofstream outfile;
        string file_name;
        string file_path;
        Solution input_sol;
        string solution_stage;
        vector<double> cpu;
        // vector<string> heu_names;
    public:
        ResultWriter(string resultpath, string filename, Solution solution, vector<double> cpu_time, string stage_name);
        void write_result();
        void record_solution();
};

#endif