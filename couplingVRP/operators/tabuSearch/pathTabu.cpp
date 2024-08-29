#include "couplingVRP/operators/tabuSearch/pathTabu.h"
#include "src/improvedALNS/ALNS_Parameters.h"
#include "couplingVRP/model/establish/VRPSolution.h"
using namespace std;

PathTabu::PathTabu(int tabuTenure, vector<vector<int>> availPathCardiSet)
{
	pathTabuList.resize(availPathCardiSet.size());
	for(int i = 0; i < availPathCardiSet.size(); i++)
	{
		pathTabuList[i].resize(availPathCardiSet[i].size());
		for(int j = 0; j < availPathCardiSet[i].size(); j++)
		{
			pathTabuList[i][j] = (availPathCardiSet[i][j] <= 1) ? -1 : 0;
		}
    }
	this->tabu_tenure = tabuTenure;
}

/*old version*/
// void APathDestroyOperator::update(ISolution&sol, ALNS_Iteration_Status& status)
// {
// 	for(int i = 0; i < pathTabu.size(); i++)
// 	{
// 		for(int j = 0; j < pathTabu[i].size(); j++)
// 		{
// 			if(pathTabu[i][j] > 0)
// 			{
// 				if(destroyed_arc_config.find({i,j}) == destroyed_arc_config.end())
// 				{
// 					pathTabu[i][j] -= 1;
// 				}
// 			}
// 		}
// 	}		
// }

/*new version*/
void PathTabu::update(ISolution& sol, ALNS_Iteration_Status& status)
{
	VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    vrpsol.getDestroyedArcsPos().clear();

	for(int i = 0; i < pathTabuList.size(); i++)
	{
		for(int j = 0; j < pathTabuList[i].size(); j++)
		{
			if(pathTabuList[i][j] > 0)
			{
				pathTabuList[i][j] -= 1;
			}
		}
	}
}

void PathTabu::updatePathTenure(vector<int> input_path)
{
    if(pathTabuList[input_path[0]][input_path[1]] == 0)
    {
        pathTabuList[input_path[0]][input_path[1]] += tabu_tenure + 1;
    }
}