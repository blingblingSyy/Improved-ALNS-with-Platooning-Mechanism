#ifndef PATH_NODEFREQ_REMOVAL_H_
#define PATH_NODEFREQ_REMOVAL_H_

#include "src/ALNS_inc.h"
using namespace std;

class ISolution;
class VRPSolution;
class APathDestroyOperator;
class ALNS_Parameters;
 
class Path_NodeFreq_Removal: public APathDestroyOperator
{
    public:
        //!  constructor
        Path_NodeFreq_Removal(string s, ALNS_Parameters& alns_param, Nodes& nodes);

        //! destructor
        virtual ~Path_NodeFreq_Removal();

        //! destroy operator: do nothing to the solution because the operator is emtpy
        void destroySolPath(ISolution& sol);
        
    private:
        //! reference to the set of nodes
        Nodes& nodeset;

        //! calculate the average frequency of intermediate nodes 
        double calPathAvgNodeFreq(VRPSolution& vrpsol, vector<int> path);
};

#endif
