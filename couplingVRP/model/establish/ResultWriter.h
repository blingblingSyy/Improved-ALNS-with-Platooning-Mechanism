// ResultWriter.h

#ifndef _RESULTWRITER_H_    //include guards
#define _RESULTWRITER_H_

#include <vector>
#include <string>
#include <fstream>
#include "couplingVRP/model/basic/config.h"
using namespace std;

/// @brief a class to write the solution results to files
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