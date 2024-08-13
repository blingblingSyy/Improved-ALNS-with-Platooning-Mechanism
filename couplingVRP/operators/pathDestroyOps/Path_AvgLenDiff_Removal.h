#ifndef PATH_AVGLENDIFF_REMOVAL_H_
#define PATH_AVGLENDIFF_REMOVAL_H_

#include "src/ALNS_inc.h"
using namespace std;

class ISolution;
class APathDestroyOperator;
class Nodes;
class IUpdatable;

class Path_AvgLenDiff_Removal: public APathDestroyOperator, public IUpdatable
{
    public:
        //!  constructor
        Path_AvgLenDiff_Removal(string s, ALNS_Parameters& alns_param, Nodes& nodes);

        //! destructor
        virtual ~Path_AvgLenDiff_Removal();

        //! destroy operator: do nothing to the solution because the operator is emtpy
        void destroySolPath(ISolution& sol);
    
    private:
        //! reference to the set of nodes
        Nodes& nodeset;
};

#endif
