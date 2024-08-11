#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include <fstream>
#include "couplingVRP/model/establish/ResultWriter.h"
#include "couplingVRP/model/basic/config.h"
using namespace std;  


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
    outfile << "total distance before platooning: " << input_sol.total_dist_before_platoon << endl;
    outfile << "total energy-related distance cost: " << input_sol.total_energy_related_dist << endl;
    outfile << "total trip duration: " << input_sol.total_trip_duration << endl;
    outfile << "total unserved requests: " << input_sol.total_unserved_requests << endl;
    outfile << "total objective value after platooning: " << input_sol.total_objval_after_platoon << endl;
    outfile << "CPU time before platooning: " << cpu[0] << endl;
    outfile << "CPU time after platooning: " << cpu[1] << endl;

    outfile.close();
}

void ResultWriter::record_solution()
{
    outfile.open(file_path, ios::out | ios::trunc);
    outfile << "Instance name: " << file_name << endl;
    outfile << "Algorithm Stage: " << solution_stage << endl;

    outfile << "total distance before platooning: " << input_sol.total_dist_before_platoon << endl;
    outfile << "total energy-related distance cost: " << input_sol.total_energy_related_dist << endl;
    outfile << "total trip duration: " << input_sol.total_trip_duration << endl;
    outfile << "total unserved requests: " << input_sol.total_unserved_requests << endl;
    outfile << "total objective value after platooning: " << input_sol.total_objval_after_platoon << endl;
    outfile << "CPU time before platooning: " << cpu[0] << endl;
    outfile << "CPU time after platooning: " << cpu[1] << endl;

    outfile.close();
}