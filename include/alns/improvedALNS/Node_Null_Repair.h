#ifndef NODE_NULL_REPAIR_H_
#define NODE_NULL_REPAIR_H_

#include "alns/ALNS_inc.h"
using namespace std;

class Node_Null_Repair: public ANodeRepairOperator
{
    public:
        //! constructor
        Node_Null_Repair(string s) : ANodeRepairOperator(s)
        {
            empty = true;
            // hasSelectedCur = true;
            toSelectNext = false;
        }

        //! destructor
        virtual ~Node_Null_Repair() {};

        //! repair operator: do nothing to the solution because the operator is emtpy
        void repairSolNode(ISolution& sol) {};
    
};

#endif
