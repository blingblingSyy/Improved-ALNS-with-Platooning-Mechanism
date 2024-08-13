#ifndef PATH_NODEFREQ_INSERT_H_
#define PATH_NODEFREQ_INSERT_H_

#include "src/ALNS_inc.h"
using namespace std;

class ISolution;
class VRPSolution;
class APathRepairOperator;
 
class Path_NodeFreq_Insert: public APathRepairOperator
{
    public:
        //!  constructor
        Path_NodeFreq_Insert(string s, Nodes& nodes, double rand_nFreq);

        //! destructor
        virtual ~Path_NodeFreq_Insert();

        //! destroy operator: do nothing to the solution because the operator is emtpy
        void repairSolPath(ISolution& sol);
                
    private:
        //! reference to the set of nodes
        Nodes& nodeset;

        //! the randomizing parameters 
        double randF;

        //! calculate the average frequency of intermediate nodes 
        double calPathAvgNodeFreq(VRPSolution& vrpsol, vector<int> path);
};

#endif
