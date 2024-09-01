#ifndef ALNS_PARAMETERS_H_
#define ALNS_PARAMETERS_H_

#include <assert.h>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

/// @brief this class contains parameters to be used by the ALNS.
class ALNS_Parameters
{
    public:

        //! Enumeration representing the various kind of stopping criteria.
        //! MAX_IT: the maximum number of iterations.
        //! MAX_RT: the maximum run time.
        //! MAX_IT_NO_IMP: the maximum number of iterations without improvement.
        //! ALL: a mix of the MAX_IT, MAX_RT and MAX_IT_NO_IMP.
        enum StoppingCriteria
        {
            MAX_IT,
            MAX_RT,
            MAX_IT_NO_IMP,
            ALL
        };

        //! An enumeration listing a set of packaged AcceptanceModule Implementation.
        enum AcceptanceCriterioKind
        {
            SA,
            DM
        };


        //! Constructor.
        ALNS_Parameters();

        //! Copy constructor.
        ALNS_Parameters(ALNS_Parameters& p);

        //! Destructor.
        ~ALNS_Parameters();

        //! This method perform some sanity checks on the value of the parameters.
        void sanityChecks();

        //! Load the parameters of a file.
        //! \param path the path to the file containing the parameters.
        void loadParameters(string path);

        //! Load the parameters of a xml file.
        //! \param path the path to the xml file containing the parameters.
        void loadXMLParameters(string path);

        //! Simple getter.
        size_t getReloadFrequency() const
        {
            return reloadFrequency;
        }

        void setReloadFrequency(size_t reloadFrequency)
        {
            assert(!lock);
            this->reloadFrequency = reloadFrequency;
        }

        //! Simple getter.
        vector<string> getForbidenLsOperators() const
        {
            return forbidenLsOperators;
        }

        void addForbiddenLsOperator(string lsOp)  //! Ls: local search
        {
            forbidenLsOperators.push_back(lsOp);
        }

        //! Simple getter.
        vector<string> getForbidenOperators() const
        {
            return forbidenOperators;
        }

        void addForbiddenOperator(string op)
        {
            forbidenOperators.push_back(op);
        }

        //! Simple getter.
        bool getPerformLocalSearch() const
        {
            return performLocalSearch;
        }

        void setPerformLocalSearch(bool performLocalSearch)
        {
            assert(!lock);
            this->performLocalSearch = performLocalSearch;
        }

        //! Simple getter.
        int getLogFrequency() const
        {
            return logFrequency;
        }

        void setLogFrequency(int logFrequency)
        {
            assert(!lock);
            this->logFrequency = logFrequency;
        }

        //! Simple getter.
        string getStatsGlobPath() const  //! record the changes of solutions for each iteration
        {
            return statsGlobPath;
        }

        //! Simple getter.
        string getStatsOpPath() const  //! record the weights of operations for each iteration
        {
            return statsOpPath;
        }

        void setStatsGlobPath(std::string statsGlobPath)
        {
            this->statsGlobPath = statsGlobPath;
        }

        void setStatsOpPath(std::string statsOpPath)
        {
            this->statsOpPath = statsOpPath;
        }

        //! Simple getter.
        AcceptanceCriterioKind getAcKind() const
        {
            return acKind;
        }

        //! Simple getter.
        // string getAcPath() const
        // {
        //     return acPath;
        // }

        //! Simple getter.
        void setAcKind(AcceptanceCriterioKind acKind)
        {
            assert(!lock);
            this->acKind = acKind;
        }

        //! Simple getter.
        // void setAcPath(string acPath)
        // {
        //     assert(!lock);
        //     this->acPath = acPath;
        // }

        //! Simple getter.
        double getProbabilityOfNoise() const
        {
            return probabilityOfNoise;
        }

        //! Simple getter.
        void setProbabilityOfNoise(double probabilityOfNoise)
        {
            assert(!lock);
            this->probabilityOfNoise = probabilityOfNoise;
        }

        //! Simple getter.
        size_t getMaxNbIterations() const
        {
            return maxNbIterations;
        }

        //! Simple setter.
        void setMaxNbIterations(size_t maxNbIterations)
        {
            assert(!lock);
            this->maxNbIterations = maxNbIterations;
        }

        //! Simple getter.
        size_t getMinNbIterations() const
        {
            return minNbIterations;
        }

        //! Simple setter.
        void setMinNbIterations(size_t minNbIterations)
        {
            assert(!lock);
            this->minNbIterations = minNbIterations;
        }

        //! Simple getter.
        size_t getMaxNbIterationsNoImp() const
        {
            return maxNbIterationsNoImp;
        }

        //! Simple setter.
        void setMaxNbIterationsNoImp(size_t maxNbIterationsNoImp)
        {
            assert(!lock);
            this->maxNbIterationsNoImp = maxNbIterationsNoImp;
        }

        //! Simple getter.
        double getMaxRunningTime() const
        {
            return maxRunningTime;
        }

        //! Simple setter.
        void setMaxRunningTime(double maxRunningTime)
        {
            assert(!lock);
            this->maxRunningTime = maxRunningTime;
        }

        //! Simple getter.
        double getMaximumWeight() const
        {
            return maximumWeight;
        }

        //! Simple setter.
        void setMaximumWeight(double maximumWeight)
        {
            assert(!lock);
            this->maximumWeight = maximumWeight;
        }

        //! Simple getter.
        double getMinimumWeight() const
        {
            return minimumWeight;
        }

        //! Simple setter.
        void setMinimumWeight(double minimumWeight)
        {
            assert(!lock);
            this->minimumWeight = minimumWeight;
        }

        //! Simple getter.
        // size_t getNbItBeforeReinit() const
        // {
        //     return nbItBeforeReinit;
        // }

        //! Simple setter.
        // void setNbItBeforeReinit(size_t nbItBeforeReinit)
        // {
        //     assert(!lock);
        //     this->nbItBeforeReinit = nbItBeforeReinit;
        // }

        //! Simple getter.
        bool getGlobalNoise() const
        {
            return globalNoise;
        }

        //! Simple setter.
        void setGlobalNoise(bool noise)
        {
            assert(!lock);
            this->globalNoise = noise;
        }

        //! Simple getter.
        double getRho() const
        {
            return rho;
        }

        //! Simple setter.
        void setRho(double rho)  //! for weight adaptive adjustment
        {
            assert(!lock);
            this->rho = rho;
        }

        //! Simple getter.
        int getSigma1() const   //! score
        {
            return sigma1;
        }

        //! Simple setter.
        void setSigma1(int sigma1)
        {
            assert(!lock);
            this->sigma1 = sigma1;
        }

        //! Simple getter.
        int getSigma2() const
        {
            return sigma2;
        }

        //! Simple setter.
        void setSigma2(int sigma2)
        {
            assert(!lock);
            this->sigma2 = sigma2;
        }

        //! Simple getter.
        int getSigma3() const
        {
            return sigma3;
        }

        //! Simple setter.
        void setSigma3(int sigma3)
        {
            assert(!lock);
            this->sigma3 = sigma3;
        }

        //! Simple getter.
        StoppingCriteria getStopCrit() const
        {
            return stopCrit;
        }

        //! Simple setter.
        void setStopCrit(StoppingCriteria stopCrit)
        {
            assert(!lock);
            this->stopCrit = stopCrit;
        }

        //! Simple getter.
        size_t getTimeSegmentsIt() const
        {
            return timeSegmentsIt;
        }

        //! Simple setter.
        void setTimeSegmentsIt(size_t timeSegmentsIt)
        {
            assert(!lock);
            this->timeSegmentsIt = timeSegmentsIt;
        }

        //! Simple setter.
        void setLock()
        {
            lock = true;
        }

        //! a simple getter
        size_t getLookBackIterations() {
            return lookBackIterations;
        }

        //! a simple setter
        void setLookBackIterations(size_t nbIterations) {
            assert(!lock);
            this->lookBackIterations = nbIterations;
        }

        //! a simple getter
        size_t getObjImpThreshold() {
            return objImpThreshold;
        }

        //! a simple setter
        void setObjImpThreshold(size_t threshold) {
            assert(!lock);
            this->objImpThreshold = threshold;
        }
    protected:
        //! Maximum number of iterations performed by the ALNS.
        size_t maxNbIterations;

        //! Minimum number of iterations that must be performed by the ALNS.
        //! for the stopping criteria: MAX_IT_NO_IMP
        size_t minNbIterations;

        //! iterations look-back -> used to calculate the gap of solution costs between two segment of look-back iterations
        size_t lookBackIterations;

        //! the objective improvement threshold
        double objImpThreshold;

        //! Maximum running time of the ALNS.
        double maxRunningTime;

        //! Maximum number of iterations without any improvement.
        size_t maxNbIterationsNoImp;

        //! Which stopping criterion should be used.
        StoppingCriteria stopCrit;

        //! Indicate if noise should be used for the whole algorithm.
        bool globalNoise;

        //! Indicate after how many iterations should the scores of
        //! the operators be recomputed.
        size_t timeSegmentsIt;

        //! Indicate the number of iterations that should be performed
        //! before reinitialization of the scores of the operators.  (accumulative score per segment -> reset)
        // size_t nbItBeforeReinit;

        //! score adjustment parameter in case the last remove-insert
        //! operation resulted in a new global best solution
        int sigma1;

        //! score adjustment parameter in case that the last remove-insert
        //! operation resulted in a solution that has not been accepted before and
        //! the objective value is better than the objective value of current solution
        int sigma2;

        //! score adjustment parameter in case that the last remove-insert
        //! operation resulted in a solution that has not been accepted before and such
        //! that the score objective value is worse than the one of current solution but
        //! the solution was accepted.
        int sigma3;

        //! reaction factor 0 <= rho <= 1 for the update of the weights of the
        //! operators.
        double rho;

        //! The minimum possible weight for an operator.
        double minimumWeight;

        //! The maximum possible weight for an operator.
        double maximumWeight;

        //! Indicates the probability of using noised operators.
        double probabilityOfNoise;

        //! Kind of acceptance criterion used. ->SA
        AcceptanceCriterioKind acKind;

        //! path to the configuration file of the acceptance criterion.
        // string acPath;

        //! path to the file where the global stats have to be saved.
        string statsGlobPath;

        //! path to the file where the operators stats have to be saved.
        string statsOpPath;

        //! Indicate every each iteration logging is done.
        int logFrequency;

        //! A set of forbidden operators.
        vector<string> forbidenOperators;

        //! A set of forbidden local search operators.
        vector<string> forbidenLsOperators;

        //! Indicate after how many iterations without improvement
        //! does the best known solution is reloaded.
        size_t reloadFrequency;

        //! Indicate if local search should be used.
        bool performLocalSearch;

        //! When the optimization process start, the parameters
        //! should not be modified. lock is set to true when the
        //! optimization begin. If the setter of the value
        //! of one parameter is called while lock is true, an
        //! error is raised.
        bool lock;

};


#endif
