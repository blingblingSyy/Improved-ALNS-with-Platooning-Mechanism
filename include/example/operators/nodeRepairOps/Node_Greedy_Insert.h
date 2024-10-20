#ifndef NODE_GREEDY_INSERT_H_
#define NODE_GREEDY_INSERT_H_

// #include "ALNS_inc.h"
#include "example/operators/nodeRepairOps/Node_Regret_Insert.h"
using namespace std;

class ISolution;
class Node_Regret_Insert;
class IUpdatable;

class Node_Greedy_Insert: public Node_Regret_Insert
{
    public:
        //! constructor
        Node_Greedy_Insert(string s);

        //! destructor
        virtual ~Node_Greedy_Insert();

        //! destroy operator
        // virtual void repairSolNode(ISolution& sol);

    // private:
    //     //! update the forbidden destroyed node positions
    //     void update(ISolution& sol, ALNS_Iteration_Status& status);
};

#endif
