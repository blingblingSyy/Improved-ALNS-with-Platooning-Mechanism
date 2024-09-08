#ifndef STRATEGYMANAGER_H_
#define STRATEGYMANAGER_H_

#include <vector>
#include "alns/improvedALNS/AStrategyManager.h"
using namespace std;

class AStrategy;
class AOperator;
class AStrategyManager;
class ALNS_Parameters;
class ALNS_Iteration_Status;
class ANodeDestroyOperator;
class ANodeRepairOperator;
class APathDestroyOperator;
class APathRepairOperator;

/// @brief A simple implementation of an operator manager, that does not allow to simply couple the node and path destroy and repair operators.
class StrategyManager : public AStrategyManager
{
	public:

		//! Constructor
		//! \param param the parameters to be used.
		StrategyManager(ALNS_Parameters& param);

		//! Destructor.
		virtual ~StrategyManager();

		//! This function recompute the weights of every strategy and operator managed by this manager.
		void recomputeWeights();

		//! This method selects a strategy with a roulette wheel and set the current strategy curSt.
		void selectStrategy();

		//! This method selects a node destroy operator.
		//! \return a node destroy operator.
		ANodeDestroyOperator& selectNodeDestroyOperator();

		//! This method selects a node repair operator.
		//! \return a node repair operator.
		ANodeRepairOperator& selectNodeRepairOperator(bool nodeDesEmpty);

		//! This method selects a path destroy operator.
		//! \return a path destroy operator.
		APathDestroyOperator& selectPathDestroyOperator();

		//! This method selects a path repair operator.
		//! \return a path repair operator.
		APathRepairOperator& selectPathRepairOperator(bool pathDesEmpty);

		//! get the name of the current selected strategy
		string getCurStName();
		
		//! This method adds a node repair operator to the list
		//! of node repair operator managed by this manager.
		//! \param nodeRepairOperator the node repair operator to be added.
		void addNodeRepairOperator(ANodeRepairOperator& nodeRepairOperator);

		//! This method adds a node destroy operator to the list
		//! of node destroy operator managed by this manager.
		//! \param nodeDestroyOperator the destroy operator to be added.
		void addNodeDestroyOperator(ANodeDestroyOperator& nodeDestroyOperator);

		//! This method adds a path repair operator to the list
		//! of path repair operator managed by this manager.
		//! \param nodeRepairOperator the node repair operator to be added.
		void addPathRepairOperator(APathRepairOperator& pathRepairOperator);

		//! This method adds a path destroy operator to the list
		//! of path destroy operator managed by this manager.
		//! \param destroyOperator the destroy operator to be added.
		void addPathDestroyOperator(APathDestroyOperator& pathDestroyOperator);

		//! This method run some sanity checks to ensure that it is possible
		//! to "safely" use this manager within the ALNS.
		void sanityChecks();

		//! Update the scores of the strategies and operators.
		virtual void updateScores(ANodeDestroyOperator& node_des, ANodeRepairOperator& node_rep, 
									APathDestroyOperator& path_des, APathRepairOperator& path_rep, ALNS_Iteration_Status& status);

		//! Indicate that the optimization process starts.
		virtual void startSignal();

		//! Destroy all the strategies and operators registered to this operator.
		void end();

	private:
		//! the pointer to a node first strategy
		AStrategy* nodeFirstStrategy;

		//! the pointer to a path first strategy
		AStrategy* pathFirstStrategy; 

		//! the pointer to a Node_Null_Destroy operator
		ANodeDestroyOperator* nodeNullDestroy;

		//! the pointer to a Node_Null_Repair operator
		ANodeRepairOperator* nodeNullRepair;

		//! the pointer to a Path_Null_Destroy operator
		APathDestroyOperator* pathNullDestroy;

		//! the pointer to a Path_Null_Repair operator
		APathRepairOperator* pathNullRepair;

		//! The set of node destroy operators.
		vector<AOperator*> nodeDestroyOperators;

		//! The set of node repair operators.
		vector<AOperator*> nodeRepairOperators;

		//! The set of path destroy operators.
		vector<AOperator*> pathDestroyOperators;

		//! The set of path repair operators.
		vector<AOperator*> pathRepairOperators;

		//! The sum of the weights of the strategies.
		double sumWeightsStrategy;

		//! The sum of the weights of the node repair operators.
		double sumWeightsNodeRepair;

		//! The sum of the weights of the node destroy operators.
		double sumWeightsNodeDestroy;

		//! The sum of the weights of the path repair operators.
		double sumWeightsPathRepair;

		//! The sum of the weights of the path destroy operators.
		double sumWeightsPathDestroy;

		//! The paramaters to be used by the ALNS.
		ALNS_Parameters* parameters;

		//! Indicate whether or not the next operators to be return
		//! should be noised or not.
		bool next_noise;

		// //! A counter that indicates the number of times repair operators with noise have been successfull
		// double performanceRepairOperatorsWithNoise;

		// //! A counter that indicates the number of times repair operators without noise have been successfull
		// double performanceNodeRepairOperatorsWithoutNoise;

		//! Use a roulette wheel to select a operator in a vector of operators.
		//! \return the selected strategy. Will update the sumW.
		AOperator& selectOpt(vector<AOperator*>& vecOp, double sumW);

		// //! Use a roulette wheel to select a strategy in a vector of strategies.
		// //! \return the selected strategy.
		// AStrategy& selectSt();

		//! Recompute the weight of an operator.
		void recomputeWeight(AStrategy& st, double& sumW);

};

#endif
