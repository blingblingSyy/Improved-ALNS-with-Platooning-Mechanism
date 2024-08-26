#ifndef PATH_NODEFREQ_REMOVAL_H_
#define PATH_NODEFREQ_REMOVAL_H_

#include "couplingVRP/operators/pathDestroyOps/Path_Removal_Template.h"
using namespace std;

class ISolution;
class Path_Removal_Template;
class ALNS_Parameters;
class PathTabu;
class Nodes;
class VRPSolution;
 
class Path_NodeFreq_Removal: public Path_Removal_Template
{
    public:
        //!  constructor
        Path_NodeFreq_Removal(string s, ALNS_Parameters& alns_param, PathTabu& tabuObj, Nodes& nodes);

        //! destructor
        virtual ~Path_NodeFreq_Removal() {};

        //! destroy operator: do nothing to the solution because the operator is emtpy
        // void destroySolPath(ISolution& sol);
        
    private:
        //! reference to the set of nodes
        Nodes& nodeset;

        //! calculate the average frequency of intermediate nodes 
        double calPathAvgNodeFreq(VRPSolution& vrpsol, vector<int> path);

        //! randomzing selection parameter for the path removal operator;
        // double randNFreqDes;

        //! calculate the node frequency for all destroyable path
        // vector<double> calAllNodeFreq(VRPSolution& vrpsol, vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig);
        vector<double> calMeasurement(VRPSolution& vrpsol, vector<tuple<int, int, int>> all_destroyable_arcpos, vector<vector<int>> all_destroyable_arcconfig);
};

#endif
