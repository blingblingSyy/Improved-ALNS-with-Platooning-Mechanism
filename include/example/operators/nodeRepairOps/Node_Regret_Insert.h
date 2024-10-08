#ifndef NODE_REGRET_INSERT_H_
#define NODE_REGRET_INSERT_H_

// #include "ALNS_inc.h"
#include "example/operators/nodeRepairOps/Node_Random_Insert.h"
using namespace std;

class ISolution;
// class ANodeRepairperator;
class Node_Random_Insert;

class Node_Regret_Insert: public Node_Random_Insert
{
    public:
        //! constructor
        Node_Regret_Insert(string s, int regret_k);

        //! destructor
        virtual ~Node_Regret_Insert();

        //! destroy operator
        void repairSolNode(ISolution& sol);
    
    private:
        //! the step size for regret heuristic
        int K;

        // //! update the forbidden destroyed node positions
        // void update(ISolution& sol, ALNS_Iteration_Status& status);
};

#endif
