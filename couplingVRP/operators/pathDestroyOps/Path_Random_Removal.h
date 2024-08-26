#ifndef PATH_RANDOM_REMOVAL_H_
#define PATH_RANDOM_REMOVAL_H_

#include "couplingVRP/operators/pathDestroyOps/Path_Removal_Template.h"
using namespace std;

class ISolution;
class Path_Removal_Template;
class ALNS_Parameters;
class PathTabu;
class Nodes;
class VRPSolution;
 
class Path_Random_Removal: public Path_Removal_Template
{
    public:
        //!  constructor
        Path_Random_Removal(string s, ALNS_Parameters& alns_param, PathTabu& tabuObj);

        //! destructor
        virtual ~Path_Random_Removal() {};

        //! destroy operator: do nothing to the solution because the operator is emtpy
        // void destroySolPath(ISolution& sol);
    
    protected:
        //! calculate measurement of a path related with its tabu status
        // vector<int> calRandMeasure(vector<vector<int>> all_destroyable_arcconfig);
        vector<double> calMeasurement(VRPSolution& vrpsol, vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig);
        
};

#endif
