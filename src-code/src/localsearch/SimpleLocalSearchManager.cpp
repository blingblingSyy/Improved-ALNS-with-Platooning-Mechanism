#include "src/localsearch/SimpleLocalSearchManager.h"
#include "src/improvedALNS/ALNS_Iteration_Status.h"
#include "src/improvedALNS/ALNS_Parameters.h"
#include "src/improvedALNS/ISolution.h"
#include "src/localsearch/ILocalSearch.h"
#include <algorithm>
#include <vector>
using namespace std;

bool SimpleLocalSearchManager::useLocalSearch(ISolution& sol, ALNS_Iteration_Status& status)
{
	if(status.getNewBestSolution() != ALNS_Iteration_Status::TRUE  //! case 1: when the new solution is not the new best solution
		|| status.getAcceptedAsCurrentSolution() != ALNS_Iteration_Status::UNKNOWN)  //! case 2: when the new solution has been compared with the current solution
	{
		return false;
	}
	else  //! (status.getNewBestSolution() == ALNS_Iteration_Status::TRUE) && (status.getAcceptedAsCurrentSolution() == ALNS_Iteration_Status::UNKNOWN)
	{
		status.setLocalSearchUsed(ALNS_Iteration_Status::TRUE);
		bool improvement;
		do //! until the solution cannot be improved by any local search operators any more
		{
			improvement = false;
			for(size_t i = 0; i < localSearchOperators.size(); i++)
			{
				improvement = localSearchOperators[i]->performLocalSearch(sol)||improvement;  //! cycle when at least one local search operator improves the solution 
			}
		} while(improvement); //! at least perform once the procedure in "do...while..."
		if(improvement)
		{
			status.setImproveByLocalSearch(ALNS_Iteration_Status::TRUE);
			return true;
		}
		else
		{
			status.setImproveByLocalSearch(ALNS_Iteration_Status::FALSE);
			return false;
		}
	}
}

void SimpleLocalSearchManager::addLocalSearchOperator(ILocalSearch& ls)
{
	auto it = find(param->getForbidenLsOperators().begin(), param->getForbidenLsOperators().end(), ls.getName());
	if(it == param->getForbidenLsOperators().end())
	{
		localSearchOperators.push_back(&ls);
	}
	else
	{
		cout << "NO " << ls.getName() << endl;
	}
	// bool ok = true;
	// for(size_t i=0; i < param->getForbidenLsOperators().size() && ok; i++)
	// {
	// 	if(param->getForbidenLsOperators()[i] == ls.getName())
	// 	{
	// 		std::cout << "NO " << ls.getName() << std::endl;
	// 		ok = false;
	// 	}
	// }
	// if(ok)
	// {
	// 	localSearchOperators.push_back(&ls);
	// }
}
