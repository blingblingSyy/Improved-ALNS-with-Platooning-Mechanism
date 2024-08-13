#ifndef NODE_RANDOM_REMOVAL_H_
#define NODE_RANDOM_REMOVAL_H_

#include "ALNS_inc.h"
using namespace std;

class ISolution;
class ANodeDestroyOperator;
class ALNS_Iteration_Status;

class Node_Random_Removal: public ANodeDestroyOperator
{
    public:
        //! constructor
        Node_Random_Removal(string s, double maxNodeDestroyRate, int cusnum);

        //! destructor
        virtual ~Node_Random_Removal();

        //! destroy operator
        void destroySolNode(ISolution& sol);

};

#endif /* TSP_RANDOM_REMOVAL_H_ */
