#ifndef PATH_NULL_DESTROY_H_
#define PATH_NULL_DESTROY_H_

#include "src/ALNS_inc.h"
using namespace std;

class Path_Null_Destroy: public APathDestroyOperator
{
public:
	//!  constructor
	Path_Null_Destroy(vector<vector<int>>& emtpyvec, string s);

	//! destructor
	virtual ~Path_Null_Destroy();

	//! destroy operator
	void destroySolPath(ISolution& sol);
};

#endif
