#ifndef OPERATORS_PARAMETERS_H_
#define OPERATORS_PARAMETERS_H_

#include <assert.h>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

/// @brief this class contains parameters to be used by the operators.
class Operators_Parameters
{
    public:

        //! Constructor.
        Operators_Parameters();

        //! Copy constructor.
        Operators_Parameters(Operators_Parameters& p);

        //! Destructor.
        ~Operators_Parameters();

        //! This method perform some sanity checks on the value of the parameters.
        void sanityChecks();

        //! Load the parameters of a file.
        //! \param path the path to the file containing the parameters.
        void loadParameters(string path);

        //! Load the parameters of a xml file.
        //! \param path the path to the xml file containing the parameters.
        void loadXMLParameters(string path);

        // int getMaxDestroyPerc() const {
        //     return maxDestroyPerc;
        // }

        // void setMaxDestroyPerc(int maxDestroyPerc) {
        //     this->maxDestroyPerc = maxDestroyPerc;
        // }

        // int getMinDestroyPerc() const {
        //     return minDestroyPerc;
        // }

        // void setMinDestroyPerc(int minDestroyPerc) {
        //     this->minDestroyPerc = minDestroyPerc;
        // }

        //! a simple getter
        double getNodeDestroySizeRate() {
            return nodeDestroyRate;
        }

        //! a simple getter
        double getPathDestroySizeRate() {
            return pathDestroyRate;
        }

        //! Simple setter.
        void setPathDestroyRate(double rate1) {
            this->pathDestroyRate = rate1;
        }

        //! Simple setter.
        void setNodeDestroyRate(double rate2) {
            this->nodeDestroyRate = rate2;
        }

        //! a simple getter
        double getRandShawParam() {
            return randShawDes;
        }

        //! a simple setter
        void setRandShawParam(double randShaw) {
            this->randShawDes = randShaw;
        }

        //! a simple getter
        double getShawRate1() {
            return shawRate1;
        }

        //! a simple getter
        double getShawRate2() {
            return shawRate2;
        }

        //! a simple getter
        double getShawRate3() {
            return shawRate3;
        }

        //! a simple getter
        double getShawRate4() {
            return shawRate4;
        }

        //! a simple setter
        void setShawRate1(double rate1) {
            this->shawRate1 = rate1;
        }
                
        //! a simple setter
        void setShawRate2(double rate2) {
            this->shawRate2 = rate2;
        }
                
        //! a simple setter
        void setShawRate3(double rate3) {
            this->shawRate3 = rate3;
        }
                
        //! a simple setter
        void setShawRate4(double rate4) {
            this->shawRate4 = rate4;
        }

        //! a simple getter
        double getRandWorstParam() {
            return randWorstDes;
        }

        //! a simple setter
        void setRandWorstParam(double randWorst) {
            this->randWorstDes = randWorst;
        }

        //! a simple getter
        double getRandAvgLenDiffDes() {
            return randAvgLenDiffDes;
        }

        //! a simple setter
        void setRandAvgLenDiffDes(double randALD) {
            this->randAvgLenDiffDes = randALD;
        }

        //! a simple getter
        double getRandAvgLenDes() {
            return randAvgLenDes;
        }

        //! a simple setter
        void setRandAvgLenDes(double randAL) {
            this->randAvgLenDes = randAL;
        }

        //! a simple getter
        double getRandCardiDes() {
            return randCardiDes;
        }

        //! a simple setter
        void setRandCardiDes (double randCardi) {
            this->randCardiDes = randCardi;
        }

        //! a simple getter
        double getRandNfreqDes() {
            return randNfreqDes;
        }

        //! a simple setter
        void setRandNfeqDes(double randNFreqDes) {
            this->randNfreqDes = randNFreqDes;
        }

        //! a simple getter
        double getRandKLenRep() {
            return randKlenRep;
        }

        //! a simple setter
        void setRandKLenRep(double randKLen) {
            this->randKlenRep = randKLen;
        }

        //! a simple getter
        double getRandNfreqRep() {
            return randNfreqRep;
        }

        //! a simple setter
        void setRandNfreqRep(double randNFreqRep) {
            this->randNfreqRep = randNfreqRep;
        }

        //! a simple getter
        int getTabuTenure() {
            return tenure;
        }

        //! a simple setter
        void setTabuTenure(double tenure) {
            this->tenure = tenure;
        }

    protected:
        //! control the maixmum size of destroyed non-tabu paths
        double pathDestroyRate;

        //! control the maximum node destroy size
        double nodeDestroyRate;

        //! relatedness parameter in the Node Shaw removal in terms of the distance
        double shawRate1;

        //! relatedness parameter in the Node Shaw removal in terms of the demands
        double shawRate2;

        //! relatedness parameter in the Node Shaw removal in terms of the arrival time
        double shawRate3;

        //! relatedness parameter in the Node Shaw removal in terms of the customer type
        double shawRate4;

        //! randomizing selection parameter for shaw removal
        double randShawDes;

        //! randomizing selection parameter for the worst removal
        double randWorstDes;

        //! randomizing selection parameter for the AvgLenDiff path destroy 
        double randAvgLenDiffDes;

        //! randomizing selection parameter for the AvgLen path destroy 
        double randAvgLenDes;

        //! randomizing selection parameter for the Cardinality path destroy
        double randCardiDes;

        //! randomizing selection parameter for the NodeFreq path destroy
        double randNfreqDes;

        //! randomizing selection parameter for the KMaxLen path insert
        double randKlenRep;

        //! randomizing selection parameter for the NodeFreq path insert
        double randNfreqRep;

        //! tabu tenure in path destroy operators
        int tenure;

        //! The minimum percentage of the solution destroyed by the destroy operators.
        // int minDestroyPerc;

        //! The maximum percentage of the solution destroyed by the destroy operators.
        // int maxDestroyPerc;
};


#endif
