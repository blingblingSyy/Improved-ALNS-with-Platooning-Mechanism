#ifndef PATH_CARDINALITY_REMOVAL_H_
#define PATH_CARDINALITY_REMOVAL_H_

#include "src/ALNS_inc.h"
using namespace std;

class ISolution;
class APathDestroyOperator;
class ALNS_Parameters;
 
class Path_Cardinality_Removal: public APathDestroyOperator
{
    public:
        //!  constructor
        Path_Cardinality_Removal(string s, ALNS_Parameters& alns, Nodes& nodes);

        //! destructor
        virtual ~Path_Cardinality_Removal();

        //! destroy operator: do nothing to the solution because the operator is emtpy
        void destroySolPath(ISolution& sol);

};

#endif
