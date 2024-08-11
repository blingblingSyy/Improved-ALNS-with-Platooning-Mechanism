#ifndef PATH_NULL_DESTROY_H_
#define PATH_NULL_DESTROY_H_

#include "src/ALNS_inc.h"
using namespace std;

class Path_Null_Destroy: public APathDestroyOperator
{
public:
	//!  constructor
	Path_Null_Destroy(string s, ALNS_Parameters& alns_param, vector<vector<int>>& emptyvec): APathDestroyOperator(s, alns_param, emptyvec)
	{
		empty = true;
		hasSelectedCurOp = true;
		toSelectNextOp = false;
	}

	//! destructor
	virtual ~Path_Null_Destroy() {};

	//! destroy operator: do nothing to the solution because the operator is emtpy
	void destroySolPath(ISolution& sol) {};
};

#endif
