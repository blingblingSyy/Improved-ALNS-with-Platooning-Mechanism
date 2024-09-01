#ifndef PATH_KMAXLEN_INSERT_H_
#define PATH_KMAXLEN_INSERT_H_

#include "couplingVRP/operators/pathRepairOps/Path_Random_Insert.h"
using namespace std;

class ISolution;
class Path_Random_Insert;
class Operators_Parameters;
 
class Path_KMaxLen_Insert: public Path_Random_Insert
{
    public:
        //!  constructor
        Path_KMaxLen_Insert(string s, Nodes& nodes, Operators_Parameters& ops_param);

        //! destructor
        virtual ~Path_KMaxLen_Insert();

        //! destroy operator: do nothing to the solution because the operator is emtpy
        // void repairSolPath(ISolution& sol);
        
    private:
        //! reference to the set of nodes
        // Nodes& nodeset;

        //! the randomizing parameters 
        double randK;

        //! sort indices based on the length of each alternative path
        vector<int> sortMeasurement(VRPSolution& vrpsol, vector<int> arc_config, int orig_usedpath);
};

#endif
