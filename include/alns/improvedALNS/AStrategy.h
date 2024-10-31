#ifndef ASTRATEGY_H_
#define ASTRATEGY_H_

#include <iostream>
using namespace std;

/// @brief this class represents a strategy or an operator. The class can be inherited by the class AOperator
class AStrategy
{
	protected:
		//! Total number of calls during the process.
		size_t totalNumberOfCalls;

		//! Number of calls since the last evaluation.
		size_t nbCallsSinceLastEval;

		//! score of the strategy (operator).
		double score;

		//! weight of the strategy (operator).
		double weight;

		//! designation of the strategy (operator).
		string Name;

		//! Indicate whether the opeartor has been selected
		// bool hasSelectedCur;

	public:

		//! Constructor.
		AStrategy(string name)
		{
			Name = name;
			// hasSelectedCur = false;
			init();
		}

		//! Destructor.
		virtual ~AStrategy() noexcept(false) {};

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
		//! \return the total number of calls to the strategy since the beginning of the optimization process.
		size_t getTotalNumberOfCalls(){return totalNumberOfCalls;};

		//! Simple getter.
		//! \return the number of calls to this strategy since the last evaluation.
		size_t getNumberOfCallsSinceLastEvaluation(){return nbCallsSinceLastEval;};

		//! Incrase the number of calls for the operator every time the strategy (operator) is called
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
		string getName(){return Name;};

		//! getter: evaluate whether the operator(strategy) has been selected
		// bool getHasSelectedCur() {return hasSelectedCur;};

		// //! a simple setter
		// void setHasSelectedCur(bool cur) {hasSelectedCur = cur;};

};


#endif
