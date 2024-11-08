#include <vector>
#include <numeric>
#include "alns/ALNS_inc.h"
#include "utility.h"
#include "example/operators/nodeDestroyOps/Node_Shaw_Removal.h"
#include "example/model/establish/VRPSolution.h"
#include "example/model/establish/ARoute.h"
#include "example/model/basic/Nodes.h"
#include "example/operators/NodeDestroyOps/Node_Random_Removal.h"
#include "example/operators/Operators_Parameters.h"
using namespace std;

Node_Shaw_Removal::Node_Shaw_Removal(string s, Operators_Parameters& ops_param, Nodes& nodes) : 
                    nodeset(nodes), Node_Random_Removal(s, ops_param, nodes.getCusNum())
{
    empty = false;
    // hasSelectedCur = true;
    toSelectNext = true;
    this->param1 = ops_param.getShawRate1();
    this->param2 = ops_param.getShawRate2();
    this->param3 = ops_param.getShawRate3();
    this->param4 = ops_param.getShawRate4();
    this->randShaw = ops_param.getRandShawParam();
    maxDistDiff = calMaxDistDiff(nodeset);
    maxDmdDiff = calMaxDmdDiff(nodeset);
}

Node_Shaw_Removal::~Node_Shaw_Removal()
{

}

void Node_Shaw_Removal::destroySolNode(ISolution& sol)
{
    VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
    //! prepare for normalization
    maxTimeDiff = calMaxTimeDiff(vrpsol);
    calNodeDestroySize(vrpsol.getTotalServedCusNum()); //! get the nodeDestroySize
    vector<pair<int, int>> all_cus_pos = vrpsol.getAllCusPos();
    keepRemovablePos(vrpsol, all_cus_pos); //! this will modify all_cus_pos by removing all the non-repairable positions from all_cus_pos
    RandomNumber r1;
    int select_pos = r1.get_rint(0, all_cus_pos.size()-1);
    destroyed_nodeset.push_back(all_cus_pos[select_pos]);
    all_cus_pos.erase(all_cus_pos.begin() + select_pos);
    int i = 0;
    while(i < nodeDestroySize && !all_cus_pos.empty())  //! destroyed_nodeset.size() < nodeDestroySize
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
    sort(destroyed_nodeset.begin(), destroyed_nodeset.end(), [&](pair<int, int> A, pair<int, int> B) {return A > B;});
    for(int k = 0; k < destroyed_nodeset.size(); k++)
    {
        vrpsol.removeNode(destroyed_nodeset[k].second, destroyed_nodeset[k].first);
    }
    destroyed_nodeset.clear();
    // if(destroyed_nodeset.empty()) //! no nodes are actually removed
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
    int dmd_diff = abs(nodeset.getNodeDemand(node1) - nodeset.getNodeDemand(node2));
    int time_diff = abs(vrpsol.getOneRoute(rid_arcpos1.first)->getFinalArrTime()[rid_arcpos1.second] - vrpsol.getOneRoute(rid_arcpos2.first)->getFinalArrTime()[rid_arcpos2.second]);
    bool isSameType = nodeset.getNodeType(node1) == nodeset.getNodeType(node2);
    normalize(dist, dmd_diff, time_diff, vrpsol);
    return param1*dist + param2*dmd_diff + param3*time_diff - param4*isSameType;
}

void Node_Shaw_Removal::normalize(double& dist, int& dmd_diff, int& arrtime_diff, VRPSolution &vrpsol)
{
    dist = dist / maxDistDiff;
    dmd_diff = double(dmd_diff) / maxDmdDiff;
    arrtime_diff = double(arrtime_diff) / maxTimeDiff;
}

int Node_Shaw_Removal::calMaxTimeDiff(VRPSolution& vrpsol)
{
    vector<int> maxArrTimeVec, minArrTimeVec;
    for(int i = 0; i < vrpsol.getRoutesNum(); i++)
    {
        ARoute* route_i = vrpsol.getOneRoute(i);
        if(route_i->getCompactRoute().size() > 2)
        {
            maxArrTimeVec.push_back(route_i->getNodeExpectedArrTW(route_i->getCompactRoute().size()-2)[1]);
            minArrTimeVec.push_back(route_i->getNodeExpectedArrTW(1)[0]);
        }
    }
    auto max_it = max_element(maxArrTimeVec.begin(), maxArrTimeVec.end());
    auto min_it = min_element(minArrTimeVec.begin(), minArrTimeVec.end());
    return max(*max_it - *min_it, 1);
}

double Node_Shaw_Removal::calMaxDistDiff(Nodes& nodeset)
{
    vector<double> spDistVec;
    for(int i = 0; i < nodeset.getNodeNum()-1; i++)
    {
        for(int j = i+1; j < nodeset.getNodeNum(); j++)
        {
            spDistVec.push_back(nodeset.getSPdist()[i][j]);
        }
    }
    auto maxit = max_element(spDistVec.begin(), spDistVec.end());
    auto minit = min_element(spDistVec.begin(), spDistVec.end());
    return *maxit - *minit;
}

int Node_Shaw_Removal::calMaxDmdDiff(Nodes& nodeset)
{
    vector<int> all_demands = nodeset.getAllDemands();
    auto maxit = max_element(all_demands.begin(), all_demands.end());
    auto minit = min_element(all_demands.begin(), all_demands.end());
    return *maxit - *minit;
}

/*old version*/
// void Node_Shaw_Removal::destroySolNode(ISolution& sol)
// {
//     VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//     int orig_noninserted = vrpsol.getNonInsertedNodes().size();
//     calNodeDestroySize(vrpsol.getTotalServedCusNum()); //! get the nodeDestroySize
//     vector<pair<int, int>> all_cus_pos = vrpsol.getAllCusPos();
//     // vector<pair<int, int>> destroyed_nodeset;
//     RandomNumber r1;
//     int select_pos = r1.get_rint(0, all_cus_pos.size()-1);
//     destroyed_nodeset.push_back(all_cus_pos[select_pos]);
//     all_cus_pos.erase(all_cus_pos.begin() + select_pos);
//     int i = 1;
//     while(i < nodeDestroySize)
//     {
//         RandomNumber r2;
//         int pos = r2.get_rint(0, destroyed_nodeset.size()-1);
//         pair<int, int> selected_request = destroyed_nodeset[pos];
//         vector<double> relatednessVec;
//         for(auto it = all_cus_pos.begin(); it != all_cus_pos.end(); it++)
//         {
//             relatednessVec.push_back(calRelatedness(vrpsol, selected_request, *it));
//         }
//         vector<int> indices(relatednessVec.size());
//         iota(indices.begin(), indices.end(), 0);
//         sort(indices.begin(), indices.end(), [&](int A, int B) -> bool {return relatednessVec[A] < relatednessVec[B];});
//         RandomNumber r3;
//         int new_select = int(pow(r3.get_rflt(0,1), randShaw) * (indices.size()-1));
//         destroyed_nodeset.push_back(all_cus_pos[indices[new_select]]);
//         all_cus_pos.erase(all_cus_pos.begin() + indices[new_select]);
//         i++;
//     }
//     for(int k = 0; k < destroyed_nodeset.size(); k++)
//     {
//         vrpsol.removeNode(destroyed_nodeset[k].second, destroyed_nodeset[k].first);
//     }
//     // if(orig_noninserted - vrpsol.getNonInsertedNodes().size() == 0) //! no nodes are actually removed
//     // {
//     //     setEmpty(true);
//     //     setToSelectNext(false);
//     // }
//     destroyed_arcpos = vrpsol.getDestroyedArcsPos();
// }


/*old version*/
// void Node_Shaw_Removal::update(ISolution& sol, ALNS_Iteration_Status& status)
// {
//     if(hasSelectedCur && needUpdate)
//     {
//         forbidden_destroyed_nodepos.clear();
//         VRPSolution& vrpsol = dynamic_cast<VRPSolution&>(sol);
//         vector<tuple<int, int, int>> destroyed_arcpos = vrpsol.getDestroyedArcsPos();
//         for(int i = 0; i < destroyed_arcpos.size(); i++)
//         {
//             pair<int, int> forbidden_pos1 = make_pair(get<2>(destroyed_arcpos[i]), get<0>(destroyed_arcpos[i]));
//             pair<int, int> forbidden_pos2 = make_pair(get<2>(destroyed_arcpos[i])+1, get<0>(destroyed_arcpos[i]));
//             forbidden_destroyed_nodepos.insert(forbidden_pos1);
//             forbidden_destroyed_nodepos.insert(forbidden_pos2);
//         }
//     }
// }