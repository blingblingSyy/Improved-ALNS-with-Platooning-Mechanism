#include <direct.h>
#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include <fstream> 
#include <tuple>
#include "config.h"
#include "utility.h"
#include "ALNS_Parameters.h"
#include "DataDealer.h"
#include "InitFeasSol.h"
using namespace std;  


int main()
{
    //obtain all the instance file paths
    string file_path = "C:/Users/SYY/Improved_ALNS_Git/dataset/Small_Benchmark/test1.txt"; //"C:\\Users\\SYY\\Improved_ALNS_Git\\dataset\\Li_Lim_Benchmark\\pdp_100\\lc101.txt"
    bool modify_nodes_to_intersects = false;
    bool modify_connectivity = false;
    bool modify_pasdmd = true;
    BenchmarkInitializer data1(file_path, modify_nodes_to_intersects, modify_connectivity, modify_pasdmd);
    Vehicles vehset = data1.get_mav_struct();
    Nodes nodeset = data1.get_node_struct();
    InitFeasSol initial_sol(nodeset, vehset);
    initial_sol.run_algorithm();
    initial_sol.get_best_solution();

    return 0;
}