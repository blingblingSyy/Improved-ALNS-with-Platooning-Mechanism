#ifndef PATH_NODEFREQ_INSERT_H_
#define PATH_NODEFREQ_INSERT_H_

#include "couplingVRP/operators/pathRepairOps/Path_Random_Insert.h"
using namespace std;

class ISolution;
class VRPSolution;
class Path_Random_Insert;
 
class Path_NodeFreq_Insert: public Path_Random_Insert
{
    public:
        //!  constructor
        Path_NodeFreq_Insert(string s, Nodes& nodes, double rand_nFreq);

        //! destructor
        virtual ~Path_NodeFreq_Insert();

        //! destroy operator: do nothing to the solution because the operator is emtpy
        // void repairSolPath(ISolution& sol);
                
    private:
        //! reference to the set of nodes
        // Nodes& nodeset;

        //! the randomizing parameters 
        double randF;

        //! calculate the average frequency of intermediate nodes 
        double calPathAvgNodeFreq(VRPSolution& vrpsol, vector<int> path);

        //! sort indices based on the length of each alternative path
        vector<int> sortMeasurement(VRPSolution& vrpsol, vector<int> arc_config, int orig_usedpath);

};

#endif
