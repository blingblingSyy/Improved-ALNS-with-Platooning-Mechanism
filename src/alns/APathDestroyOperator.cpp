#include "src/alns/APathDestroyOperator.h"
#include "src/alns/AOperator.h"
using namespace std;

APathDestroyOperator::APathDestroyOperator(vector<vector<int>>& availPathSetSize, double maxPathDestroyRate, string s) : AOperator(s)
{
	minimunDestroy = 1;
	maximumDestroy = 0; 
	pathTabu.resize(availPathSetSize.size());
	for(int i = 0; i < availPathSetSize.size(); i++)
	{
		pathTabu[i].resize(availPathSetSize[i].size());
		maximumDestroy += availPathSetSize[i].size();
		for(int j = 0; j < availPathSetSize[i].size(); j++)
		{
			pathTabu[i][j] = (availPathSetSize[i][j] <= 1) ? -1 : 0;
			maximumDestroy += pathTabu[i][j];
		}
	} //! now the maximumDestroy is equal to the number of paths that are not permanantly forbidden.
	maximumDestroy *= maxPathDestroyRate;
}