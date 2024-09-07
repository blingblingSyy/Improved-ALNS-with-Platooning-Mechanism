#ifndef NODE_RANDOM_REMOVAL_H_
#define NODE_RANDOM_REMOVAL_H_

#include "ALNS_inc.h"
#include <vector>
#include <tuple>
using namespace std;

class ISolution;
class ANodeDestroyOperator;
class ALNS_Iteration_Status;
class VRPSolution;
class NodeTabu;
class Operators_Parameters;

class Node_Random_Removal: public ANodeDestroyOperator
{
    public:
        //! constructor
        Node_Random_Removal(string s, Operators_Parameters& ops_param, int cusnum);  //, NodeTabu& tabuObj

        //! destructor
        virtual ~Node_Random_Removal();

        //! destroy operator
        virtual void destroySolNode(ISolution& sol);
    
    protected:
        // //! node tabu object
        // NodeTabu& nodeTabuObj;

        //! keep the removable positions -> move to node tabu
        /*old version*/
        // vector<vector<int>> findRemovablePos(VRPSolution& vrpsol, vector<vector<int>> all_cus_pos);
        /*new version*/
        void keepRemovablePos(VRPSolution& vrpsol, vector<pair<int, int>>& all_cus_pos);

};

#endif /* TSP_RANDOM_REMOVAL_H_ */
