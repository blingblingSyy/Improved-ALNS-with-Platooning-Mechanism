#ifndef PATH_RANDOM_REMOVAL_H_
#define PATH_RANDOM_REMOVAL_H_

#include "src/ALNS_inc.h"
using namespace std;

class ISolution;
class APathDestroyOperator;
class IUpdatable;
class Nodes;
 
class Path_Random_Removal: public APathDestroyOperator, public IUpdatable
{
    public:
        //!  constructor
        Path_Random_Removal(string s, ALNS_Parameters& alns_param, Nodes& nodes);

        //! destructor
        virtual ~Path_Random_Removal();

        //! destroy operator: do nothing to the solution because the operator is emtpy
        void destroySolPath(ISolution& sol);
        
};

#endif
