#ifndef PATH_AVGLENDIFF_REMOVAL_H_
#define PATH_AVGLENDIFF_REMOVAL_H_

#include "couplingVRP/operators/pathDestroyOps/Path_Random_Removal.h"
#include <tuple>
using namespace std;

class ISolution;
class Path_Removal_Template;
class Nodes;
class PathTabu;

class Path_AvgLenDiff_Removal: public Path_Random_Removal
{
    public:
        //!  constructor
        Path_AvgLenDiff_Removal(string s, ALNS_Parameters& alns_param, PathTabu& tabuObj, Nodes& nodes);

        //! destructor
        virtual ~Path_AvgLenDiff_Removal() {};

        //! destroy operator: inherit from the Path_Removal_Template
        // void destroySolPath(ISolution& sol);
    
    private:
        //! reference to the set of nodes
        Nodes& nodeset;

        //! randomzing selection parameter for the path removal operator;
        // double randAvgLenDiff;

        //! calculate the average alternatives' length differences for all destroyable path
        // vector<double> calAllAvgLenDiff(vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig);
        vector<double> calMeasurement(VRPSolution& vrpsol, vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig);
};

#endif
