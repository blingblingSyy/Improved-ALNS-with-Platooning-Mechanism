#include <assert.h>
#include <float.h>
#include <iostream>
#include <stdlib.h>
#include <set>
#include <time.h>
#include "src/improvedALNS/ALNS.h"
#include "src/improvedALNS/ALNS_Parameters.h"
#include "src/improvedALNS/ALNS_Iteration_Status.h"
#include "src/improvedALNS/AStrategy.h"
#include "src/improvedALNS/AOperator.h"
#include "src/improvedALNS/ANodeRepairOperator.h"
#include "src/improvedALNS/APathRepairOperator.h"
#include "src/improvedALNS/ANodeDestroyOperator.h"
#include "src/improvedALNS/APathDestroyOperator.h"
#include "src/improvedALNS/Node_Null_Destroy.h"
#include "src/improvedALNS/Node_Null_Repair.h"
#include "src/improvedALNS/Path_Null_Destroy.h"
#include "src/improvedALNS/Path_Null_Repair.h"
#include "src/improvedALNS/AStrategyManager.h"
#include "src/improvedALNS/ISolution.h"
#include "src/improvedALNS/IBestSolutionManager.h"
#include "src/improvedALNS/IUpdatable.h"
#include "src/localsearch/ILocalSearchManager.h"
#include "src/acceptanceModule/IAcceptanceModule.h"
#include "src/statistics/Statistics.h"

using namespace std;


ALNS::ALNS(string instanceName,
		 ISolution& initialSolution,
		 IAcceptanceModule& acceptanceCrit,
		 ALNS_Parameters& parameters,
		 AStrategyManager& stMan,
		 IBestSolutionManager& bestSolMan,
		 ILocalSearchManager& lsMan)
{
	name = instanceName;
	currentSolution = initialSolution.getCopy();
	acceptanceCriterion = &acceptanceCrit;
	param = &parameters;
	lowerBound = -DBL_MAX;
	nbIterationsWC = 0;
	nbIterations = 0;
	nbIterationsWithoutImprovement = 0;
	stManager = &stMan;
	bestSolManager = &bestSolMan;
	lsManager = &lsMan;

	stManager->setStatistics(&stats);

	// add the initial solution in the best solution manager.
	bestSolManager->isNewBestSolution(initialSolution);

	nbIterationsWithoutImprovementCurrent = 0;

	nbIterationsWithoutTransition = 0;

	nbIterationsWithoutLocalSearch = 0;

}

ALNS::~ALNS()
{
	delete currentSolution;
}

//! start the iterations of the Improved ALNS
bool ALNS::solve()
{
	startingTime = clock();
	param->setLock();
	acceptanceCriterion->startSignal();
	stManager->startSignal();
	stats.setStart();
	//! if not meeting the stopping criterion, will keep doing the iterations
	while(!isStoppingCriterionMet())
	{
		performOneIteration();
	}
	//! get the results of the solution, and return whether the solution is feasible or not
	string pathGlob = param->getStatsGlobPath(); //! store the 
	pathGlob += name;
	pathGlob += ".txt";
	string pathOp = param->getStatsOpPath();
	pathOp += name;
	pathOp += ".txt";
	stats.generateStatsFile(pathGlob,pathOp);
	return (*(bestSolManager->begin()))->isFeasible();
}

//! this method performs one iteration of the algorithm.
//! first select the node first strategy or the path first strategy.
//! according to the selected strategy, select the operators and decide the sequence of using these operators.
//! evaluate the quality of the new temporary solution.
//! decide whether to use local search operators to improve the solution.
//! decide wether to accept the new solution by the acceptance criteria.
//! update the scores of the strategies, operators and ALNS iteration status.
void ALNS::performOneIteration()
{
	//! initialize some status of the ALNS iteration
	status.partialReinit();
	
	//! select the strategy and corresponding operators
	stManager->selectStrategy();
	ANodeDestroyOperator& nodeDes = stManager->selectNodeDestroyOperator();
	ANodeRepairOperator& nodeRep = stManager->selectNodeRepairOperator(nodeDes.isEmpty());
	APathDestroyOperator& pathDes = stManager->selectPathDestroyOperator();
	APathRepairOperator& pathRep = stManager->selectPathRepairOperator(pathDes.isEmpty());

	ISolution* newSolution = currentSolution->getCopy();
	
	//! output the number of iterations and solution results
	//! param->getLogFrequence() obtains the value of logFrequence, which represents how often the information is output.
	if(nbIterations % param->getLogFrequency() == 0)
	{
		cout << "[ALNS] it. " << nbIterations << " best sol: " << (*(bestSolManager->begin()))->getObjectiveValue() << " nb known solutions: " << knownKeys.size() << endl;
	}

	//! create new solution with the selected strategy and the operators
	if(stManager->getCurStrategy() == AStrategyManager::NodeFirst)
	{
		nodeDes.destroySolNode(*newSolution);
		status.setAlreadyNodeDestroyed(ALNS_Iteration_Status::TRUE);
		nodeDes.setHasSelectedCur(true);
		nodeRep.repairSolNode(*newSolution);
		status.setAlreadyNodeRepaired(ALNS_Iteration_Status::TRUE);
		pathDes.destroySolPath(*newSolution);
		status.setAlreadyPathDestroyed(ALNS_Iteration_Status::TRUE);
	    //! update the updatable operators
	    for(vector<IUpdatable*>::iterator it = updatableStructures.begin(); it != updatableStructures.end(); it++)
	    {
		    (*it)->update(*newSolution,status);
	    }
		pathRep.repairSolPath(*newSolution);
		status.setAlreadyPathRepaired(ALNS_Iteration_Status::TRUE);
	}
	else if(stManager->getCurStrategy() == AStrategyManager::PathFirst)
	{
		pathDes.destroySolPath(*newSolution);
		status.setAlreadyPathDestroyed(ALNS_Iteration_Status::TRUE);
	    //! update the updatable operators
	    for(vector<IUpdatable*>::iterator it = updatableStructures.begin(); it != updatableStructures.end(); it++)
	    {
		    (*it)->update(*newSolution,status);
	    }
		pathRep.repairSolPath(*newSolution);
		status.setAlreadyPathRepaired(ALNS_Iteration_Status::TRUE);
		nodeDes.destroySolNode(*newSolution);
		status.setAlreadyNodeDestroyed(ALNS_Iteration_Status::TRUE);
		nodeRep.repairSolNode(*newSolution);
		status.setAlreadyNodeRepaired(ALNS_Iteration_Status::TRUE);
	}

	//! update the number of iterations
	nbIterations++;
	status.setIterationId(nbIterations);
	nbIterationsWC++;

	double newCost = newSolution->getObjectiveValue();

	//! check whether the new solution is the new best solution
	isNewBest(newSolution);
	//! check whether the new solution has appeared before
	checkAgainstKnownSolution(*newSolution);
	//! check whether the new solution is better than the current solution
	bool betterThanCurrent = (*newSolution) < (*currentSolution);
	//! if the new solution is better than the current solution
	if(betterThanCurrent)
	{
		nbIterationsWithoutImprovementCurrent = 0; //! clear
		status.setImproveCurrentSolution(ALNS_Iteration_Status::TRUE);
	}
	//! not better
	else
	{
		nbIterationsWithoutImprovementCurrent++;
		status.setImproveCurrentSolution(ALNS_Iteration_Status::FALSE);	//! the solution is not improved
	}
	//! update the number of iterations without improving the current solution
	status.setNbIterationWithoutImprovementCurrent(nbIterationsWithoutImprovementCurrent);

	//! param->getPerformLocalSearch(): whether to use LocalSearch
	//! lsManager->useLocalSearch(*newSolution, status)
	//lsManager->useLocalSearch(*newSolution,status)将返回true如果经过LocalSearch之后的解有改进的话。
	if(param->getPerformLocalSearch() && lsManager->useLocalSearch(*newSolution,status))
	{
		//判断LocalSearch之后的新解是不是最优解。
		bestSolManager->isNewBestSolution(*newSolution);
	}
	//判断是否接受当前的解。
	bool transitionAccepted = transitionCurrentSolution(newSolution);
	//如果接受
	if(transitionAccepted)
	{
		status.setAcceptedAsCurrentSolution(ALNS_Iteration_Status::TRUE);
		nbIterationsWithoutTransition = 0;
	}
	//否则
	else
	{
		status.setAcceptedAsCurrentSolution(ALNS_Iteration_Status::FALSE);
		nbIterationsWithoutTransition++;
	}
	//更新信息
	status.setNbIterationWithoutTransition(nbIterationsWithoutTransition);
	
	//再一次进行LocalSearch操作，以取得更好的效果。
	if(param->getPerformLocalSearch() && lsManager->useLocalSearch(*newSolution,status))
	{
		bestSolManager->isNewBestSolution(*newSolution);
		if(status.getAcceptedAsCurrentSolution() == ALNS_Iteration_Status::TRUE)
		{
			transitionCurrentSolution(newSolution);
		}
	}

	//对destroy,repair方法进行成绩更新。
	stManager->updateScores(destroy,repair,status);

	//记录本次迭代过程的一些信息。
	stats.addEntry(static_cast<double>(clock()-startingTime)/CLOCKS_PER_SEC,nbIterations,destroy.getName(),repair.getName(),newCost,currentSolution->getObjectiveValue(),(*(bestSolManager->begin()))->getObjectiveValue(),knownKeys.size());

	//更新destroy,repair方法的权重。是在进行了一定迭代次数以后才更新的，具体次数由param->getTimeSegmentsIt()获得。
	if(nbIterationsWC % param->getTimeSegmentsIt() == 0)
	{
		stManager->recomputeWeights();
		nbIterationsWC = 0;
	}
	//接口，在求解的过程中某些内容需要更新。
	for(vector<IUpdatable*>::iterator it = updatableStructures.begin(); it != updatableStructures.end(); it++)
	{
		(*it)->update(*newSolution,status);
	}
	
	//如果有需要，将当前解转变成最优解再进行下一次迭代操作。
	currentSolution = bestSolManager->reloadBestSolution(currentSolution,status);

	delete newSolution;
}

//检查该解是否是之前出现过的解。主要原理是利用一个解的哈希表，所有第一次出现的解都会生成一个唯一的哈希值存于哈希表中。
//将传入解的哈希值在哈希表中进行匹配，如果存在，那么这个解之前已经出现过了，否则就是独一无二的新解。
bool ALNS::checkAgainstKnownSolution(ISolution& sol)
{
	bool notKnownSolution = false;
	long long keySol = sol.getHash();
 	//哈希值匹配
	if(knownKeys.find(keySol) == knownKeys.end())
	{
		notKnownSolution = true;
		knownKeys.insert(keySol);
	}
	//之前已经出现过的解。
	if(!notKnownSolution)
	{
		status.setAlreadyKnownSolution(ALNS_Iteration_Status::TRUE);
	}
	//全新的解，之前没有出现过。
	else
	{
		status.setAlreadyKnownSolution(ALNS_Iteration_Status::FALSE);
	}
	return notKnownSolution;
}

//用来判断解是否为新的最优解，并做出相应的设置。
bool ALNS::isNewBest(ISolution* newSol)
{
	//如果是新的最优解
	if(bestSolManager->isNewBestSolution(*newSol))
	{
		status.setNewBestSolution(ALNS_Iteration_Status::TRUE);
		nbIterationsWithoutImprovement = 0;
		status.setNbIterationWithoutImprovement(nbIterationsWithoutImprovement);
		status.setNbIterationWithoutImprovementSinceLastReload(0);
		return true;
	}
	//如果不是。
	else
	{
		status.setNewBestSolution(ALNS_Iteration_Status::FALSE);
		nbIterationsWithoutImprovement++;
		status.setNbIterationWithoutImprovement(nbIterationsWithoutImprovement);
		status.setNbIterationWithoutImprovementSinceLastReload(status.getNbIterationWithoutImprovementSinceLastReload()+1);
		return false;
	}
}

//利用接受准则判断是否要接受当前的解作为新的解。
bool ALNS::transitionCurrentSolution(ISolution* newSol)
{
	//如果接受。
	if(acceptanceCriterion->transitionAccepted(*bestSolManager,*currentSolution,*newSol,status))
	{
		delete currentSolution;
		currentSolution = newSol->getCopy();
		return true;
	}
	//不接受，原来解不变，什么也不做。
	else
	{
		return false;
	}
}

//判断算法迭代是否遇到终止条件
bool ALNS::isStoppingCriterionMet()
{
	//是否找到最优可行解。
	if((*(bestSolManager->begin()))->isFeasible() && (*(bestSolManager->begin()))->getObjectiveValue() == lowerBound)
	{
		return true;
	}
	else
	{
		switch(param->getStopCrit())
		{
			//是否达到最大迭代次数。
			case ALNS_Parameters::MAX_IT: {
				return nbIterations >= param->getMaxNbIterations();
			}
			//是否达到最大限制运行时间。
			case ALNS_Parameters::MAX_RT: {
				clock_t currentTime = clock();
				double elapsed = (static_cast<double>(currentTime - startingTime)) / CLOCKS_PER_SEC;
				return elapsed >= param->getMaxRunningTime();
			}
			//the maximum number of iterations without improvement. 
			case ALNS_Parameters::MAX_IT_NO_IMP: {
				return nbIterationsWithoutImprovement >= param->getMaxNbIterationsNoImp();
			}
			//a mix of the MAX_IT, MAX_RT and MAX_IT_NO_IMP.
			case ALNS_Parameters::ALL: {
				if(nbIterations >= param->getMaxNbIterations())
				{
					return true;
				}
				if(nbIterationsWithoutImprovement >= param->getMaxNbIterationsNoImp())
				{
					return true;
				}
				clock_t currentTime = clock();
				double elapsed = (static_cast<double>(currentTime - startingTime)) / CLOCKS_PER_SEC;
				if(elapsed >= param->getMaxRunningTime())
				{
					return true;
				}
				return false;
			}

			default: {
				assert(false);
				return false;
			}
		}
	}

}

void ALNS::end()
{
	stManager->end();
	delete stManager;
	delete acceptanceCriterion;
	delete lsManager;
	delete bestSolManager;
}
