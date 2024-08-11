#ifndef NODE_NULL_DESTROY_H_
#define NODE_NULL_DESTROY_H_

#include "src/ALNS_inc.h"
using namespace std;

class Node_Null_Destroy: public ANodeDestroyOperator
{
public:
    //! constructor
	Node_Null_Destroy(string s) : ANodeDestroyOperator(s, 1, 1)
    {
        empty = true;
        hasSelectedCurOp = true;
        toSelectNextOp = false;
    }

    //! destructor
	virtual ~Node_Null_Destroy() {};

    //! destroy operator: do nothing to the solution because the operator is emtpy
	void destroySolNode(ISolution& sol) {};
};

#endif
