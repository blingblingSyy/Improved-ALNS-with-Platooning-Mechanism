// /* ALNS_Framework - a framework to develop ALNS based solvers
//  *
//  * Copyright (C) 2012 Renaud Masson
//  *
//  * This library is free software; you can redistribute it and/or
//  * modify it either under the terms of the GNU Lesser General Public
//  * License version 3 as published by the Free Software Foundation
//  * (the "LGPL"). If you do not alter this notice, a recipient may use
//  * your version of this file under the LGPL.
//  *
//  * You should have received a copy of the LGPL along with this library
//  * in the file COPYING-LGPL-3; if not, write to the Free Software
//  * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA
//  *
//  * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY
//  * OF ANY KIND, either express or implied. See the LGPL for
//  * the specific language governing rights and limitations.
//  *
//  * The Original Code is the ALNS_Framework library.
//  *
//  *
//  * Contributor(s):
//  *	Renaud Masson
//  */
// #include <assert.h>
// #include <float.h>
// #include <iostream>
// #include <stdlib.h>
// #include <set>
// #include <time.h>
// #include "ALNS.h"
// #include "ALNS_Parameters.h"
// #include "ALNS_Iteration_Status.h"
// #include "ISolution.h"
// #include "AOperatorManager.h"
// #include "ARepairOperator.h"
// #include "ADestroyOperator.h"
// #include "AOperator.h"
// #include "IUpdatable.h"
// #include "IBestSolutionManager.h"
// #include "../localsearch/ILocalSearchManager.h"
// #include "../acceptanceModule/IAcceptanceModule.h"
// #include "../statistics/Statistics.h"

// using namespace std;


// ALNS::ALNS(string instanceName,
// 		 ISolution& initialSolution,
// 		 IAcceptanceModule& acceptanceCrit,
// 		 ALNS_Parameters& parameters,
// 		 AOperatorManager& opMan,
// 		 IBestSolutionManager& bestSolMan,
// 		 ILocalSearchManager& lsMan)
// {
// 	name = instanceName;
// 	currentSolution = initialSolution.getCopy();
// 	acceptanceCriterion = &acceptanceCrit;
// 	param = &parameters;
// 	lowerBound = -DBL_MAX;
// 	nbIterationsWC = 0;
// 	nbIterations = 0;
// 	nbIterationsWithoutImprovement = 0;
// 	opManager = &opMan;
// 	bestSolManager = &bestSolMan;
// 	lsManager = &lsMan;

// 	opManager->setStatistics(&stats);

// 	// We add the initial solution in the best solution manager.
// 	bestSolManager->isNewBestSolution(initialSolution);

// 	nbIterationsWithoutImprovementCurrent = 0;

// 	nbIterationsWithoutTransition = 0;

// 	nbIterationsWithoutLocalSearch = 0;


// }

// ALNS::~ALNS()
// {
// 	delete currentSolution;
// }

// //开始ALNS算法的迭代过程。这是将下面的模块组装起来，然后跑算法求解的过程了。
// bool ALNS::solve()
// {
// 	startingTime = clock();
// 	param->setLock();
// 	acceptanceCriterion->startSignal();
// 	opManager->startSignal();
// 	stats.setStart();
// 	//如果没有遇到终止条件，那么将一次次迭代下去。
// 	while(!isStoppingCriterionMet())
// 	{
// 		performOneIteration();
// 	}
// 	//获取运行结果，返回解是否可行。
// 	string pathGlob = param->getStatsGlobPath();
// 	pathGlob += name;
// 	pathGlob += ".txt";
// 	string pathOp = param->getStatsOpPath();
// 	pathOp += name;
// 	pathOp += ".txt";
// 	stats.generateStatsFile(pathGlob,pathOp);
// 	return (*(bestSolManager->begin()))->isFeasible();
// }

// //该方法执行一次迭代操作。也是整个流程比较核心的部分。大概过程是：
// //1）先进行destroy操作和进行repair操作，然后判断新解质量。
// //2）而后看情况要不要使用LocalSearch进行搜索，再用接受准则看是否要接受新解。
// //3）最后更新destroy操作和repair操作的成绩。再做一些状态量的处理等。
// void ALNS::performOneIteration()
// {
// 	//重新初始化一些状态量。
// 	status.partialReinit();

// 	//选择Repair和Destroy方法
// 	ARepairOperator& repair = opManager->selectRepairOperator();
// 	ADestroyOperator& destroy = opManager->selectDestroyOperator();
	
// 	ISolution* newSolution = currentSolution->getCopy();
	
// 	//输出迭代次数等信息。 param->getLogFrequency()获取的是logFrequency变量的值，logFrequency变量表示的意思是每隔多少次输出一下相关信息。
// 	if(nbIterations % param->getLogFrequency() == 0)
// 	{
// 		cout << "[ALNS] it. " << nbIterations << " best sol: " << (*(bestSolManager->begin()))->getObjectiveValue() << " nb known solutions: " << knownKeys.size() << endl;
// 	}
	
// 	//destroy操作
// 	destroy.destroySolution(*newSolution);
// 	//更新状态
// 	status.setAlreadyDestroyed(ALNS_Iteration_Status::TRUE);
// 	status.setAlreadyRepaired(ALNS_Iteration_Status::FALSE);	//未进行repair操作
// 	//表示newSolution还是status的信息对解进行更新。这里只提供接口，后面应用的时候要具体重写。
// 	for(vector<IUpdatable*>::iterator it = updatableStructures.begin(); it != updatableStructures.end(); it++)
// 	{
// 		(*it)->update(*newSolution,status);
// 	}
// 	//进行repair操作
// 	repair.repairSolution(*newSolution);
// 	status.setAlreadyRepaired(ALNS_Iteration_Status::TRUE);

// 	//更新迭代次数
// 	nbIterations++;
// 	status.setIterationId(nbIterations);
// 	nbIterationsWC++;

// 	double newCost = newSolution->getObjectiveValue();
// 	//判断新生产的解是不是新的最优解
// 	isNewBest(newSolution);
// 	//判断新生成的解之前有没有出现过
// 	checkAgainstKnownSolution(*newSolution);
// 	//判断新生成的解和当前解谁更优
// 	bool betterThanCurrent = (*newSolution)<(*currentSolution);
// 	//如果新生成的解更优
// 	if(betterThanCurrent)
// 	{
// 		nbIterationsWithoutImprovementCurrent = 0; //清0
// 		status.setImproveCurrentSolution(ALNS_Iteration_Status::TRUE);
// 	}
// 	//否则
// 	else
// 	{
// 		nbIterationsWithoutImprovementCurrent++;
// 		status.setImproveCurrentSolution(ALNS_Iteration_Status::FALSE);	//解没有得到提高
// 	}
// 	//更新状态
// 	status.setNbIterationWithoutImprovementCurrent(nbIterationsWithoutImprovementCurrent);

// 	//param->getPerformLocalSearch()指出要不要用LocalSearch，然后再用LocalSearch对新生成的解进行搜索。
// 	//lsManager->useLocalSearch(*newSolution,status)将返回true如果经过LocalSearch之后的解有改进的话。
// 	if(param->getPerformLocalSearch() && lsManager->useLocalSearch(*newSolution,status))
// 	{
// 		//判断LocalSearch之后的新解是不是最优解。
// 		bestSolManager->isNewBestSolution(*newSolution);
// 	}
// 	//判断是否接受当前的解。
// 	bool transitionAccepted = transitionCurrentSolution(newSolution);
// 	//如果接受
// 	if(transitionAccepted)
// 	{
// 		status.setAcceptedAsCurrentSolution(ALNS_Iteration_Status::TRUE);
// 		nbIterationsWithoutTransition = 0;
// 	}
// 	//否则
// 	else
// 	{
// 		status.setAcceptedAsCurrentSolution(ALNS_Iteration_Status::FALSE);
// 		nbIterationsWithoutTransition++;
// 	}
// 	//更新信息
// 	status.setNbIterationWithoutTransition(nbIterationsWithoutTransition);
	
// 	//再一次进行LocalSearch操作，以取得更好的效果。
// 	if(param->getPerformLocalSearch() && lsManager->useLocalSearch(*newSolution,status))
// 	{
// 		bestSolManager->isNewBestSolution(*newSolution);
// 		if(status.getAcceptedAsCurrentSolution() == ALNS_Iteration_Status::TRUE)
// 		{
// 			transitionCurrentSolution(newSolution);
// 		}
// 	}

// 	//对destroy,repair方法进行成绩更新。
// 	opManager->updateScores(destroy,repair,status);

// 	//记录本次迭代过程的一些信息。
// 	stats.addEntry(static_cast<double>(clock()-startingTime)/CLOCKS_PER_SEC,nbIterations,destroy.getName(),repair.getName(),newCost,currentSolution->getObjectiveValue(),(*(bestSolManager->begin()))->getObjectiveValue(),knownKeys.size());

// 	//更新destroy,repair方法的权重。是在进行了一定迭代次数以后才更新的，具体次数由param->getTimeSegmentsIt()获得。
// 	if(nbIterationsWC % param->getTimeSegmentsIt() == 0)
// 	{
// 		opManager->recomputeWeights();
// 		nbIterationsWC = 0;
// 	}
// 	//接口，在求解的过程中某些内容需要更新。
// 	for(vector<IUpdatable*>::iterator it = updatableStructures.begin(); it != updatableStructures.end(); it++)
// 	{
// 		(*it)->update(*newSolution,status);
// 	}
	
// 	//如果有需要，将当前解转变成最优解再进行下一次迭代操作。
// 	currentSolution = bestSolManager->reloadBestSolution(currentSolution,status);

// 	delete newSolution;
// }

// //检查该解是否是之前出现过的解。主要原理是利用一个解的哈希表，所有第一次出现的解都会生成一个唯一的哈希值存于哈希表中。
// //将传入解的哈希值在哈希表中进行匹配，如果存在，那么这个解之前已经出现过了，否则就是独一无二的新解。
// bool ALNS::checkAgainstKnownSolution(ISolution& sol)
// {
// 	bool notKnownSolution = false;
// 	long long keySol = sol.getHash();
//  	//哈希值匹配
// 	if(knownKeys.find(keySol) == knownKeys.end())
// 	{
// 		notKnownSolution = true;
// 		knownKeys.insert(keySol);
// 	}
// 	//之前已经出现过的解。
// 	if(!notKnownSolution)
// 	{
// 		status.setAlreadyKnownSolution(ALNS_Iteration_Status::TRUE);
// 	}
// 	//全新的解，之前没有出现过。
// 	else
// 	{
// 		status.setAlreadyKnownSolution(ALNS_Iteration_Status::FALSE);
// 	}
// 	return notKnownSolution;
// }

// //用来判断解是否为新的最优解，并做出相应的设置。
// bool ALNS::isNewBest(ISolution* newSol)
// {
// 	//如果是新的最优解
// 	if(bestSolManager->isNewBestSolution(*newSol))
// 	{
// 		status.setNewBestSolution(ALNS_Iteration_Status::TRUE);
// 		nbIterationsWithoutImprovement = 0;
// 		status.setNbIterationWithoutImprovement(nbIterationsWithoutImprovement);
// 		status.setNbIterationWithoutImprovementSinceLastReload(0);
// 		return true;
// 	}
// 	//如果不是。
// 	else
// 	{
// 		status.setNewBestSolution(ALNS_Iteration_Status::FALSE);
// 		nbIterationsWithoutImprovement++;
// 		status.setNbIterationWithoutImprovement(nbIterationsWithoutImprovement);
// 		status.setNbIterationWithoutImprovementSinceLastReload(status.getNbIterationWithoutImprovementSinceLastReload()+1);
// 		return false;
// 	}
// }

// //利用接受准则判断是否要接受当前的解作为新的解。
// bool ALNS::transitionCurrentSolution(ISolution* newSol)
// {
// 	//如果接受。
// 	if(acceptanceCriterion->transitionAccepted(*bestSolManager,*currentSolution,*newSol,status))
// 	{
// 		delete currentSolution;
// 		currentSolution = newSol->getCopy();
// 		return true;
// 	}
// 	//不接受，原来解不变，什么也不做。
// 	else
// 	{
// 		return false;
// 	}
// }

// //判断算法迭代是否遇到终止条件
// bool ALNS::isStoppingCriterionMet()
// {
// 	//是否找到最优可行解。
// 	if((*(bestSolManager->begin()))->isFeasible() && (*(bestSolManager->begin()))->getObjectiveValue() == lowerBound)
// 	{
// 		return true;
// 	}
// 	else
// 	{
// 		switch(param->getStopCrit())
// 		{
// 			//是否达到最大迭代次数。
// 			case ALNS_Parameters::MAX_IT: {
// 				return nbIterations >= param->getMaxNbIterations();
// 			}
// 			//是否达到最大限制运行时间。
// 			case ALNS_Parameters::MAX_RT: {
// 				clock_t currentTime = clock();
// 				double elapsed = (static_cast<double>(currentTime - startingTime)) / CLOCKS_PER_SEC;
// 				return elapsed >= param->getMaxRunningTime();
// 			}
// 			//the maximum number of iterations without improvement. 
// 			case ALNS_Parameters::MAX_IT_NO_IMP: {
// 				return nbIterationsWithoutImprovement >= param->getMaxNbIterationsNoImp();
// 			}
// 			//a mix of the MAX_IT, MAX_RT and MAX_IT_NO_IMP.
// 			case ALNS_Parameters::ALL: {
// 				if(nbIterations >= param->getMaxNbIterations())
// 				{
// 					return true;
// 				}
// 				if(nbIterationsWithoutImprovement >= param->getMaxNbIterationsNoImp())
// 				{
// 					return true;
// 				}
// 				clock_t currentTime = clock();
// 				double elapsed = (static_cast<double>(currentTime - startingTime)) / CLOCKS_PER_SEC;
// 				if(elapsed >= param->getMaxRunningTime())
// 				{
// 					return true;
// 				}
// 				return false;
// 			}

// 			default: {
// 				assert(false);
// 				return false;
// 			}
// 		}
// 	}

// }

// void ALNS::end()
// {
// 	opManager->end();
// 	delete opManager;
// 	delete acceptanceCriterion;
// 	delete lsManager;
// 	delete bestSolManager;
// }
