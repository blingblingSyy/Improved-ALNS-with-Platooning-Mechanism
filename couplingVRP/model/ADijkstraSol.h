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
        ADijkstraSol(); 
        ~ADijkstraSol() {};
        
        //! a comparator
        virtual bool operator ==(const ADijkstraSol& other) const;

        //! a simple getter
        double getDist() {return KSP_Dist;};
        vector<int> getPath() {return KSP_Path;};
};

#endif