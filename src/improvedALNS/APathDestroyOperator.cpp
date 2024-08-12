#include <algorithm>
#include "src/improvedALNS/APathDestroyOperator.h"
#include "src/improvedALNS/AOperator.h"
#include "src/improvedALNS/ALNS_Parameters.h"
#include "utility.h"
using namespace std;

APathDestroyOperator::APathDestroyOperator(string s, ALNS_Parameters& alns_param, vector<vector<int>> availPathSetCardi) : AOperator(s)
{
	minimunDestroy = 1;
	maximumDestroy = 0; 
	// this->availPathSetCardi = &availPathSetCardi;
	pathTabu.resize(availPathSetCardi.size());
	for(int i = 0; i < availPathSetCardi.size(); i++)
	{
		pathTabu[i].resize(availPathSetCardi[i].size());
		maximumDestroy += availPathSetCardi[i].size();
		for(int j = 0; j < availPathSetCardi[i].size(); j++)
		{
			pathTabu[i][j] = (availPathSetCardi[i][j] <= 1) ? -1 : 0;
			maximumDestroy += pathTabu[i][j];
		}
	} //! now the maximumDestroy is equal to the number of paths that are not permanantly forbidden.
	maximumDestroy *= alns_param.getPathDestroyRate();
	this->tabu_tenure = alns_param.getTabuTenure();
}

void APathDestroyOperator::calPathDestroySize(int destroyable_pathnum)
{
	RandomNumber r;
	pathDestroySize = r.get_rint(min(minimunDestroy, destroyable_pathnum), min(destroyable_pathnum, maximumDestroy));
}

void APathDestroyOperator::update(ISolution&sol, ALNS_Iteration_Status& status)
{
    for(int i = 0; i < pathTabu.size(); i++)
    {
        for(int j = 0; j < pathTabu[i].size(); j++)
        {
            if(pathTabu[i][j] > 0)
            {
                if(destroyed_arc_config.find({i,j}) == destroyed_arc_config.end())
                {
                    pathTabu[i][j] -= 1;
                }
            }
        }
    }
}