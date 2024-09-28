#ifndef PATH_AVGLEN_REMOVAL_H_
#define PATH_AVGLEN_REMOVAL_H_

#include "example/operators/pathDestroyOps/Path_Random_Removal.h"
#include <tuple>
using namespace std;

class ISolution;
class Path_Random_Removal;
class Nodes;
class PathTabu;
class Operators_Parameters;

class Path_AvgLen_Removal: public Path_Random_Removal
{
    public:
        //!  constructor
        Path_AvgLen_Removal(string s, Operators_Parameters& ops_param, PathTabu& tabuObj, Nodes& nodes);

        //! destructor
        virtual ~Path_AvgLen_Removal() {};

        //! destroy operator: inherit from the Path_Removal_Template
        // void destroySolPath(ISolution& sol);
    
    private:
        // //! reference to the set of nodes
        // Nodes& nodeset;

        //! randomzing selection parameter for the path removal operator;
        // double randAvgLenDiff;

        //! calculate the average alternatives' length differences for all destroyable path
        // vector<double> calAllAvgLenDiff(vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig);
        vector<double> calMeasurement(VRPSolution& vrpsol, vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig);
};

#endif
