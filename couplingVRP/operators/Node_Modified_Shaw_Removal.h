#ifndef NODE_MODIFIED_SHAW_REMOVAL_H_
#define NODE_MODIFIED_SHAW_REMOVAL_H_

#include <vector>
#include "ALNS_inc.h"
using namespace std;

class ISolution;
class ALNS_Iteration_Status;

class Node_Modified_Shaw_Removal: public ANodeDestroyOperator
{
    public:
        Node_Modified_Shaw_Removal(int cusnum, double maxNodeDestroyRate, string s);
        virtual ~Node_Modified_Shaw_Removal();
        void destroySolNode(ISolution& sol);
};

#endif
