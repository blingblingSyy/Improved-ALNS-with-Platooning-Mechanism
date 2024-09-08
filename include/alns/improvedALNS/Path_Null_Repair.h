#ifndef PATH_NULL_REPAIR_H_
#define PATH_NULL_REPAIR_H_

#include "alns/ALNS_inc.h"
using namespace std;

class Path_Null_Repair: public APathRepairOperator
{
public:
	//!  constructor
	Path_Null_Repair(string s): APathRepairOperator(s)
	{
		empty = true;
		// hasSelectedCur = true;
		toSelectNext = false;
	}

	//! destructor
	virtual ~Path_Null_Repair() {};

	//! repair operator: do nothing to the solution because the operator is emtpy
	void repairSolPath(ISolution& sol) {};
};

#endif
