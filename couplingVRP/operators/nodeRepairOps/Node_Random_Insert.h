#ifndef NODE_RANDOM_INSERT_H_
#define NODE_RANDOM_INSERT_H_

#include "ALNS_inc.h"
using namespace std;

class ISolution;
class ANodeRepairperator;

class Node_Random_Insert: public ANodeRepairOperator
{
    public:
        //! constructor
        Node_Random_Insert(string s);

        //! destructor
        virtual ~Node_Random_Insert();

        //! destroy operator
        void repairSolNode(ISolution& sol);
        
    private:
        //! update the forbidden destroyed node positions
        void update(ISolution& sol, ALNS_Iteration_Status& status);
};

#endif
