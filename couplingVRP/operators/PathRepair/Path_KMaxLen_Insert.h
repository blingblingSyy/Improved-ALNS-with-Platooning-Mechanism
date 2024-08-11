#ifndef PATH_KMAXLEN_INSERT_H_
#define PATH_KMAXLEN_INSERT_H_

#include "src/ALNS_inc.h"
using namespace std;

class ISolution;
class APathRepairOperator;
 
class Path_KMaxLen_Insert: public APathRepairOperator
{
    public:
        //!  constructor
        Path_KMaxLen_Insert(string s, Nodes& nodes, double rand_klen);

        //! destructor
        virtual ~Path_KMaxLen_Insert();

        //! destroy operator: do nothing to the solution because the operator is emtpy
        void repairSolPath(ISolution& sol);
        
    private:
        //! reference to the set of nodes
        Nodes& nodeset;

        //! the randomizing parameters 
        double randK;
};

#endif
