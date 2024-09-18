// ResultWriter.h

#ifndef _RESULTWRITER_H_    //include guards
#define _RESULTWRITER_H_

#include <vector>
#include <string>
#include <fstream>
#include "example/model/establish/VRPSolution.h"
using namespace std;

class VRPSolution;

/// @brief a class to write the solution results to files
class ResultWriter
{
    private: 
        string instance_name;
        string result_filepath;
        VRPSolution& input_sol;
        string solution_stage;
        double cpu;
        
    public:
        ofstream outfile;

        //! constructor
        ResultWriter(VRPSolution& solution, string instancename, string stage_name, double cpu);

        //! default constructor
        ResultWriter() = default;

        //! destructor
        ~ResultWriter() {};

        //! write the detailed structure of the solution
        void write_result();

        //! record the key measure of the solution
        void record_solution();
};

#endif