#ifndef PATH_RANDOM_INSERT_H_
#define PATH_RANDOM_INSERT_H_

#include "src/ALNS_inc.h"
using namespace std;

class ISolution;
class APathRepairOperator;
class Nodes;
 
class Path_Random_Insert: public APathRepairOperator
{
    public:
        //!  constructor
        Path_Random_Insert(string s, Nodes& nodes);

        //! destructor
        virtual ~Path_Random_Insert();

        //! destroy operator: do nothing to the solution because the operator is emtpy
        void repairSolPath(ISolution& sol);
    
    private:
        //! reference to the set of nodes
        Nodes& nodeset;
};

#endif
