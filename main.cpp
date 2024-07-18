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
// #include "InitFeasSol.h"
using namespace std;  


int main()
{
    //obtain all the instance file paths
    string file_path = "C:/Users/SYY/Improved_ALNS_Git/dataset/Small_Benchmark/test1.txt"; //"dataset\\Small_Benchmark\\test1.txt";
    BenchmarkInitializer data1(file_path, false);

    return 0;
}