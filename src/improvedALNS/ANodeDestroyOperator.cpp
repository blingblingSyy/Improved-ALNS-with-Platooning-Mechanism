#include "utility.h"
#include "src/improvedALNS/ANodeDestroyOperator.h"
#include "src/improvedALNS/AOperator.h"
using namespace std;

ANodeDestroyOperator::ANodeDestroyOperator(string s, double maxNodeDestroyRate, int cusnum) : AOperator(s)
{
	minimunDestroy = 1;
	maximumDestroy = maxNodeDestroyRate * cusnum;
	// this->destroyed_arcpos = destroyed_arcpos;
    destroyed_nodeset.clear();
}

void ANodeDestroyOperator::calNodeDestroySize(int served_num)
{
	RandomNumber r;
	nodeDestroySize = r.get_rint(min(minimunDestroy, served_num), min(served_num, maximumDestroy));
}

/* new version */
void ANodeDestroyOperator::update(ISolution& sol, ALNS_Iteration_Status& status)
{
	if(needUpdate)
	{
		for(int i = 0; i < destroyed_arcpos.size(); i++)
		{
			pair<int, int> forbidden_pos1 = make_pair(get<2>(destroyed_arcpos[i]), get<0>(destroyed_arcpos[i]));
			pair<int, int> forbidden_pos2 = make_pair(get<2>(destroyed_arcpos[i])+1, get<0>(destroyed_arcpos[i]));
			forbidden_destroyed_nodepos.insert(forbidden_pos1);
			forbidden_destroyed_nodepos.insert(forbidden_pos2);
		}
	}
}