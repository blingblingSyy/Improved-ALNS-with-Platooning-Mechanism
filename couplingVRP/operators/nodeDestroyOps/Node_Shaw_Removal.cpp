#include <vector>
#include <numeric>
#include "src/ALNS_inc.h"
#include "utility.h"
#include "couplingVRP/operators/nodeDestroyOps/Node_Shaw_Removal.h"
#include "couplingVRP/model/establish/VRPSolution.h"
#include "couplingVRP/model/establish/ARoute.h"
#include "couplingVRP/model/basic/Nodes.h"
using namespace std;

Node_Shaw_Removal::Node_Shaw_Removal(string s, Nodes& nodes, ALNS_Parameters& alns_param) : 
                    nodeset(nodes), ANodeDestroyOperator(s, alns_param.getNodeDestroyRate(), nodeset.getCusNum())
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
    this->param1 = alns_param.getShawRate1();
    this->param2 = alns_param.getShawRate2();
    this->param3 = alns_param.getShawRate3();
    this->param4 = alns_param.getShawRate4();
    this->randShaw = alns_param.getRandShawParam();
}

Node_Shaw_Removal::~Node_Shaw_Removal()
{

}

void Node_Shaw_Removal::destroySolNode(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    int orig_noninserted = vrpsol.getNonInsertedNodes().size();
    calNodeDestroySize(vrpsol.getTotalServedCusNum()); //! get the nodeDestroySize
    vector<pair<int, int>> all_cus_pos = vrpsol.getAllCusPos();
    // vector<pair<int, int>> destroyed_nodeset;
    RandomNumber r1;
    int select_pos = r1.get_rint(0, all_cus_pos.size()-1);
    destroyed_nodeset.push_back(all_cus_pos[select_pos]);
    all_cus_pos.erase(all_cus_pos.begin() + select_pos);
    int i = 1;
    while(i < nodeDestroySize)
    {
        RandomNumber r2;
        int pos = r2.get_rint(0, destroyed_nodeset.size()-1);
        pair<int, int> selected_request = destroyed_nodeset[pos];
        vector<double> relatednessVec;
        for(auto it = all_cus_pos.begin(); it != all_cus_pos.end(); it++)
        {
            relatednessVec.push_back(calRelatedness(vrpsol, selected_request, *it));
        }
        vector<int> indices(relatednessVec.size());
        iota(indices.begin(), indices.end(), 0);
        sort(indices.begin(), indices.end(), [&](int A, int B) -> bool {return relatednessVec[A] < relatednessVec[B];});
        RandomNumber r3;
        int new_select = int(pow(r3.get_rflt(0,1), randShaw) * (indices.size()-1));
        destroyed_nodeset.push_back(all_cus_pos[indices[new_select]]);
        all_cus_pos.erase(all_cus_pos.begin() + indices[new_select]);
        i++;
    }
    for(int k = 0; k < destroyed_nodeset.size(); k++)
    {
        vrpsol.removeNode(destroyed_nodeset[k].second, destroyed_nodeset[k].first);
    }
    // if(orig_noninserted - vrpsol.getNonInsertedNodes().size() == 0) //! no nodes are actually removed
    // {
    //     setEmpty(true);
    //     setToSelectNext(false);
    // }
}

double Node_Shaw_Removal::calRelatedness(VRPSolution& vrpsol, pair<int, int> rid_arcpos1, pair<int, int> rid_arcpos2)
{
    int node1 = vrpsol.getOneRoute(rid_arcpos1.first)->getCompactRoute()[rid_arcpos1.second];
    int node2 = vrpsol.getOneRoute(rid_arcpos2.first)->getCompactRoute()[rid_arcpos2.second];
    double dist = nodeset.getSPdist()[node1][node2];
    int dmd_diff = abs(nodeset.getDemands(node1) - nodeset.getDemands(node2));
    int time_diff = abs(vrpsol.getOneRoute(rid_arcpos1.first)->getFinalArrTime()[rid_arcpos1.second] - vrpsol.getOneRoute(rid_arcpos2.first)->getFinalArrTime()[rid_arcpos2.second]);
    bool isSameType = nodeset.getNodeType(node1) == nodeset.getNodeType(node2);
    normalize(dist, dmd_diff, time_diff, vrpsol);
    return param1*dist + param2*dmd_diff + param3*time_diff - param4*isSameType;
}

int Node_Shaw_Removal::calMaxTimeDiff(VRPSolution& vrpsol)
{
    vector<int> maxArrTimeVec, minArrTimeVec;
    for(int i = 0; i < vrpsol.getRoutesNum(); i++)
    {
        ARoute* route_i = vrpsol.getOneRoute(i);
        maxArrTimeVec.push_back(route_i->getFinalArrTime()[route_i->getCompactRoute().size()-2]);
        minArrTimeVec.push_back(route_i->getFinalArrTime()[1]);
    }
    auto max_it = max_element(maxArrTimeVec.begin(), maxArrTimeVec.end());
    auto min_it = min_element(minArrTimeVec.begin(), minArrTimeVec.end());
    return *max_it - *min_it;
}

void Node_Shaw_Removal::normalize(double& dist, int& dmd_diff, int& arrtime_diff, VRPSolution &vrpsol)
{
    dist = dist / nodeset.getMaxSPDist();
    dmd_diff = double(dmd_diff) / (nodeset.getMaxDmd() - nodeset.getMinDmd());
    arrtime_diff = double(arrtime_diff) / calMaxTimeDiff(vrpsol);
}