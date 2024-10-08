#ifndef NODE_SHAW_REMOVAL_H_
#define NODE_SHAW_REMOVAL_H_

#include <vector>
#include "example/operators/nodeDestroyOps/Node_Random_Removal.h"
#include <tuple>
using namespace std;

class ISolution;
class ALNS_Iteration_Status;
class VRPSolution;
class Nodes;
class Operators_Parameters;
class Node_Random_Removal;

class Node_Shaw_Removal: public Node_Random_Removal
{
    public:
        //! constructor
        Node_Shaw_Removal(string s, Operators_Parameters& ops_param, Nodes& nodes);

        //! destructor
        virtual ~Node_Shaw_Removal();

        //! the destory operator
        void destroySolNode(ISolution& sol);

    private:
        //! the reference to the set of nodes
        Nodes& nodeset;

        //! relatedness parameters
        int param1, param2, param3, param4;

        //! randomizinng parameters
        double randShaw;
        
        //! calculate the relatedness measure between node i and node j
        double calRelatedness(VRPSolution& vrpsol, pair<int, int> rid_arcpos1, pair<int, int> rid_arcpos2);

        //! maximum time difference
        int maxTimeDiff;

        //! maximum distance difference
        double maxDistDiff;

        //! maximum demand difference
        int maxDmdDiff;

        //! calculate the maximum time diff
        int calMaxTimeDiff(VRPSolution& vrpsol);

        //! calculate the maximum distance difference
        double calMaxDistDiff(Nodes& nodeset);

        //! calculate the maximum demand difference
        int calMaxDmdDiff(Nodes& nodeset);
        
        //! normalization
        void normalize(double& dist, int& dmd_diff, int& arrtime_diff, VRPSolution &vrpsol);
};

#endif
