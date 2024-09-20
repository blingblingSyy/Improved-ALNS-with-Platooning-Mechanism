#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <set>
#define NDEBUG 	//add to cancel debug mode
#include "alns/improvedALNS/AStrategy.h"
#include "alns/improvedALNS/AOperator.h"
#include "alns/improvedALNS/ANodeDestroyOperator.h"
#include "alns/improvedALNS/ANodeRepairOperator.h"
#include "alns/improvedALNS/APathDestroyOperator.h"
#include "alns/improvedALNS/APathRepairOperator.h"
#include "alns/improvedALNS/Node_Null_Destroy.h"
#include "alns/improvedALNS/Node_Null_Repair.h"
#include "alns/improvedALNS/Path_Null_Destroy.h"
#include "alns/improvedALNS/Path_Null_Repair.h"
#include "alns/improvedALNS/APathRepairOperator.h"
#include "alns/improvedALNS/AStrategyManager.h"
#include "alns/improvedALNS/StrategyManager.h"
#include "alns/improvedALNS/ALNS_Parameters.h"
#include "alns/improvedALNS/ALNS_Iteration_Status.h"
#include "alns/statistics/Statistics.h"
using namespace std;

StrategyManager::StrategyManager(ALNS_Parameters& param)
{
	parameters = &param;
	sumWeightsStrategy = 0;
	sumWeightsNodeRepair = 0;
	sumWeightsNodeDestroy = 0;
	sumWeightsPathRepair = 0;
	sumWeightsPathDestroy = 0;

	this->nodeFirstStrategy = new AStrategy("nodefirst");
	this->pathFirstStrategy = new AStrategy("pathfirst");
	this->nodeNullDestroy = new Node_Null_Destroy("nodeNullDestroy");
	this->nodeNullRepair = new Node_Null_Repair("nodeNullRepair");   //! reset weights
	this->pathNullDestroy = new Path_Null_Destroy("pathNullDestroy");
	this->pathNullRepair = new Path_Null_Repair("pathNullRepair");

	/* new version */
	nodeDestroyOperators.push_back(nodeNullDestroy);
	pathDestroyOperators.push_back(pathNullDestroy);

    addStrategies();

	// next_noise = false;
	// performanceRepairOperatorsWithNoise = 1;
	// performanceNodeRepairOperatorsWithoutNoise = 1;
}

StrategyManager::~StrategyManager()
{
	// Nothing to be done.
	delete this->nodeFirstStrategy;
	delete this->pathFirstStrategy;
	delete this->nodeNullDestroy;
	delete this->nodeNullRepair;
	delete this->pathNullDestroy;
	delete this->pathNullRepair;
}

void StrategyManager::recomputeWeight(AStrategy& st, double& sumW)
{
	double prevWeight = st.getWeight();
	sumW -= prevWeight;
	double currentScore = st.getScore();
	size_t nbCalls = st.getNumberOfCallsSinceLastEvaluation(); //! the nb of calls in the last segment
	/* new version */
	double newWeight = (1-parameters->getRho())*prevWeight + parameters->getRho()*(static_cast<double>(nbCalls)/static_cast<double>(parameters->getTimeSegmentsIt()))*currentScore;
	/* old version */
	// double newWeight = (1-parameters->getRho())*prevWeight + parameters->getRho()*currentScore/(static_cast<double>(nbCalls));
	//! ensure that the weight is within the bounds.
	if(newWeight > parameters->getMaximumWeight())
	{
		newWeight = parameters->getMaximumWeight();
	}
	if(newWeight < parameters->getMinimumWeight())
	{
		newWeight = parameters->getMinimumWeight();
	}

	sumW += newWeight;
	st.setWeight(newWeight);
	st.resetScore();  //! reset score at the end of each segment to prepare for new segment
	st.resetNumberOfCalls();
}

void StrategyManager::initWeights()
{
	//! node null operators
	int nodeNullDesWeight = 0;
	for(int i = 1; i < nodeDestroyOperators.size(); i++)
	{
		nodeNullDesWeight += nodeDestroyOperators[i]->getWeight();
	}
	nodeNullDestroy->setWeight(nodeNullDesWeight);
	nodeNullRepair->setWeight(nodeNullDesWeight);
	//! path null operators
	int pathNullDesWeight = 0;
	for(int i = 1; i < pathDestroyOperators.size(); i++)
	{
		pathNullDesWeight += pathDestroyOperators[i]->getWeight();
	}
	pathNullDestroy->setWeight(pathNullDesWeight);
	pathNullRepair->setWeight(pathNullDesWeight);
}

void StrategyManager::recomputeWeights()
{
	/* Retrieval of the number of calls for the statistics module. */
	vector<size_t>* nbCalls = new vector<size_t>();
	//! nb of calls for strategy
	nbCalls->push_back(nodeFirstStrategy->getNumberOfCallsSinceLastEvaluation());
	nbCalls->push_back(pathFirstStrategy->getNumberOfCallsSinceLastEvaluation());
	//! nb of calls for node destroy
	for(size_t i = 0; i < nodeDestroyOperators.size(); i++)
	{
		nbCalls->push_back(nodeDestroyOperators[i]->getNumberOfCallsSinceLastEvaluation());
	}
	//! nb of calls for node repair
	for(size_t i = 0; i < nodeRepairOperators.size(); i++)
	{
		nbCalls->push_back(nodeRepairOperators[i]->getNumberOfCallsSinceLastEvaluation());
	}
	//! nb of calls for path destroy
	for(size_t i = 0; i < pathDestroyOperators.size(); i++)
	{
		nbCalls->push_back(pathDestroyOperators[i]->getNumberOfCallsSinceLastEvaluation());
	}
	//! nb of calls for path repair
	for(size_t i = 0; i < pathRepairOperators.size(); i++)
	{
		nbCalls->push_back(pathRepairOperators[i]->getNumberOfCallsSinceLastEvaluation());
	}

	/* weight recomputation */
	//! Weight recomputation for strategies.
	recomputeWeight(dynamic_cast<AStrategy&>(*nodeFirstStrategy), sumWeightsStrategy);
	recomputeWeight(dynamic_cast<AStrategy&>(*pathFirstStrategy), sumWeightsStrategy);
	//! Weight recomputation for node destroy operators.
	for(size_t i = 0; i < nodeDestroyOperators.size(); i++)
	{
		recomputeWeight(dynamic_cast<AStrategy&>(*(nodeDestroyOperators[i])), sumWeightsNodeDestroy);
	}
	//! Weight recomputation for node repair operators.
	for(size_t i = 0; i < nodeRepairOperators.size(); i++)
	{
		recomputeWeight(dynamic_cast<AStrategy&>(*(nodeRepairOperators[i])), sumWeightsNodeRepair);
	}
	//! Weight recomputation for node destroy operators.
	for(size_t i = 0; i < pathDestroyOperators.size(); i++)
	{
		recomputeWeight(dynamic_cast<AStrategy&>(*(pathDestroyOperators[i])), sumWeightsPathDestroy);
	}
	//! Weight recomputation for node repair operators.
	for(size_t i = 0; i < pathRepairOperators.size(); i++)
	{
		recomputeWeight(dynamic_cast<AStrategy&>(*(pathRepairOperators[i])), sumWeightsPathRepair);
	}

	/* Retrieval of the weights for the statistics module. Be careful of the sequence. */
	vector<double>* weightsStats = new vector<double>();
	weightsStats->push_back(nodeFirstStrategy->getWeight());
	weightsStats->push_back(pathFirstStrategy->getWeight());
	for(size_t i = 0; i < nodeDestroyOperators.size(); i++)
	{
		weightsStats->push_back(nodeDestroyOperators[i]->getWeight());
	}
	for(size_t i = 0; i < nodeRepairOperators.size(); i++)
	{
		weightsStats->push_back(nodeRepairOperators[i]->getWeight());
	}
	for(size_t i = 0; i < pathDestroyOperators.size(); i++)
	{
		weightsStats->push_back(pathDestroyOperators[i]->getWeight());
	}
	for(size_t i = 0; i < pathRepairOperators.size(); i++)
	{
		weightsStats->push_back(pathRepairOperators[i]->getWeight());
	}

	stats->addOperatorEntry(weightsStats,nbCalls);

}

void StrategyManager::startSignal()  //! Be careful of the sequence.
{
	vector<string>* names = new vector<string>();
	names->push_back(nodeFirstStrategy->getName());
	names->push_back(pathFirstStrategy->getName());
	for(size_t i = 0; i < nodeDestroyOperators.size(); i++)
	{
		names->push_back(nodeDestroyOperators[i]->getName());
	}
	for(size_t i = 0; i < nodeRepairOperators.size(); i++)
	{
		names->push_back(nodeRepairOperators[i]->getName());
	}
	for(size_t i = 0; i < pathDestroyOperators.size(); i++)
	{
		names->push_back(pathDestroyOperators[i]->getName());
	}
	for(size_t i = 0; i < pathRepairOperators.size(); i++)
	{
		names->push_back(pathRepairOperators[i]->getName());
	}
	stats->addOperatorsNames(names);
}

AOperator& StrategyManager::selectOpt(vector<AOperator*>& vecOp, double sumW)
{
	double randomVal = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
	double randomWeightPos = randomVal*sumW;
	double cumulSum = 0;
	for(size_t i = 0; i < vecOp.size(); i++)
	{
		cumulSum += vecOp[i]->getWeight();
		if(cumulSum >= randomWeightPos)
		{
			// if(next_noise)
			// {
			// 	vecOp[i]->setNoise();
			// }
			// else
			// {
			// 	vecOp[i]->unsetNoise();
			// }
			vecOp[i]->increaseNumberOfCalls();
			return *(vecOp[i]);
		}
	}
	assert(false);  //! write assert(false) at places that is never supposed to reach
	return *(vecOp.back());
}

void StrategyManager::selectStrategy()
{
	double randomVal = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
	double randomWeightPos = randomVal * 0; //sumWeightsStrategy;
	double cumulSum = 0;
	cumulSum += nodeFirstStrategy->getWeight();
	if(cumulSum >= randomWeightPos)
	{
		nodeFirstStrategy->increaseNumberOfCalls();
		nodeFirstStrategy->setHasSelectedCur(true);
		setCurStrategy(NodeFirst);
	}
	else
	{
		pathFirstStrategy->increaseNumberOfCalls();
		pathFirstStrategy->setHasSelectedCur(true);
		setCurStrategy(PathFirst);
	}
}

string StrategyManager::getCurStName()
{
	string strategy_name;
	if(getCurStrategy() == NodeFirst)
	{
		strategy_name = nodeFirstStrategy->getName();
	}
	else if(getCurStrategy() == PathFirst)
	{
		strategy_name = pathFirstStrategy->getName();
	}
	return strategy_name;
}

ANodeDestroyOperator& StrategyManager::selectNodeDestroyOperator()
{
	if(curSt == NodeFirst)
	{
		// vector<AOperator*> updatedNodeDestroyOperators = nodeDestroyOperators;
		// updatedNodeDestroyOperators.erase(remove(updatedNodeDestroyOperators.begin(), updatedNodeDestroyOperators.end(), nodeNullDestroy));
		vector<AOperator*> updatedNodeDestroyOperators(nodeDestroyOperators.begin()+1, nodeDestroyOperators.end());
		return dynamic_cast<ANodeDestroyOperator&>(selectOpt(updatedNodeDestroyOperators, sumWeightsNodeDestroy));
	}
	else //! curSt == PathFirst
	{
		sumWeightsNodeDestroy = sumWeightsNodeDestroy + nodeNullDestroy->getWeight();
		return dynamic_cast<ANodeDestroyOperator&>(selectOpt(nodeDestroyOperators, sumWeightsNodeDestroy));
	}
}

ANodeRepairOperator& StrategyManager::selectNodeRepairOperator(bool nodeDesEmpty)
{
	if(!nodeDesEmpty)
	{
		return dynamic_cast<ANodeRepairOperator&>(selectOpt(nodeRepairOperators, sumWeightsNodeRepair));
	}
	else //! nodeDesEmpty == true
	{
		return dynamic_cast<ANodeRepairOperator&>(*nodeNullRepair);
	}
}

APathDestroyOperator& StrategyManager::selectPathDestroyOperator()
{
	if(curSt == PathFirst)
	{
		// vector<AOperator*> updatedPathDestroyOperators = pathDestroyOperators;
		// updatedPathDestroyOperators.erase(remove(updatedPathDestroyOperators.begin(), updatedPathDestroyOperators.end(), pathNullDestroy));
		vector<AOperator*> updatedPathDestroyOperators(pathDestroyOperators.begin()+1, pathDestroyOperators.end());
		return dynamic_cast<APathDestroyOperator&>(selectOpt(updatedPathDestroyOperators, sumWeightsPathDestroy));
	}
	else //! curSt == NodeFirst
	{
		sumWeightsPathDestroy = sumWeightsPathDestroy + pathNullDestroy->getWeight();
		return dynamic_cast<APathDestroyOperator&>(selectOpt(pathDestroyOperators, sumWeightsPathDestroy));
	}
}

APathRepairOperator& StrategyManager::selectPathRepairOperator(bool pathDesEmpty)
{
	if(!pathDesEmpty)
	{
		return dynamic_cast<APathRepairOperator&>(selectOpt(pathRepairOperators, sumWeightsPathRepair));
	}
	else //! nodeDesEmpty == true
	{
		return dynamic_cast<APathRepairOperator&>(*pathNullRepair);
	}
}

void StrategyManager::addStrategies()
{
	strategies.push_back(nodeFirstStrategy);
	strategies.push_back(pathFirstStrategy);
	sumWeightsStrategy += nodeFirstStrategy->getWeight();
	sumWeightsStrategy += pathFirstStrategy->getWeight();
}

void StrategyManager::addNodeRepairOperator(ANodeRepairOperator& nodeRepairOperator)
{
	//set.find()==set.end() does not work -> different calling returns different vector containers
	vector<string> forbidden_operators = parameters->getForbidenOperators();
	auto forbiden_it = find(forbidden_operators.begin(), forbidden_operators.end(), nodeRepairOperator.getName());
	if(forbiden_it != forbidden_operators.end())
	{
		cout << nodeRepairOperator.getName().c_str() << ": forbidden" << endl;
	}
	else
	{
		nodeRepairOperators.push_back(&nodeRepairOperator);
		sumWeightsNodeRepair += nodeRepairOperator.getWeight();
	}
}

void StrategyManager::addNodeDestroyOperator(ANodeDestroyOperator& nodeDestroyOperator)
{
	vector<string> forbidden_operators = parameters->getForbidenOperators();
	auto forbiden_it = find(forbidden_operators.begin(), forbidden_operators.end(), nodeDestroyOperator.getName());
	if(forbiden_it != forbidden_operators.end())
	{
		cout << nodeDestroyOperator.getName().c_str() << ": forbidden" << endl;
	}
	else
	{
		nodeDestroyOperators.push_back(&nodeDestroyOperator);
		sumWeightsNodeDestroy += nodeDestroyOperator.getWeight();
	}
}

void StrategyManager::addPathRepairOperator(APathRepairOperator& pathRepairOperator)
{
	vector<string> forbidden_operators = parameters->getForbidenOperators();
	auto forbiden_it = find(forbidden_operators.begin(), forbidden_operators.end(), pathRepairOperator.getName());
	if(forbiden_it != forbidden_operators.end())
	{
		cout << pathRepairOperator.getName().c_str() << ": forbidden" << endl;
	}
	else
	{
		pathRepairOperators.push_back(&pathRepairOperator);
		sumWeightsPathRepair += pathRepairOperator.getWeight();
	}
}

void StrategyManager::addPathDestroyOperator(APathDestroyOperator& pathDestroyOperator)
{
	vector<string> forbidden_operators = parameters->getForbidenOperators();
	auto forbiden_it = find(forbidden_operators.begin(), forbidden_operators.end(), pathDestroyOperator.getName());
	if(forbiden_it != forbidden_operators.end())
	{
		cout << pathDestroyOperator.getName().c_str() << ": forbidden" << endl;
	}
	else
	{
		pathDestroyOperators.push_back(&pathDestroyOperator);
		sumWeightsPathDestroy += pathDestroyOperator.getWeight();
	}
}

void StrategyManager::sanityChecks()
{
	assert(!nodeRepairOperators.empty());
	assert(!nodeDestroyOperators.empty());
	assert(!pathRepairOperators.empty());
	assert(!pathDestroyOperators.empty());
	assert(sumWeightsStrategy>0);
	assert(sumWeightsNodeRepair>0);
	assert(sumWeightsNodeDestroy>0);
	assert(sumWeightsPathRepair>0);
	assert(sumWeightsPathDestroy>0);
}

void StrategyManager::updateScores(ANodeDestroyOperator& node_des, ANodeRepairOperator& node_rep,
									APathDestroyOperator& path_des, APathRepairOperator& path_rep, ALNS_Iteration_Status& status)
{
	if(status.getNewBestSolution() == ALNS_Iteration_Status::TRUE)
	{
		if(curSt == NodeFirst)
		{
			nodeFirstStrategy->setScore(nodeFirstStrategy->getScore() + parameters->getSigma1());
		}
		else
		{
			pathFirstStrategy->setScore(pathFirstStrategy->getScore() + parameters->getSigma1());
		}
		node_des.setScore(node_des.getScore() + parameters->getSigma1());
		node_rep.setScore(node_rep.getScore() + parameters->getSigma1());
		path_des.setScore(path_des.getScore() + parameters->getSigma1());
		path_rep.setScore(path_rep.getScore() + parameters->getSigma1());
	}

	if(status.getImproveCurrentSolution() == ALNS_Iteration_Status::TRUE)
	{
		if(curSt == NodeFirst)
		{
			nodeFirstStrategy->setScore(nodeFirstStrategy->getScore() + parameters->getSigma2());
		}
		else
		{
			pathFirstStrategy->setScore(pathFirstStrategy->getScore() + parameters->getSigma2());
		}
		node_des.setScore(node_des.getScore() + parameters->getSigma2());
		node_rep.setScore(node_rep.getScore() + parameters->getSigma2());
		path_des.setScore(path_des.getScore() + parameters->getSigma2());
		path_rep.setScore(path_rep.getScore() + parameters->getSigma2());
	}

	if(status.getImproveCurrentSolution() == ALNS_Iteration_Status::FALSE
			&& status.getAcceptedAsCurrentSolution() == ALNS_Iteration_Status::TRUE
			&& status.getAlreadyKnownSolution() == ALNS_Iteration_Status::FALSE)
	{
		if(curSt == NodeFirst)
		{
			nodeFirstStrategy->setScore(nodeFirstStrategy->getScore() + parameters->getSigma3());
		}
		else
		{
			pathFirstStrategy->setScore(pathFirstStrategy->getScore() + parameters->getSigma3());
		}
		node_des.setScore(node_des.getScore() + parameters->getSigma3());
		node_rep.setScore(node_rep.getScore() + parameters->getSigma3());
		path_des.setScore(path_des.getScore() + parameters->getSigma3());
		path_rep.setScore(path_rep.getScore() + parameters->getSigma3());

	}
	
	// //! TODO wrong logic of setting next_noise -> should set it like setting the strategy
	// //! TODO some repair operators are just random, don't need the noise.
	// if(next_noise)
	// {
	// 	performanceRepairOperatorsWithNoise += 1;
	// }
	// else
	// {
	// 	performanceNodeRepairOperatorsWithoutNoise += 1;
	// }

	// if(parameters->getGlobalNoise()) 
	// {
	// 	double performanceRepairOperatorsGlobal = 0;
	// 	performanceRepairOperatorsGlobal += performanceRepairOperatorsWithNoise;
	// 	performanceRepairOperatorsGlobal += performanceNodeRepairOperatorsWithoutNoise;

	// 	double randomVal = static_cast<double>(rand())/RAND_MAX;
	// 	double randomWeightPos = randomVal*performanceRepairOperatorsGlobal;
	// 	next_noise = (randomWeightPos < performanceRepairOperatorsWithNoise);
	// }
}

void StrategyManager::end()
{
	delete nodeFirstStrategy;
	delete pathFirstStrategy;
	delete nodeNullDestroy;
	delete nodeNullRepair;
	delete pathNullDestroy;
	delete pathNullRepair;
	// for(int i = 0; i < nodeRepairOperators.size(); ++i)
	// {
	// 	delete nodeRepairOperators[i];
	// }
	// for(int i = 0; i < nodeDestroyOperators.size(); ++i)
	// {
	// 	delete nodeDestroyOperators[i];
	// }
	// for(int i = 0; i < pathRepairOperators.size(); ++i)
	// {
	// 	delete pathRepairOperators[i];
	// }
	// for(int i = 0; i < pathDestroyOperators.size(); ++i)
	// {
	// 	delete pathDestroyOperators[i];
	// }
}
