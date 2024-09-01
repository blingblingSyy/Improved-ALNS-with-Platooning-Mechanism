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
	string pathGlob = param->getStatsGlobPath();
	pathGlob += name;
	pathGlob += ".txt";
	string pathOp = param->getStatsOpPath();
	pathOp += name;
	pathOp += ".txt";
	stats.generateStatsFile(pathGlob, pathOp);
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
		status.setAlreadyStrategySelected(ALNS_Iteration_Status::TRUE);
		nodeDes.destroySolNode(*newSolution);
		status.setAlreadyNodeDestroyed(ALNS_Iteration_Status::TRUE);
		nodeDes.setHasSelectedCur(true);
		nodeRep.repairSolNode(*newSolution);
		status.setAlreadyNodeRepaired(ALNS_Iteration_Status::TRUE);
		nodeRep.setHasSelectedCur(true);
		pathDes.destroySolPath(*newSolution);
		status.setAlreadyPathDestroyed(ALNS_Iteration_Status::TRUE);
		pathDes.setHasSelectedCur(true);
		pathRep.repairSolPath(*newSolution);
		status.setAlreadyPathRepaired(ALNS_Iteration_Status::TRUE);
		pathRep.setHasSelectedCur(true);
	}
	else if(stManager->getCurStrategy() == AStrategyManager::PathFirst)
	{
		status.setAlreadyStrategySelected(ALNS_Iteration_Status::TRUE);
		pathDes.destroySolPath(*newSolution);
		status.setAlreadyPathDestroyed(ALNS_Iteration_Status::TRUE);
		pathDes.setHasSelectedCur(true);
		pathRep.repairSolPath(*newSolution);
		status.setAlreadyPathRepaired(ALNS_Iteration_Status::TRUE);
		pathRep.setHasSelectedCur(true);
		nodeDes.destroySolNode(*newSolution);
		status.setAlreadyNodeDestroyed(ALNS_Iteration_Status::TRUE);
		nodeDes.setHasSelectedCur(true);
		nodeRep.repairSolNode(*newSolution);
		status.setAlreadyNodeRepaired(ALNS_Iteration_Status::TRUE);
		nodeRep.setHasSelectedCur(true);
	}
	//! update the updatable operators
	// for(vector<IUpdatable*>::iterator it = updatableStructures.begin(); it != updatableStructures.end(); it++)
	// {
	// 	(*it)->update(*newSolution,status);
	// }

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

	//! param->getPerformLocalSearch(): whether to use LocalSearch (2 cases: see in the useLocalSearch())
	//! lsManager->useLocalSearch(*newSolution, status)
	//! lsManager->useLocalSearch(*newSolution,status) will return true if the solution is improved after LocalSearch
	if(param->getPerformLocalSearch() && lsManager->useLocalSearch(*newSolution,status))
	{
		//! evaluate whether the solution after LocalSearch is the new best solution
		bestSolManager->isNewBestSolution(*newSolution);
	}

	//! evaluate whether to accept the current solution or not
	bool transitionAccepted = transitionCurrentSolution(newSolution);
	//! if accepted
	if(transitionAccepted)
	{
		status.setAcceptedAsCurrentSolution(ALNS_Iteration_Status::TRUE);
		nbIterationsWithoutTransition = 0;
	}
	//! or else
	else
	{
		status.setAcceptedAsCurrentSolution(ALNS_Iteration_Status::FALSE);
		nbIterationsWithoutTransition++;
	}
	//update the information
	status.setNbIterationWithoutTransition(nbIterationsWithoutTransition);
	
	//! performance once more LocalSearch to achieve a better result
	if(param->getPerformLocalSearch() && lsManager->useLocalSearch(*newSolution,status))
	{
		bestSolManager->isNewBestSolution(*newSolution);
		if(status.getAcceptedAsCurrentSolution() == ALNS_Iteration_Status::TRUE)
		{
			transitionCurrentSolution(newSolution);
		}
	}

	//! update the score for all strategies and operators
	stManager->updateScores(nodeDes, nodeRep, pathDes, pathRep, status);

	//record the information (statistics) of the current iteration
	stats.addEntry(static_cast<double>(clock()-startingTime)/CLOCKS_PER_SEC, 
					nbIterations,
					stManager->getCurStName(),
					nodeDes.getName(),
					nodeRep.getName(),
					pathDes.getName(),
					pathRep.getName(),
					newCost,
					currentSolution->getObjectiveValue(),
					(*(bestSolManager->begin()))->getObjectiveValue(),
					knownKeys.size());

	//! update the weights for all strategies and operators -> update every segment
	if(nbIterationsWC % param->getTimeSegmentsIt() == 0)
	{
		stManager->recomputeWeights();
		nbIterationsWC = 0;
	}
	//! update during the solution iteration -> do this update because performing the local search
	for(vector<IUpdatable*>::iterator it = updatableStructures.begin(); it != updatableStructures.end(); it++)
	{
		(*it)->update(*newSolution,status);
	}
	
	//! reload the best solution as the current solution for the next iteration
	//! do this because sometimes a worse solution can be accepted in the transition period
	currentSolution = bestSolManager->reloadBestSolution(currentSolution,status);

	delete newSolution;
}

//! check against whether the solution has been appeared before 
//! by checking whether the hash key of the solution has been appeared in the hash key list before
bool ALNS::checkAgainstKnownSolution(ISolution& sol)
{
	bool notKnownSolution = false;
	long long keySol = sol.getHash();
 	//! check in the hash key list
	if(knownKeys.find(keySol) == knownKeys.end())
	{
		notKnownSolution = true;
		knownKeys.insert(keySol);
	}
	//! if the solution has appeared before
	if(!notKnownSolution)
	{
		status.setAlreadyKnownSolution(ALNS_Iteration_Status::TRUE);
	}
	//! if the solution has never appeared before
	else
	{
		status.setAlreadyKnownSolution(ALNS_Iteration_Status::FALSE);
	}
	return notKnownSolution;
}

//! check whether the current solution is the new best solution
bool ALNS::isNewBest(ISolution* newSol)
{
	//! if this is the new best solution
	if(bestSolManager->isNewBestSolution(*newSol))
	{
		status.setNewBestSolution(ALNS_Iteration_Status::TRUE);
		nbIterationsWithoutImprovement = 0;
		status.setNbIterationWithoutImprovement(nbIterationsWithoutImprovement);
		status.setNbIterationWithoutImprovementSinceLastReload(0);  //! improvement: better than the previous best solution
		return true;
	}
	//! if else
	else
	{
		status.setNewBestSolution(ALNS_Iteration_Status::FALSE);
		nbIterationsWithoutImprovement++;
		status.setNbIterationWithoutImprovement(nbIterationsWithoutImprovement);
		status.setNbIterationWithoutImprovementSinceLastReload(status.getNbIterationWithoutImprovementSinceLastReload()+1);
		return false;
	}
}

//! use acceptance module to check whether to use the new temperary solution as the current solution
bool ALNS::transitionCurrentSolution(ISolution* newSol)
{
	//! if accepted
	if(acceptanceCriterion->transitionAccepted(*bestSolManager,*currentSolution,*newSol,status))
	{
		delete currentSolution;
		currentSolution = newSol->getCopy();
		return true;
	}
	//! if not accepted
	else
	{
		return false;
	}
}

//! evaluate whether the algorithm has met the stopping criteria
bool ALNS::isStoppingCriterionMet()
{
	//! whether the algorithm has found the new best feasible solution 是否找到最优可行解。
	if((*(bestSolManager->begin()))->isFeasible() && (*(bestSolManager->begin()))->getObjectiveValue() == lowerBound)
	{
		return true;
	}
	else
	{
		switch(param->getStopCrit())
		{
			//! whether reach the maximum iteration times
			case ALNS_Parameters::MAX_IT:
			{
				return nbIterations >= param->getMaxNbIterations();
			}
			//! whether reach the maximum running time limit
			case ALNS_Parameters::MAX_RT:
			{
				clock_t currentTime = clock();
				double elapsed = (static_cast<double>(currentTime - startingTime)) / CLOCKS_PER_SEC;
				return elapsed >= param->getMaxRunningTime();
			}
			//! whether reach the maximum number of iterations without improvement. 
			//! modify to the criteria in Part A paper
			case ALNS_Parameters::MAX_IT_NO_IMP:
			{
				if(nbIterations >= param->getMinNbIterations())
				{
					double accum_objval_before = 0;
					double accum_objval_after = 0;
					for(int i = nbIterations - param->getLookBackIterations(); i <= nbIterations; i++)
					{
						accum_objval_after += stats.getOneBestCost(i);
					}
					for(int i = nbIterations - 2*param->getLookBackIterations(); i <= nbIterations - param->getLookBackIterations(); i++)
					{
						accum_objval_before += stats.getOneBestCost(i);
					}
					return (accum_objval_before / accum_objval_after - 1 <= param->getObjImpThreshold());
				}
				return false;
				// return nbIterationsWithoutImprovement >= param->getMaxNbIterationsNoImp();
			}
			//! a mix of the MAX_IT, MAX_RT and MAX_IT_NO_IMP.
			case ALNS_Parameters::ALL:
			{
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

			default:
			{
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
