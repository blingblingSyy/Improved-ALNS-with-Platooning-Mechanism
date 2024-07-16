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
    string file_path = "dataset\\Li_Lim_Benchmark\\pdp_100\\lc101.txt";
    BenchmarkInitializer data1(file_path);

    return 0;
}