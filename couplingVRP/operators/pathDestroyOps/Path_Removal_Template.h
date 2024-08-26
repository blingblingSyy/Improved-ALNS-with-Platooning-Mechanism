#ifndef PATH_REMOVAL_TEMPLATE_H_
#define PATH_REMOVAL_TEMPLATE_H_

#include "src/ALNS_inc.h"
using namespace std;

class ISolution;
class APathDestroyOperator;
class ALNS_Parameters;
class PathTabu;
class Nodes;
class VRPSolution;
 
class Path_Removal_Template: public APathDestroyOperator
{
    public:
        //!  constructor
        Path_Removal_Template(string s, ALNS_Parameters& alns_param, PathTabu& tabuObj);

        //! destructor
        virtual ~Path_Removal_Template();

        //! destroy operator: do nothing to the solution because the operator is emtpy
        void destroySolPath(ISolution& sol);
    
    protected:
        //! the measurement to destroy a path
        vector<double> path_measurement;

        //! randomizing selection parameters to destroy a path
        int randSel;

        //! calculate the measurement
        virtual vector<double> calMeasurement(VRPSolution& vrpsol, vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig) = 0;
        
};

#endif
