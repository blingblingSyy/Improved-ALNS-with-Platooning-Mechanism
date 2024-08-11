#include "utility.h"
#include "src/improvedALNS/ANodeDestroyOperator.h"
#include "src/improvedALNS/AOperator.h"
using namespace std;

ANodeDestroyOperator::ANodeDestroyOperator(string s, double maxNodeDestroyRate, int cusnum) : AOperator(s)
{
	minimunDestroy = 1;
	maximumDestroy = maxNodeDestroyRate * cusnum;
}

void ANodeDestroyOperator::calNodeDestroySize(int served_num)
{
	RandomNumber r;
	nodeDestroySize = r.get_rint(min(minimunDestroy, served_num), min(served_num, maximumDestroy));
}