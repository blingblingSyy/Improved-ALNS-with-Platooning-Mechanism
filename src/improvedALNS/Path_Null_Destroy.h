#ifndef PATH_NULL_DESTROY_H_
#define PATH_NULL_DESTROY_H_

#include "src/ALNS_inc.h"
using namespace std;

class Path_Null_Destroy: public APathDestroyOperator
{
public:
	//!  constructor
	Path_Null_Destroy(string s): APathDestroyOperator(s)
	{
		empty = true;
		// hasSelectedCur = true;
		toSelectNext = false;
	}

	//! destructor
	virtual ~Path_Null_Destroy() {};

	//! destroy operator: do nothing to the solution because the operator is emtpy
	void destroySolPath(ISolution& sol) {};

	//! the update procedure of this object does nothing
	void update(ISolution& sol, ALNS_Iteration_Status& status) {};
};

#endif
