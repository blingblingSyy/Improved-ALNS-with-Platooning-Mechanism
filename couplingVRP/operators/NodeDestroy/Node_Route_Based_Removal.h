#ifndef NODE_ROUTEBASED_REMOVAL_H_
#define NODE_ROUTEBASED_REMOVAL_H_

#include "ALNS_inc.h"
using namespace std;

class ISolution;
class ANodeDestroyOperator;

class Node_RouteBased_Removal: public ANodeDestroyOperator
{
    public:
        //! constructor
        Node_RouteBased_Removal(string s);

        //! destructor
        virtual ~Node_RouteBased_Removal();

        //! destroy operator
        void destroySolNode(ISolution& sol);
};

#endif /* TSP_RANDOM_REMOVAL_H_ */
