// InitFeasSol.h

#ifndef _INSERTION_HEURISTIC_H_    //include guards
#define _INSERTION_HEURISTIC_H_

#include <vector>
#include <string>
#include "HeuristicFramework.h"
using namespace std;


class InitFeasSol: public HeuristicFramework
{
    private: 
        vector<int> cus_id_set;
        vector<int> veh_id_set;
        //vector<OneCus> new_compact_route;

        // vector<int> build_new_compact_route(vector<int> &remain_cus_id, vector<int> &remain_vehs_id, int &route_veh_id, int &route_veh_type); //build a new route with randomly selected node from the set of uninserted customers
        Route build_new_compact_route(vector<int> &remain_cus_id, vector<int> &remain_vehs_id); //build a new route with randomly selected node from the set of uninserted customers
        void build_initial_sol(vector<int> &unserved_cus_id, vector<int> &unused_vehs_id); //create an initial solution for all customers

    public:
        InitFeasSol(Nodes &nodes, Vehicles &vehs);
        void run_algorithm(); //run the heuristic and record the cpu time
};

#endif