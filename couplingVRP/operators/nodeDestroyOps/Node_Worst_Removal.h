#ifndef NODE_WORST_REMOVAL_H_
#define NODE_WORST_REMOVAL_H_

#include <vector>
#include <tuple>
#include "ALNS_inc.h"
using namespace std;

class ISolution;
class VRPSolution;
class Operators_Parameters;
class Node_Random_Removal;
class Nodes;

class Node_Worst_Removal: public Node_Random_Removal
{
    public:
        //! constructor
        Node_Worst_Removal(string s, Operators_Parameters& ops_param, int cusnum);

        //! destructor
        ~Node_Worst_Removal();

        //! destroy operator
        void destroySolNode(ISolution& sol);
    
    private:
        //! calculate the cost of removing a node
        double calRemovalCost(VRPSolution& vrpsol, pair<int, int> removed_node); //! removed_node -> <routeid, arcpos>

        //! randomizinng parameters
        double randWorst;

};

#endif /* TSP_RANDOM_REMOVAL_H_ */
