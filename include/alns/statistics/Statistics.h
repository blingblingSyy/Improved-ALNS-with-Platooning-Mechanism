#ifndef STATISTICS_H_
#define STATISTICS_H_

#include <vector>
#include <time.h>
using namespace std;

/// @brief a class to record the solutions' and operators' statistics over all iterations
class Statistics {
	public:
		//! Constructor.
		Statistics(){};

		//! Destructor.
		virtual ~Statistics();

		//! This method adds an entry to the data
		void addEntry(double timeStamp,
					size_t iteration,
					string strategyName,
					string nodeDestroyName,
					string nodeRepairName,
					string pathDestroyName,
					string pathRepairName,
					double newCost,
					double currentCost,
					double bestCost,
					int cumKS);

		void addOperatorEntry(vector<double>* weight,
								vector<size_t>* calls);

		void addOperatorsNames(vector<string>* names){operatorNames = names;};

		//! This method generate the file containing the datas.
		void generateStatsFile(string path, string pathOp);

		void setStart(){start = clock();};

		double getOneCurCost(int NbIter) {return currentCosts[NbIter];};

		double getOneBestCost(int NbIter) {return bestCosts[NbIter];};

	private:
		vector<double> timeStamps;
		vector<size_t> iterations;
		vector<string> strategyNames;
		vector<string> nodeDestroyNames;
		vector<string> nodeRepairNames;
		vector<string> pathDestroyNames;
		vector<string> pathRepairNames;
		vector<double> newCosts;
		vector<double> currentCosts;
		vector<double> bestCosts;
		vector<int> cumulativeKnownSolutions;
		vector<vector<double>*> weights;
		vector<vector<size_t>*> nbCalls;
		vector<double> timeStampsOperators;
		vector<string>* operatorNames;
		clock_t start;

};

#endif /* STATISTICS_H_ */
