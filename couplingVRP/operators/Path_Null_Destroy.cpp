#include <time.h>
#include <stdlib.h>
#include "couplingVRP/operators/Path_Null_Destroy.h"
#include "couplingVRP/model/VRPSolution.h"
#include "ALNS_inc.h"
using namespace std;

Path_Null_Destroy::Path_Null_Destroy(vector<vector<int>>& emptyvec, string s): APathDestroyOperator(emptyvec, 1, s)
{
	empty = true;
}

Path_Null_Destroy::~Path_Null_Destroy()
{

}

void Path_Null_Destroy::destroySolPath(ISolution& sol)
{
    //! do nothing to the solution because the operator is emtpy
}
