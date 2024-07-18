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

class AlternativePaths;

DataMaster::DataMaster()
{

}


//generate type for each MAV - 0 for passenger and 1 for freight
vector<int> DataMaster::set_mavtype(int mav_num, double prob)   //prob = PAS_MAV_PROP
{
    vector<int> mav_type(mav_num);
    int pas_mavs = int(ceil(mav_num * prob));
    if(prob == 0)
    {
        for(int i = 0; i < mav_num; i++)
        {
            mav_type[i] = 1;
        }
    }
    else if(prob == 1)
    {
        for(int i = 0; i < mav_num; i++)
        {
            mav_type[i] = 0;
        }
    }
    else
    {
        for(int i = 0; i < pas_mavs; i++)
        {
            mav_type[i] = 0;
        }
        for(int i = pas_mavs; i < mav_num; i++)
        {
            mav_type[i] = 1;
        }
    }
    return mav_type;
    // vector<int> amv_type(amv_num);
    // RandomNumber r;
    // for(int i = 0; i < amv_num; i++)
    // {
    //     amv_type[i] = (r.get_rflt() <= prob) ? 0 : 1;
    // }
}

//link vehicle capacity with each vehicle type
vector<int> DataMaster::set_mavcap(vector<int> mav_type)    //0 for passenger and 1 for freight
{
    vector<int> mav_cap(int(mav_type.size()));
    for(int i = 0; i < mav_type.size(); i++)
    {
        mav_cap[i] = (mav_type[i] == 0) ? VCAP_0: VCAP_1;
    }
    return mav_cap;
}

//generate waiting time limit for each AMV
vector<int> DataMaster::set_mavwlim(vector<int> mav_type)
{
    vector<int> mav_wlim(int(mav_type.size()));
    for(int i = 0; i < mav_type.size(); i++)
    {
        mav_wlim[i] = (mav_type[i] == 0) ? WAIT_0: WAIT_1;
    }
    return mav_wlim;
}


//generate type for each request - 0 for passenger and 1 for freight
//in larger transportation network, some nodes have type 2, which are intersections without demands. 
vector<int> DataMaster::set_nodetype(int node_num, double prob, bool add_intersects) //prob = PAS_REQ_PROP
{
    vector<int> node_type(node_num);
    int pas_reqs = int(ceil(node_num * prob));
    if(prob == 0)
    {
        for(int i = 0; i < node_num; i++)
        {
            node_type[i] = 1;
        }
    }
    else if(prob == 1)
    {
        for(int i = 0; i < node_num; i++)
        {
            node_type[i] = 0;
        }
    }
    else
    {
        for(int i = 0; i < pas_reqs; i++)
        {
            node_type[i] = 0;
        }
        for(int i = pas_reqs; i < node_num; i++)
        {
            node_type[i] = 1;
        }
    }
    if(add_intersects) //if intersections are added, some nodes will be changed to intersections with type 2
    {
        RandomNumber r;
        for(int i = 0; i < node_num; i++)
        {
            node_type[i] = (r.get_rflt() <= INTERSECTIONS_PROB) ? 2 : node_type[i];
        }
    }
    return node_type;
}

//generate random demand for each node according to their node type (for self-created data)
vector<int> DataMaster::randdemand(vector<int> nodetype)
{
    vector<int> demand;
    demand.resize(int(nodetype.size()));
    RandomNumber r;
    for(int i = 0; i < nodetype.size(); i++)
    {
        switch(nodetype[i])
        {
            case 0:
                demand[i] = r.get_rint(DMR_0[0], DMR_0[1]);
                break;
            case 1:
                demand[i] = r.get_rint(DMR_1[0], DMR_1[1]);
                break;
            case 2:
                demand[i] = 0;
                break;
        }
    }
    return demand;
}

//modify demands according to node types by setting demands
void DataMaster::modify_demand(vector<int> &initial_dmd, vector<int> nodetype)
{
    int nodenum = nodetype.size();
    for(int i = 0; i < nodenum; i++)
    {
        if(nodetype[i] == 2)
        {
            initial_dmd[i] = 0;
        }
    }
}

//set the demand type for each node based on the node types and the demands
vector<int> DataMaster::set_demand_type(vector<int> demands, vector<int> nodetype)
{
    vector<int> dmd_type; //0: pickup nodes; 1: delivery nodes; 2: depot or intersections
    int nodenum = nodetype.size();
    for(int i = 0; i < nodenum; i++)
    {
        if(nodetype[i] != 2)
        {
            dmd_type[i] = (demands[i] >= 0) ? 0 : 1;
        }
        else //nodetype[i] == 2
        {
            dmd_type[i] = 2;
        }
    }
    return dmd_type;
}

//generate constant service time length for each location (for self-created data)
vector<int> DataMaster::set_sertime_const(vector<int> node_type)
{
    vector<int> serve_time(int(node_type.size()));
    for(int i = 0; i < node_type.size(); i++)
    {
        switch(node_type[i])
        {
            case 0:
                serve_time[i] = SERT_0;
                break;
            case 1:
                serve_time[i] = SERT_1;
                break;
            case 2:
                serve_time[i] = 0;
                break;
        }
    }
    return serve_time;
}

//generate service rate for each customer (for self-created data)
vector<double> DataMaster::set_serverate(vector<int> node_type)
{
    vector<double> serve_rate(int(node_type.size()));
    for(int i = 0; i < node_type.size(); i++)
    {
        switch(node_type[i])
        {
            case 0:
                serve_rate[i] = SER_RATE_0;
                break;
            case 1:
                serve_rate[i] = SER_RATE_1;
                break;
            case 2:
                serve_rate[i] = 0;
                break;
        }
    }
    return serve_rate;
}

//generate available AMV serving time window for each customer according to their node type (for self-created data)
vector<vector<int>> DataMaster::set_sertw(vector<vector<int>> tvl_tw, vector<int> nodetype) //the two parameters should be of the same length
{
    RandomNumber r;
    vector<vector<int>> ser_tw(int(tvl_tw.size()));
    for(int i = 0; i < tvl_tw.size(); i++)
    {
        if(nodetype[i] != 2)
        {
            int serve_period = (nodetype[i] == 0) ? SER_PRD_0 : SER_PRD_1;
            if(tvl_tw[i][1] - serve_period <= tvl_tw[i][0])
            {
                ser_tw[i] = tvl_tw[i];
            }
            else
            {
                int ser_start = r.get_rint(tvl_tw[i][0], tvl_tw[i][1] - serve_period);
                int ser_end = ser_start + serve_period;
                ser_tw[i] = {max(ser_start, tvl_tw[i][0]), min(ser_end, tvl_tw[i][1])};
            }
        }
        else
        {
            ser_tw[i] = tvl_tw[i];
        }
    }
    return ser_tw;
}

//get the combination of customers and amvs of the same type
vector<vector<int>> DataMaster::set_matchmavs(vector<int> nodetype, vector<int> mavtype)
{
    vector<vector<int>> match_comb;
    match_comb.resize(int(nodetype.size()));
    for(int i = 0; i < nodetype.size(); i++)
    {
        if(nodetype[i] < 2)
        {
            for(int v = 0; v < mavtype.size(); v++)
            {
                if(mavtype[v] == nodetype[i])
                {
                    match_comb[i].push_back(v);
                }
            }
        }
        else    //nodetype[i] == 2 -> intersections
        {
            for(int v = 0; v < mavtype.size(); v++)
            {
                match_comb[i].push_back(v);
            }
        }
    }
    return match_comb;
}


//get initial distance matrix given the coordinate of each node
vector<vector<double>> DataMaster::get_init_distmat(vector<vector<double>> coordinates)
{
    int nodenum = coordinates.size();
    vector<vector<double>> init_dist(nodenum);
    for(int i = 0; i < nodenum; i++)
    {
        init_dist[i].resize(nodenum);
        for(int j = 0; j < nodenum && j != i; j++)
        {
            init_dist[i][j] = cal_euclid_dist(coordinates[i], coordinates[j]);
        }
        init_dist[i][i] = 0;
    }
    return init_dist;
}

//modify the initial distance matrix by randomly disconnecting some direct links between two nodes
vector<vector<double>> DataMaster::modify_init_distmat(vector<vector<double>> init_dist)
{
    int nodenum = init_dist.size();
    vector<vector<double>> copy_dist = init_dist;
    RandomNumber r;
    for(int i = 0; i < nodenum; i++)
    {
        for(int j = 0; j < nodenum && j != i; j++)
        {
            copy_dist[i][j] = (r.get_rflt() <= DISCONNECTION_PROB) ? INF : init_dist[i][j];
        }
    }
    return copy_dist;
}

//find the neighbours of each node
vector<vector<int>> DataMaster::get_neighbours(vector<vector<double>> init_dist, int node_num)
{
    vector<vector<int>> adjacent_nodes(node_num);
    for(int i = 0; i < node_num; i++)
    {
        for(int j = 0; j < node_num; j++)
        {
            if((i != j) && (init_dist[i][j] != INF))
            {
                adjacent_nodes[i].push_back(j);
            }
        }
    }
    return adjacent_nodes;
}

//generate the travel time matrix based on original distance matrix
vector<vector<int>> DataMaster::get_init_tvltime(vector<vector<double>> init_dist, int node_num, double speed)
{
    vector<vector<int>> travel_time(node_num);
    for(int i = 0; i < node_num; i++)
    {
        travel_time[i].resize(node_num);
        for(int j = 0; j < node_num; j++)
        {
            travel_time[i][j] = (init_dist[i][j] < INF) ? int(ceil(init_dist[i][j] / speed)) : INF; //rounding up to integer
        }
    }
    return travel_time;
}

//generate available AMV travelling time windows for each customer: earliest arrival time & latest leaving time
//to model time-space network, transform time into integers
vector<vector<int>> DataMaster::get_tvltw(vector<double> source_dist, int plan_horizon, double speed)   //source dist is the shortest path distance from the depot
{
    vector<vector<int>> tvl_tw(int(source_dist.size()));
    for(int i = 0; i < source_dist.size(); i++)
    {
        int source_time = int(ceil(source_dist[i] / speed));
        if(plan_horizon - source_time <= 0 + source_time)
        {
            cerr << "Infeasible travel time window of node "<< i << endl;
        }
        else
            tvl_tw[i] = {0 + source_time, plan_horizon - source_time};  //by default, the starting time of the planning horizon is 0
    }
	return tvl_tw;
}

/*BenchmarkInitializer: read and deal with the dataset in the file*/
BenchmarkInitializer::BenchmarkInitializer(string filepath, bool modify_dist): DataMaster()
{
    filename = filepath;
    file_row = 0;
    countrows();
    readdata();
    veh_num = data_vec[0][0];
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
    modified_distmat = (modify_dist) ? modify_init_distmat(initial_distmat) : initial_distmat;
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
                for(int j = 0; j < 3; j++)
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
        int start_serve_late = data_vec[i][5];  //start service time
        int bypass_early = travel_tw[i-1][0];
        int bypass_late = travel_tw[i-1][1] - data_vec[i][6];  //data_vec[i][6]: const service time for the node
        service_tw.push_back({max(start_serve_early, bypass_early), min(start_serve_late, bypass_late)});
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
ResultWriter::ResultWriter(string filepath, string filename, Solution solution, vector<double> cpu_time, string stage_name)
{
    file_path = filepath;
    file_name = filename;
    input_sol = solution;
    cpu = cpu_time;
    solution_stage = stage_name;
    // heu_names = heuristic_names;
}

//write the result to file 
void ResultWriter::write_result()
{
    outfile.open(file_path, ios::out | ios::trunc);
    outfile << "Instance name: " << file_name << endl;
    outfile << "Algorithm Stage: " << solution_stage << endl;
    outfile << "* All routes:" << endl;
    vector<Route> allroutes = input_sol.sol_config;
    for(int i = 0; i < allroutes.size(); i++)
    {
        outfile << "vehicle id: " << allroutes[i].veh_id << "  ";
        outfile << "vehicle type: " << allroutes[i].veh_type << endl;
        outfile << "compact route: ";
        for(int r = 0; r < allroutes[i].compact_route.size(); r++)
        {
            outfile << allroutes[i].compact_route[r] << " ";
        }
        outfile << endl;
        outfile << "used paths between two served nodes: ";
        for(int p = 0; p < allroutes[i].used_paths_in_compact_route.size(); p++)
        {
            outfile << allroutes[i].used_paths_in_compact_route[p] << " ";
        }
        outfile << endl;
        outfile << "extended route: ";
        for(int r = 0; r < allroutes[i].extended_route.size(); r++)
        {
            outfile << allroutes[i].extended_route[r] << " ";
        }
        outfile << endl;
        outfile << "node labels: ";
        for(int s = 0; s < allroutes[i].node_labels.size(); s++)
        {
            outfile << allroutes[i].node_labels[s] << " ";
        }
        outfile << endl;
        outfile << "arrival time windows: ";
        for(int t = 0; t < allroutes[i].route_arrtw.size(); t++)
        {
            outfile << "{" << allroutes[i].route_arrtw[t][0] << ", " << allroutes[i].route_arrtw[t][1] << "}";
        }
        outfile << endl;
        outfile << "departure time windows: ";
        for(int t = 0; t < allroutes[i].route_deptw.size(); t++)
        {
            outfile << "{" << allroutes[i].route_deptw[t][0] << ", " << allroutes[i].route_deptw[t][1] << "}";
        }
        outfile << endl;
        outfile << "arrival time: ";
        for(int t = 0; t < allroutes[i].route_arrtime.size(); t++)
        {
            outfile << allroutes[i].route_arrtime[t] << " ";
        }
        outfile << endl;
        outfile << "departure time: ";
        for(int t = 0; t < allroutes[i].route_deptime.size(); t++)
        {
            outfile << allroutes[i].route_deptime[t] << " ";
        }
        outfile << endl;
        outfile << "route load: ";
        for(int r = 0; r < allroutes[i].route_load.size(); r++)
        {
            outfile << allroutes[i].route_load[r] << " ";
        }
        outfile << endl;
        outfile << "route mileage: ";
        for(int m = 0; m < allroutes[i].route_mileage.size(); m++)
        {
            outfile << allroutes[i].route_deptime[m] << " ";
        }
        outfile << endl;
    }
    outfile << endl;
    outfile << "* All platoons: " << endl;
    vector<CouplingArcSol> allplatoons = input_sol.sol_platoons_all_arcs;
    for(int i = 0; i < allplatoons.size(); i++)
    {
        outfile << "platooning arc: {" << allplatoons[i].thisarc[0] << ", " << allplatoons[i].thisarc[1] << "}" <<endl;
        outfile << "platoons on this arc: " << endl;
        for(int p = 0; p < allplatoons[i].platoons_config_on_arc.size(); p++)
        {
            outfile << "platoon " << p+1;
            for(int a = 0; a < allplatoons[i].platoons_config_on_arc[p].size(); a++)
            {
                outfile << allplatoons[i].platoons_config_on_arc[p][a].first << " ";
            }
            outfile << endl;
        }
        outfile << "energy saving on this arc:" << allplatoons[i].energy_saving << endl;
    }
    outfile << endl;
    outfile << "total energy-related distance cost: " << input_sol.total_energy_related_dist << endl;
    outfile << "total trip duration: " << input_sol.total_trip_duration << endl;
    outfile << "total unserved requests: " << input_sol.total_unserved_requests << endl;
    outfile << "total objective value: " << input_sol.total_obj_val << endl;
    outfile << "CPU time before platooning: " << cpu[0] << endl;
    outfile << "CPU time after platooning: " << cpu[1] << endl;

    outfile.close();
}

void ResultWriter::record_solution()
{
    outfile.open(file_path, ios::out | ios::trunc);
    outfile << "Instance name: " << file_name << endl;
    outfile << "Algorithm Stage: " << solution_stage << endl;

    outfile << "total energy-related distance cost: " << input_sol.total_energy_related_dist << endl;
    outfile << "total trip duration: " << input_sol.total_trip_duration << endl;
    outfile << "total unserved requests: " << input_sol.total_unserved_requests << endl;
    outfile << "total objective value: " << input_sol.total_obj_val << endl;
    outfile << "CPU time before platooning: " << cpu[0] << endl;
    outfile << "CPU time after platooning: " << cpu[1] << endl;

    outfile.close();
}