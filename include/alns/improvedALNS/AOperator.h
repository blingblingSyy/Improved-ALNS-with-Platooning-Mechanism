#ifndef AOPERATOR_H_
#define AOPERATOR_H_

#include <iostream>
#include "alns/improvedALNS/AStrategy.h"
using namespace std;

class ASelecter;

/// @brief this abstract class represetns an operator, Node Destroy & Node Repair & Path Destroy & Path Repair Operators inherit from this class.
class AOperator : public AStrategy
{
	protected:
		//! Indicate if the operator is used in noise mode or not in terms of node.
		bool noise;

		//! indicate if the operator is empty or not.
		//! if empty, the operator does nothing to the solution
		bool empty;

		//! Indicate whether the next operator can be selected
		bool toSelectNext;

	public:

		//! Constructor.
		AOperator(string name) : AStrategy(name)
		{
			noise = false;
		}

		//! Set noise to true.
		void setNoise() {this->noise = true;};

		//! check whether the path noise operator is used or not
		void unsetNoise() {this->noise = false;};

		//! Destructor.
		virtual ~AOperator() {};

		//! evaluate whether the operator is empty or not.
		//! \return empty
		bool isEmpty() {return empty;};

		//! set opertor to be empty
		void setEmpty(bool isempty) {empty = isempty;};

		//! getter: evaluate whether the operator can proceed to select the next operator
		bool getToSelectNext() {return toSelectNext;};

		//! a simple setter
		void setToSelectNext(bool next) {toSelectNext = next;};
};


#endif
