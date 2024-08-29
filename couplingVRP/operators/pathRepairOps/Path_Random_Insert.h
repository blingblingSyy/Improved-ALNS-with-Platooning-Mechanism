#ifndef PATH_RANDOM_INSERT_H_
#define PATH_RANDOM_INSERT_H_

#include "src/ALNS_inc.h"
using namespace std;

class ISolution;
class APathRepairOperator;
class Nodes;
class VRPSolution;
 
class Path_Random_Insert: public APathRepairOperator
{
    public:
        //!  constructor
        Path_Random_Insert(string s, Nodes& nodes);

        //! destructor
        virtual ~Path_Random_Insert();

        //! destroy operator: do nothing to the solution because the operator is emtpy
        void repairSolPath(ISolution& sol);
    
    protected:
        //! reference to the set of nodes
        Nodes& nodeset;

        //! randomizing selection parameters to insert a path
        double randSel;

        //! the measurement to sort the alternative paths within a path
        virtual vector<int> sortMeasurement(VRPSolution& vrpsol, vector<int> arc_config, int orig_usedpath);
};

#endif
