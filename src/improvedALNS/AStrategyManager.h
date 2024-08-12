#ifndef ISTRATEGYMANAGER_H_
#define ISTRATEGYMANAGER_H_

#include <vector>

class AStrategy;
class ANodeDestroyOperator;
class APathDestroyOperator;
class ANodeRepairOperator;
class APathRepairOperator;
class ALNS_Iteration_Status;
class Statistics;

/// @brief This interface represents the methods that should be implemented by an operator manager.
class AStrategyManager
{
	public:
		//! an emuration representing the strategies used in the improved ALNS
		enum CurStrategy
		{
			NodeFirst,
			PathFirst
		};

		//! This method selects a strategy.
		//! \return a node destroy operator.
		virtual void selectStrategy()=0;

		//! This method selects a node destroy operator.
		//! \return a node destroy operator.
		virtual ANodeDestroyOperator& selectNodeDestroyOperator()=0;

		//! This method selects a node repair operator.
		//! \return a node repair operator.
		virtual ANodeRepairOperator& selectNodeRepairOperator(bool nodeDesEmpty)=0;

		//! This method selects a path destroy operator.
		//! \return a node destroy operator.
		virtual APathDestroyOperator& selectPathDestroyOperator()=0;

		//! This method selects a path repair operator.
		//! \return a node repair operator.
		virtual APathRepairOperator& selectPathRepairOperator(bool pathDesEmpty)=0;

		//! recompute the weights of the operators and strategies
		virtual void recomputeWeights()=0;

		//! Update the scores of the operators and strategies.
		// virtual void updateScores(ANodeDestroyOperator& node_des, ANodeRepairOperator& node_rep, APathDestroyOperator& path_des, APathRepairOperator& path_rep, ALNS_Iteration_Status& status)=0;
		virtual void updateScores(ANodeDestroyOperator& node_des, ANodeRepairOperator& node_rep, APathDestroyOperator& path_des, APathRepairOperator& path_rep, ALNS_Iteration_Status& status)=0;

		//! Indicate that the optimization process starts.
		virtual void startSignal()=0;

		//! Destroy the operators registered to this operator manager.
		virtual void end()=0;

		//! Simple setter.
		void setStatistics(Statistics* statistics){stats = statistics;};
		
		//! a simple setter for the current strategy
		void setCurStrategy(CurStrategy st) {curSt = st;};

		//! a simple getter of the current strategy
		CurStrategy getCurStrategy() {return curSt;};

	protected:
		//! A pointer to the instance of the statistics module.
		Statistics* stats;

		//! the currently selected strategy
		CurStrategy curSt;

};

#endif /* IOPERATORMANAGER_H_ */
