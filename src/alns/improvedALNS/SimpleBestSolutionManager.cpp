#include <list>
#include "alns/improvedALNS/SimpleBestSolutionManager.h"
#include "alns/improvedALNS/ISolution.h"
#include "alns/improvedALNS/ALNS_Iteration_Status.h"
#include "alns/improvedALNS/ALNS_Parameters.h"
using namespace std;

SimpleBestSolutionManager::SimpleBestSolutionManager(ALNS_Parameters& param)
{
	parameters = &param;
}

SimpleBestSolutionManager::~SimpleBestSolutionManager()
{
	for(list<ISolution*>::iterator it = bestSols.begin(); it != bestSols.end(); it++)
	{
		delete (*it);
	}
}

bool SimpleBestSolutionManager::isNewBestSolution(ISolution& sol)
{
	for(list<ISolution*>::iterator it = bestSols.begin(); it != bestSols.end(); it++) //it++
	{
		ISolution& currentSol = *(*it);
		if(currentSol < sol) //! not better
		{
			return false;
		}
		else if(sol < currentSol) //! the input sol is better than the current sol
		{
			delete *it;
			it = bestSols.erase(it); 
			if(it == bestSols.end())
			{
				break; //! jump out of the for cycle, and add the input sol to the list of bestSols
			}
			continue; //not execute it++ at the end of the current for cycle
		}
		else if(currentSol.getHash() == sol.getHash()) //! not better(==), not new
		{
			return false;
		} //! if not the same, compare the next and finally add the input sol to the list of bestSols
	}
	ISolution* copy = sol.getCopy();
	bestSols.push_back(copy); //! always put the better solution at the back of the list
	return true;
}

ISolution* SimpleBestSolutionManager::reloadBestSolution(ISolution* currSol, ALNS_Iteration_Status& status)
{
	//! if the solution is not improved for a period of time and reach the reload frequency, then reload the best solution as the current solution
	if(status.getNbIterationWithoutImprovementSinceLastReload() > 0 &&
	   ((status.getNbIterationWithoutImprovementSinceLastReload() % parameters->getReloadFrequency()) == 0))
	{
		status.setNbIterationWithoutImprovementSinceLastReload(0);
		delete currSol;
		return bestSols.back()->getCopy();
	}
	else
	{
		return currSol;
	}
}
