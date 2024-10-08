#include "example/operators/nodeDestroyOps/Node_Route_Based_Removal.h"
#include "example/model/establish/VRPSolution.h"
#include "alns/ALNS_inc.h"
#include "utility.h"
#include <vector>
#include <tuple>
using namespace std;

Node_RouteBased_Removal::Node_RouteBased_Removal(string s) : ANodeDestroyOperator(s)
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
}

Node_RouteBased_Removal::~Node_RouteBased_Removal()
{

}

void Node_RouteBased_Removal::destroySolNode(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    RandomNumber r1;
    int select_rid = r1.get_rint(0, vrpsol.getRoutesNum()-1);
    vrpsol.deleteOneRoute(select_rid);
    // vrpsol.deleteOneRoute(3);
}