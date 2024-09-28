#ifndef A_PATH_REPAIROPERATOR_H_
#define A_PATH_REPAIROPERATOR_H_

#include <iostream>
#include "alns/improvedALNS/AOperator.h"
using namespace std;

class ISolution;

/// @brief this abstract class represent a Path Repair Operator. All Path Repair Operator implmentations should inherit from this class.
class APathRepairOperator : public AOperator
{
	public:
		APathRepairOperator(string s) : AOperator(s) {};

		virtual ~APathRepairOperator(){};

		virtual void repairSolPath(ISolution& sol)=0;

};

#endif
