#ifndef NODE_WORST_REMOVAL_H_
#define NODE_WORST_REMOVAL_H_

#include <vector>
#include <tuple>
#include "ALNS_inc.h"
using namespace std;

class ISolution;
class ANodeDestroyOperator;
class VRPSolution;
class ALNS_Parameters;

class Node_Worst_Removal: public ANodeDestroyOperator
{
    public:
        //! constructor
        Node_Worst_Removal(string s, ALNS_Parameters& alns_param, int cusnum);

        //! destructor
        virtual ~Node_Worst_Removal();

        //! destroy operator
        void destroySolNode(ISolution& sol);
    
    private:
        //! calculate the cost of removing a node
        double calRemovalCost(VRPSolution& vrpsol, pair<int, int> removed_node); //! removed_node -> <routeid, arcpos>

        //! randomizinng parameters
        double randWorst;
};

#endif /* TSP_RANDOM_REMOVAL_H_ */