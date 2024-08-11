#ifndef A_NODE_REPAIROPERATOR_H_
#define A_NODE_REPAIROPERATOR_H_

#include <iostream>
#include "AOperator.h"

class ISolution;

/// @brief this abstract class represents a Node Repair Operator, all repair operator implementations should inherit from this class.
class ANodeRepairOperator : public AOperator 
{
	public:
		ANodeRepairOperator(std::string s) : AOperator(s)
		{
			
		}

		virtual ~ANodeRepairOperator(){};

		virtual void repairSolNode(ISolution& sol)=0;
};

#endif /* AREPAIROPERATOR_H_ */
