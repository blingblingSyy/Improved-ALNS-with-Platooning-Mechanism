#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include <fstream>
#include "DataDealer.h"
#include "AlternativePaths.h"
#include "ALNS_Parameters.h"
#include "utility.h"
#include "config.h"
using namespace std;  

/*BenchmarkInitializer: read and deal with the dataset in the file*/
BenchmarkInitializer::BenchmarkInitializer(string filepath)
{
    filename = filepath;
    file_row = 0;
    countrows();
    readdata();
    veh_num = data_vec[0][1];
    veh_type = set_mavtype(veh_num, PAS_MAV_PROP);
    veh_cap = set_mavcap(veh_type);
    veh_waitlim = set_mavwlim(veh_type);
    veh_speed = data_vec[0][2];
    max_dist = MAX_DIST;
    pl_max = PLEN_MAX;
    node_num = file_row-1;
    node_type = set_nodetype(node_num, PAS_REQ_PROP, true);
    extract_demands(node_type); //get demands
    dmd_type = set_demand_type(demands, node_type);
    match_mavset = set_matchmavs(node_type, veh_type);
    extract_coordinates(); //get coordinates
    initial_distmat = get_init_distmat(coordinates);
    modified_distmat = modify_init_distmat(initial_distmat);
    AlternativePaths altpathsets_obj(modified_distmat, node_num, ALTERSET_SIZE_K);
    SP_distmat = altpathsets_obj.get_Dijkstra_Dist_allpaths();
    altpath_sets = altpathsets_obj.get_alternatives_allpaths();
    neighbours = get_neighbours(modified_distmat, node_num);
    travel_tw = get_tvltw(SP_distmat[0], data_vec[1][5], veh_speed);
    extract_servetw(); //get modified service time windows for each node
    extract_servicetime(); //get service time for each node
}

//count the rows in the file
void BenchmarkInitializer::countrows()
{
    ifstream inputstr(filename);
	char chr;
	while(inputstr.get(chr))
	{
		if(chr == '\n')
			file_row++;
	}
	inputstr.close();
}

//read data from data source file
void BenchmarkInitializer::readdata()
{
	vector<int> raw_data;
	vector<int>::iterator it;
	ifstream file(filename);    //string filename = "lc101.txt";
	
	int num;
	while(file >> num)
	{
		raw_data.push_back(num);
	}
	file.close();

    it = raw_data.begin();
    data_vec.resize(file_row);
    while (it != raw_data.end())
    {
       for(int i = 0; i < file_row; i++)
        {
            if(i != 0)
            {
                data_vec[i].resize(9);
                for(int j = 0; j < 9; j++)
                {
                    data_vec[i][j] = *it;
                    it++;
                }
            }
            else  //i == 0
            {
                data_vec[i].resize(3);
                for(int j = 0; j < 2; j++)
                {
                    data_vec[i][j] = *it;
                    it++;
                }
            }
        }
    }
}

//extract and modify demands from the benchmark data
void BenchmarkInitializer::extract_demands(vector<int> nodetype)
{
    for(int i = 1; i < file_row; i++)
    {
        demands.push_back(data_vec[i][3]); //the first node is depot
    }
    modify_demand(demands, nodetype);
}

//extract coordinates from the benchmark data
void BenchmarkInitializer::extract_coordinates()
{
    for(int i = 1; i < file_row; i++)
    {
        coordinates.push_back({double(data_vec[i][1]), double(data_vec[i][2])}); //the first node is depot
    }
}

void BenchmarkInitializer::extract_servetw()
{
    for(int i = 1; i < file_row; i++)
    {
        int start_serve_early = data_vec[i][4];
        int start_serve_late = data_vec[i][5];
        int bypass_early = travel_tw[i-1][0];
        int bypass_late = travel_tw[i-1][1] - data_vec[i][6];  //data_vec[i][6]: const service time for the node
        service_tw.push_back({max(start_serve_early, bypass_early), min(start_serve_early, bypass_early)});
    }
}

void BenchmarkInitializer::extract_servicetime()
{
    for(int i = 1; i < file_row; i++)
    {
        servetime_const.push_back(data_vec[i][6]);
    }
}

//get the number of rows in the file
int BenchmarkInitializer::get_filerow()
{
    return file_row;
}

//get the number of mavs
int BenchmarkInitializer::get_veh_num()
{
    return veh_num;
}

//set vehicle type
vector<int> BenchmarkInitializer::get_veh_type()
{
    return veh_type;
}

vector<int> BenchmarkInitializer::get_veh_cap()
{
    return veh_cap;
}

vector<int> BenchmarkInitializer::get_veh_waitlim()
{
    return veh_waitlim;
}

//get vehicle speed
int BenchmarkInitializer::get_veh_speed()
{
    return veh_speed;
}

//get the maximum distance for each vehicle
int BenchmarkInitializer::get_max_dist()
{
    return max_dist;
}

//get the maximum platoon length set in the road network
int BenchmarkInitializer::get_plmax()
{
    return pl_max;
};


//get the number of nodes in the road network
int BenchmarkInitializer::get_node_num()  //including the depot node
{
    return node_num;
};

//get the passenger or freight types of each node in the road network
vector<int> BenchmarkInitializer::get_nodetype()
{
    return node_type;
};

vector<int> BenchmarkInitializer::get_demands()
{
    return demands;
}

//get the demand types of each node in the road network
vector<int> BenchmarkInitializer::get_dmdtype()
{
    return dmd_type;
};

vector<vector<int>> BenchmarkInitializer::get_matchmavs()
{
    return match_mavset;
}

vector<vector<double>> BenchmarkInitializer::get_coordinates()
{
    return coordinates;
}

//get the original distance matrix from the road network
vector<vector<double>> BenchmarkInitializer::get_initial_distmat()
{
    return initial_distmat;
}

vector<vector<double>> BenchmarkInitializer::get_modified_distmat()
{
    return modified_distmat;
}

vector<vector<double>> BenchmarkInitializer::get_SP_distmat()
{
    return SP_distmat;
}

vector<vector<vector<DijkstraOneSol>>> BenchmarkInitializer::get_altpath_set()
{
    return altpath_sets;
}

vector<vector<int>> BenchmarkInitializer::get_ajacent_nodes()
{
    return neighbours;
}

vector<vector<int>> BenchmarkInitializer::get_travel_tw()
{
    return travel_tw;
}

vector<vector<int>> BenchmarkInitializer::get_service_tw()
{
    return service_tw;
}

vector<int> BenchmarkInitializer::get_service_time()
{
    return servetime_const;
}

//initialize the amv structure
void BenchmarkInitializer::build_mav_struct(struct Vehicles &mavs)
{
    //initialize the information of mav_set
    mavs.veh_num = this->veh_num;
    mavs.veh_speed = this->veh_speed;
    mavs.max_range = this->max_dist;
    mavs.max_plen = this->pl_max;
    mavs.veh_type = this->veh_type;
    mavs.veh_cap = this->veh_cap;
    mavs.wait_lim = this->veh_waitlim;
}

//initialize the node structure
void BenchmarkInitializer::build_node_struct(struct Nodes &nodes, vector<int> mav_type)
{
    nodes.nodenum = this->node_num;
    nodes.nodetype = this->node_type;
    nodes.demands = this->demands;
    nodes.demand_type = this->dmd_type;
    nodes.neighbours = this->neighbours;
    nodes.initial_distmat = this->initial_distmat;
    nodes.initial_timemat = get_init_tvltime(nodes.initial_distmat, this->node_num, this->veh_speed);
    nodes.SP_distmat = this->SP_distmat;
    nodes.SP_timemat = get_init_tvltime(nodes.SP_distmat, this->node_num, this->veh_speed);
    nodes.avail_path_set = this->altpath_sets;
    nodes.service_tw = this->service_tw;
    nodes.travel_tw = this->travel_tw;
    nodes.service_time = this->servetime_const;
    nodes.match_mav = this->match_mavset;
    nodes.service_rate = set_serverate(node_type);
}


/*ResultWriter: write the results to the result file*/
// ResultWriter::ResultWriter(string filepath, string filename, vector<string> heuristic_names, vector<Solution> solution)
// {
//     file_path = filepath;
//     file_name = filename;
//     input_sols = solution;
//     heu_names = heuristic_names;
//     if(input_sols.size() != heu_names.size())
//     {
//         cerr << "Check the number of heuristics you input!";
//     }
// }

// //write the result to file 
// void ResultWriter::write_result()
// {
//     outfile.open(file_path, ios::out | ios::trunc);
//     outfile << "Instance name: " << file_name << endl;
//     for(int nsol = 0; nsol < static_cast<int>(input_sols.size()); nsol++)
//     {
//         outfile << endl;
//         outfile << heu_names[nsol] << ": "<< endl;
//         outfile << "  CPU time per iteration: " << input_sols[nsol].cpu_time << " seconds" << endl;
//         outfile << "  Routing Solution: " << endl;
//         for(int i = 0; i < input_sols[nsol].veh_num; i++)
//         {
//             outfile << "    " << "route " << i << ": ";
//             for(auto iter = input_sols[nsol].sol_container[i].begin(); iter != input_sols[nsol].sol_container[i].end()-1; iter++)
//             {
//                 outfile << *iter << " - ";
//             }
//             outfile << *(input_sols[nsol].sol_container[i].end()-1) << endl;;
//         }
//         outfile << "  Route loads: " << endl;
//         for(int i = 0; i < input_sols[nsol].veh_num; i++)
//         {
//             outfile << "    " << "route " << i << ": " << input_sols[nsol].route_load[i] << ";";
//         }
//         outfile << endl;
//         outfile << "  Route miles: " << endl;
//         for(int i = 0; i < input_sols[nsol].veh_num; i++)
//         {
//             outfile << "    " << "route " << i << ": " << input_sols[nsol].route_miles[i] << ";";
//         }
//         outfile << endl;
//         outfile << "  Objective value: " << input_sols[nsol].total_cost << endl;
//     }
//     outfile.close();
// }