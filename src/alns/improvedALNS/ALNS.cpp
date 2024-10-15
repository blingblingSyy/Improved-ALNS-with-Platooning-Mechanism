#include <assert.h>
#include <float.h>
#include <iostream>
#include <stdlib.h>
#include <set>
#include <time.h>
#include "alns/improvedALNS/ALNS.h"
#include "alns/improvedALNS/ALNS_Parameters.h"
#include "alns/improvedALNS/ALNS_Iteration_Status.h"
#include "alns/improvedALNS/AStrategy.h"
#include "alns/improvedALNS/AOperator.h"
#include "alns/improvedALNS/ANodeRepairOperator.h"
#include "alns/improvedALNS/APathRepairOperator.h"
#include "alns/improvedALNS/ANodeDestroyOperator.h"
#include "alns/improvedALNS/APathDestroyOperator.h"
#include "alns/improvedALNS/Node_Null_Destroy.h"
#include "alns/improvedALNS/Node_Null_Repair.h"
#include "alns/improvedALNS/Path_Null_Destroy.h"
#include "alns/improvedALNS/Path_Null_Repair.h"
#include "alns/improvedALNS/AStrategyManager.h"
#include "alns/improvedALNS/ISolution.h"
#include "alns/improvedALNS/IBestSolutionManager.h"
#include "alns/improvedALNS/IUpdatable.h"
#include "alns/localsearch/ILocalSearchManager.h"
#include "alns/acceptanceModule/IAcceptanceModule.h"
#include "alns/statistics/Statistics.h"

// #define NDEBUG

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

	// long long key_initialSol = initialSolution.getHash();
	// knownKeys.insert(key_initialSol);

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
	// //! select the strategy and corresponding operators
	// stManager->addStrategies();
    // stManager->initNullOperators();
	stManager->startSignal();
	stats.setStart();
	//! if not meeting the stopping criterion, will keep doing the iterations
	while(!isStoppingCriterionMet())
	{
		performOneIteration();
	}
	//! record the cpu time
	clock_t currentTime = clock();
	cpu = static_cast<double>(currentTime - startingTime);
	//! get the results of the solution, and return whether the solution is feasible or not
	string pathGlob = result_dir + param->getStatsGlobPath() + "_";
	pathGlob += name;
	pathGlob += ".txt";
	string pathOp = result_dir + param->getStatsOpPath() + "_";
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

	//! select strategies
	stManager->selectStrategy();

	// cout << "iteration: " << nbIterations << endl;
	// cout << "strategy: " << stManager->getCurStName() << endl;

	ANodeDestroyOperator& nodeDes = stManager->selectNodeDestroyOperator();
	ANodeRepairOperator& nodeRep = stManager->selectNodeRepairOperator(nodeDes.isEmpty());
	APathDestroyOperator& pathDes = stManager->selectPathDestroyOperator();
	APathRepairOperator& pathRep = stManager->selectPathRepairOperator(pathDes.isEmpty());

	// cout << "ALNS: APathDestroyOperator& pathDes: " << "\t ";
	// cout << pathDes.getName() << "\t " << pathDes.getNumberOfCallsSinceLastEvaluation() << endl;

	ISolution* newSolution = currentSolution->getCopy();
	newSolution->updateSol();
	
	//! output the number of iterations and solution results
	//! param->getLogFrequence() obtains the value of logFrequence, which represents how often the information is output.
	if(nbIterations % param->getLogFrequency() == 0)
	{
		cout << "[ALNS] it. " << nbIterations << " best sol: " << (*(bestSolManager->begin()))->getObjectiveValue() << "; nb known solutions: " << knownKeys.size() << endl;
	}

	//! create new solution with the selected strategy and the operators
	if(stManager->getCurStrategy() == AStrategyManager::NodeFirst)
	{
		status.setAlreadyStrategySelected(ALNS_Iteration_Status::TRUE);
		//! node destroy
		nodeDes.destroySolNode(*newSolution);
		status.setAlreadyNodeDestroyed(ALNS_Iteration_Status::TRUE);
		nodeDes.setHasSelectedCur(true);
		//! node repair
		nodeRep.repairSolNode(*newSolution);
		status.setAlreadyNodeRepaired(ALNS_Iteration_Status::TRUE);
		nodeRep.setHasSelectedCur(true);
		//! path destroy
		pathDes.destroySolPath(*newSolution);
		status.setAlreadyPathDestroyed(ALNS_Iteration_Status::TRUE);
		pathDes.setHasSelectedCur(true);
		//! path repair
		pathRep.repairSolPath(*newSolution);
		status.setAlreadyPathRepaired(ALNS_Iteration_Status::TRUE);
		pathRep.setHasSelectedCur(true);
	}
	else if(stManager->getCurStrategy() == AStrategyManager::PathFirst)
	{
		status.setAlreadyStrategySelected(ALNS_Iteration_Status::TRUE);
		//! path destroy
		pathDes.destroySolPath(*newSolution);
		status.setAlreadyPathDestroyed(ALNS_Iteration_Status::TRUE);
		pathDes.setHasSelectedCur(true);
		//! path repair
		pathRep.repairSolPath(*newSolution);
		status.setAlreadyPathRepaired(ALNS_Iteration_Status::TRUE);
		pathRep.setHasSelectedCur(true);
		//! node destroy
		nodeDes.destroySolNode(*newSolution);
		status.setAlreadyNodeDestroyed(ALNS_Iteration_Status::TRUE);
		nodeDes.setHasSelectedCur(true);
		//! node repair
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
	nbIterations++; //! nbIterations starts from 1
	status.setIterationId(nbIterations);
	nbIterationsWC++;

	newSolution->makePlatoons();
	newSolution->recomputeCost();
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
		delete currentSolution; //! may have error in deleting the currentSolution
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
		cout << "Stop: the objective value of the best solution reaches the lower bound.";
		return true;
	}
	else
	{
		switch(param->getStopCrit())
		{
			//! whether reach the maximum iteration times
			case ALNS_Parameters::MAX_IT:
			{
				bool maxit = nbIterations >= param->getMaxNbIterations();
				if(maxit)
				{
					cout << "Stop: the algorithm reaches the maximum number of iterations.";
				}
				return maxit;
			}
			//! whether reach the maximum running time limit
			case ALNS_Parameters::MAX_RT:
			{
				clock_t currentTime = clock();
				double elapsed = (static_cast<double>(currentTime - startingTime)) / CLOCKS_PER_SEC;
				bool maxrt = elapsed >= param->getMaxRunningTime();
				if(maxrt)
				{
					cout << "Stop: the algorithm reaches the maximum running time.";
				}
				return maxrt;
			}
			//! whether reach the maximum number of iterations without improvement. 
			//! modify to the criteria in Part A paper
			case ALNS_Parameters::MAX_IT_NO_IMP:
			{
				// if(nbIterations > 0)
				// 	return stats.getOneBestCost(nbIterations-1) <= 5021; //! for test2.txt

				if(nbIterations >= param->getMinNbIterations() && nbIterations > 2*param->getLookBackIterations())
				{
					double accum_objval_before = 0;
					double accum_objval_after = 0;
					for(int i = nbIterations - param->getLookBackIterations(); i < nbIterations; i++)
					{
						accum_objval_after += stats.getOneBestCost(i);
					}
					for(int i = nbIterations - 2*param->getLookBackIterations(); i < nbIterations - param->getLookBackIterations(); i++)
					{
						accum_objval_before += stats.getOneBestCost(i);
					}
					bool maxitnoimp = (accum_objval_before / accum_objval_after - 1 <= param->getObjImpThreshold());
					if(maxitnoimp)
					{
						cout << "Stop: the algorithm reaches the maximum number of iterations without improvement by threshold.";
					}
					return maxitnoimp;
				}
				return false;

				// return nbIterationsWithoutImprovement >= param->getMaxNbIterationsNoImp();
			}
			//! a mix of the MAX_IT, MAX_RT and MAX_IT_NO_IMP.
			case ALNS_Parameters::ALL:
			{
				if(nbIterations >= param->getMaxNbIterations())
				{
					cout << "Stop: the algorithm reaches the maximum number of iterations.";
					return true;
				}
				if(nbIterationsWithoutImprovement >= param->getMaxNbIterationsNoImp())
				{
					cout << "Stop: the algorithm reaches the maximum number of iterations without improvement by threshold.";
					return true;
				}
				clock_t currentTime = clock();
				double elapsed = (static_cast<double>(currentTime - startingTime)) / CLOCKS_PER_SEC;
				if(elapsed >= param->getMaxRunningTime())
				{
					cout << "Stop: the algorithm reaches the maximum running time.";
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

double ALNS::getCpuTime()
{
	return cpu / (double) CLOCKS_PER_SEC;
}