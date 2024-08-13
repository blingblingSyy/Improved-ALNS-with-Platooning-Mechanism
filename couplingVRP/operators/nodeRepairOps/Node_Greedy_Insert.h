#ifndef NODE_GREEDY_INSERT_H_
#define NODE_GREEDY_INSERT_H_

#include "ALNS_inc.h"
using namespace std;

class ISolution;
class ANodeRepairperator;
class IUpdatable;

class Node_Greedy_Insert: public ANodeRepairOperator, public IUpdatable
{
    public:
        //! constructor
        Node_Greedy_Insert(string s);

        //! destructor
        virtual ~Node_Greedy_Insert();

        //! destroy operator
        void repairSolNode(ISolution& sol);

    private:
        //! update the forbidden destroyed node positions
        void update(ISolution& sol, ALNS_Iteration_Status& status);
};

#endif
