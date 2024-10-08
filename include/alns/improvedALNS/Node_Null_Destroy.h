#ifndef NODE_NULL_DESTROY_H_
#define NODE_NULL_DESTROY_H_

#include "alns/ALNS_inc.h"
using namespace std;

class Node_Null_Destroy: public ANodeDestroyOperator
{
public:
    //! constructor
	Node_Null_Destroy(string s) : ANodeDestroyOperator(s)
    {
        empty = true;
        // hasSelectedCur = true;
        toSelectNext = false;
    }

    //! destructor
	virtual ~Node_Null_Destroy() {};

    //! destroy operator: do nothing to the solution because the operator is emtpy
	void destroySolNode(ISolution& sol) {};
};

#endif
