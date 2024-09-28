#ifndef PATH_RANDOM_REMOVAL_H_
#define PATH_RANDOM_REMOVAL_H_

// #include "ALNS_inc.h"
#include "alns/improvedALNS/APathDestroyOperator.h"
using namespace std;

class ISolution;
class APathDestroyOperator;
class Operators_Parameters;
class PathTabu;
class Nodes;
class VRPSolution;
 
class Path_Random_Removal: public APathDestroyOperator
{
    public:
        //!  constructor
        Path_Random_Removal(string s, Operators_Parameters& ops_param, PathTabu& tabuObj, Nodes& nodes);

        //! destructor
        virtual ~Path_Random_Removal() {};

        //! destroy operator: do nothing to the solution because the operator is emtpy
        void destroySolPath(ISolution& sol);
    
    protected:
        //! reference to the set of nodes
        Nodes& nodeset;

        //! the measurement to destroy a path
        vector<double> path_measurement;

        //! randomizing selection parameters to destroy a path
        int randSel;

        //! calculate the measurement
        virtual vector<double> calMeasurement(VRPSolution& vrpsol, vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig);
        
};

#endif
