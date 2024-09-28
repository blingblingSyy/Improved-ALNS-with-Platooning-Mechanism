#ifndef PATH_CARDINALITY_REMOVAL_H_
#define PATH_CARDINALITY_REMOVAL_H_

#include "example/operators/pathDestroyOps/Path_Random_Removal.h"
using namespace std;

class ISolution;
class Path_Removal_Template;
class Operators_Parameters;
class PathTabu;
class Nodes;
 
class Path_Cardinality_Removal: public Path_Random_Removal
{
    public:
        //!  constructor
        Path_Cardinality_Removal(string s, Operators_Parameters& ops_param, PathTabu& tabuObj, Nodes& nodes);

        //! destructor
        virtual ~Path_Cardinality_Removal() {};

        //! destroy operator: do nothing to the solution because the operator is emtpy
        // void destroySolPath(ISolution& sol);
        
    private:
        // //! reference to the set of nodes
        // Nodes& nodeset;

        //! randomzing selection parameter for the path removal operator;
        // double randCardi;

        //! calculate the Cardinality for all destroyable path
        // vector<int> calAllCardi(vector<vector<int>> all_destroyable_arcconfig);
        vector<double> calMeasurement(VRPSolution& vrpsol, vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig);

};

#endif
