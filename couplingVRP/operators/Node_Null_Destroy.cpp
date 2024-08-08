#include <time.h>
#include <stdlib.h>
#include "couplingVRP/operators/Node_Null_Destroy.h"
#include "couplingVRP/model/VRPSolution.h"
#include "couplingVRP/model/Nodes.h"
#include "ALNS_inc.h"
using namespace std;

Node_Null_Destroy::Node_Null_Destroy(string s): ANodeDestroyOperator(1, 1, s)
{
	empty = true;
}

void Node_Null_Destroy::destroySolNode(ISolution& sol)
{
    //! do nothing to the solution because the operator is emtpy
}
