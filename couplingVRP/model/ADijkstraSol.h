#ifndef ADIJKSTRASOL_H_
#define ADIJKSTRASOL_H_

#include <vector>
using namespace std;

/// @brief a class to store a (K's) shortest path solution
class ADijkstraSol
{
    private:
        double KSP_Dist;
        vector<int> KSP_Path;

    public:
        ADijkstraSol() {KSP_Dist = 0; KSP_Path = {};}; 
        ~ADijkstraSol() {};
        
        //! a == comparator
        virtual bool operator ==(const ADijkstraSol& other) const;

        //! a < comparator
        virtual bool operator <(const ADijkstraSol& other) const;

        //! a simple getter
        double getDist() {return KSP_Dist;};
        vector<int> getPath() {return KSP_Path;};

        //! a simple setter
        void setDist(double inputDist) {KSP_Dist = inputDist;};
        void setPath(vector<int> inputPath) {KSP_Path = inputPath;};
};

#endif