#ifndef NODE_GREEDY_INSERT_H_
#define NODE_GREEDY_INSERT_H_

#include "ALNS_inc.h"
using namespace std;

class ISolution;
class ANodeRepairperator;

class Node_Greedy_Insert: public ANodeRepairOperator
{
    public:
        //! constructor
        Node_Greedy_Insert(string s);

        //! destructor
        virtual ~Node_Greedy_Insert();

        //! destroy operator
        void repairSolNode(ISolution& sol);
};

#endif
