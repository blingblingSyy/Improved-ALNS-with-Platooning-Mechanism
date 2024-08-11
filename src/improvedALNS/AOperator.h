#ifndef AOPERATOR_H_
#define AOPERATOR_H_

#include <iostream>
using namespace std;

/// @brief this abstract class represetns an operator, Node Destroy & Node Repair & Path Destroy & Path Repair Operators inherit from this class.
class AOperator
{
	private:
		//! Total number of calls during the process.
		size_t totalNumberOfCalls;

		//! Number of calls since the last evaluation.
		size_t nbCallsSinceLastEval;

		//! score of the operator.
		double score;

		//! weight of the operator.
		double weight;

		//! designation of the operator.
		string operatorName;

	protected:
		//! Indicate if the operator is used in noise mode or not.
		bool noise;

		//! indicate if the operator is empty or not.
		//! if empty, the operator does nothing to the solution
		bool empty;

		//! Indicate whether the next operator can be selected
		bool toSelectNextOp;

		//! Indicate whether the opeartor has been selected
		bool hasSelectedCurOp;

	public:

		//! Constructor.
		AOperator(string name)
		{
			operatorName = name;
			init();
		}

		//! Destructor.
		virtual ~AOperator(){};

		//! Initialize the values of the numbers of calls.
		void init()
		{
			totalNumberOfCalls = 0;
			nbCallsSinceLastEval = 0;
			score = 0;
			weight = 1;
		}

		//! reset the number of calls since last eval.
		void resetNumberOfCalls()
		{
			nbCallsSinceLastEval = 0;
		}

		//! Simple getter.
		//! \return the total number of calls to the operator since the beginning of the optimization process.
		size_t getTotalNumberOfCalls(){return totalNumberOfCalls;};

		//! Simple getter.
		//! \return the number of calls to this operator since the last evaluation.
		size_t getNumberOfCallsSinceLastEvaluation(){return nbCallsSinceLastEval;};

		//! Incrase the number of calls for the operator every time the operator is called
		void increaseNumberOfCalls()
		{
			totalNumberOfCalls++;
			nbCallsSinceLastEval++;
		}

		//! Simple getter.
		double getScore() const
		{
			return score;
		}

		//! Simple getter.
		double getWeight() const
		{
			return weight;
		}

		//! resetter.
		void resetScore()
		{
			this->score = 0;
		}

		//! Simple setter.
		void setScore(double s)
		{
			this->score = s;
		}

		//! Simple setter.
		void setWeight(double weight)
		{
			this->weight = weight;
		}

		//! Simple getter.
		string getName(){return operatorName;};

		//! Set noise to true.
		void setNoise(){noise=true;};

		//! Set noise to false.
		void unsetNoise(){noise=false;};

		//! evaluate whether the operator is empty or not.
		//! \return empty
		bool isEmpty() {return empty;};

		//! set opertor to be empty
		void setEmpty(bool isempty) {empty = isempty;};

		//! getter: evaluate whether the operator can proceed to select the next operator
		bool getToSelectNext() {return toSelectNextOp;};

		//! getter: evaluate whether the operator has been selected
		bool getHasSelectedCur() {return hasSelectedCurOp;};

		//! a simple setter
		void setToSelectNext(bool next) {toSelectNextOp = next;};

		//! a simple setter
		void setHasSelected(bool cur) {hasSelectedCurOp = cur;};
};


#endif
