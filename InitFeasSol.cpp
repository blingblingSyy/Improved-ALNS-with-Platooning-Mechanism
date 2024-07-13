#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include <fstream>
#include <tuple>
#include "InitFeasSol.h"
#include "TimeWindowUpdater.h"
#include "utility.h"
#include "config.h"
using namespace std;  

//initialize the algorithm
InitFeasSol::InitFeasSol(Nodes &nodes, Vehicles &vehs): HeuristicFramework(nodes, vehs)
{
    for(int i = 0; i < nodeset.nodenum; i++)
    {
        cus_set.push_back(i);
    }
    cpu_time = 0;
}

//build a new route with randomly selected node from the set of uninserted customers -> mainly focus on whether vehicle type and customer type match
/* should consider the following aspects: 1. whether there are remaining customers or remaining vehicles; 
2. whether the newly inserted customers have the same customer type as the vehicle type; 
3. whether there are remaining vehicles of the same type as the first inserted customer type*/
vector<int> InitFeasSol::build_new_compact_route(vector<int> &remain_cus_id, vector<int> &remain_vehs_id, int &route_veh_id, int &route_veh_type) //remain_cus_type and remain_cus have equal size
{
    auto FailTrial = [&]() -> vector<int> {route_veh_id = -1; route_veh_type = -1; return {};};
    auto SuccTrial = [&](auto cus_iter, auto veh_iter) -> vector<int> 
    {
        vector<int> new_compact_route = {0, (*cus_iter), 0};
        route_veh_id = (*veh_iter);
        route_veh_type = nodeset.nodetype[(*cus_iter)];
        remain_cus_config.erase(cus_iter);
        remain_vehs_config.erase(veh_iter);
        return new_compact_route;
    };
    if(remain_cus_id.empty() || remain_vehs_id.empty()) //if any set is empty, return no new route
    {
        FailTrial();
    }
    else //if neither set is empty, do the following
    {
        //randomly pick a customer to be inserted
        RandomNumber r;
        int pick_pos = r.get_rint(0, static_cast<int>(remain_cus_id.size())-1);
        int cus_type = nodeset.nodetype[remain_cus_id[pick_pos]];
        //after picking the customer, check whether there are lefting vehicles of the same type as the customer type to be inserted
        vector<int>::iterator veh_it = find_if(remain_vehs_id.begin(), remain_vehs_id.end(), [&](int x) -> bool {return vehset.veh_type[x] == cus_type;});
        if(veh_it != remain_vehs_id.end())  //there are at least one vehicle of the same type as the chosen customer
        {
            auto cus_it = remain_cus_id.begin() + pick_pos;
            SuccTrial(cus_it, veh_it);
        }
        else //there are no lefting vehicles of the same type as the selected customer type -> change a customer to create a new route
        {
            vector<int>::iterator new_cus_it = find_if(remain_cus_id.begin(), remain_cus_id.end(), [&](int x) -> bool {return nodeset.nodetype[x] != cus_type;});
            if(new_cus_it != remain_cus_id.end())
            {
                auto new_veh_it = remain_vehs_id.begin();
                SuccTrial(new_cus_it, new_veh_it);
            }
            else //this means that the remaining customers and vehicles are incompatible
            {
                FailTrial();
            }
        }
    }
}


// vector<int> InitFeasSol::build_new_compact_route(vector<OneCus> &remain_cus_id, vector<OneVeh> &remain_vehs_id, int &route_id, int &route_type) //remain_cus_type and remain_cus have equal size
// {
//     auto FailTrial = [&]() -> vector<int> {route_id = -1; route_type = -1; return {};};
//     auto SuccTrial = [&](auto cus_iter, auto veh_iter) -> vector<int> 
//     {
//         vector<int> new_compact_route = {0, (*cus_iter).cus_id, 0};
//         route_id = (*veh_iter).veh_id;
//         route_type = (*cus_iter).cus_type;
//         remain_cus_config.erase(cus_iter);
//         remain_vehs_config.erase(veh_iter);
//         return new_compact_route;
//     };
//     if(remain_cus_config.empty() || remain_vehs_config.empty()) //if any set is empty, return no new route
//     {
//         FailTrial();
//     }
//     else //if neither set is empty, do the following
//     {
//         //randomly pick a customer to be inserted
//         RandomNumber r;
//         int pick_pos = r.get_rint(0, static_cast<int>(remain_cus_config.size())-1);
//         int cus_type = remain_cus_config[pick_pos].cus_type;
//         //after picking the customer, check whether there are lefting vehicles of the same type as the customer type to be inserted
//         vector<OneVeh>::iterator veh_it = find_if(remain_vehs_config.begin(), remain_vehs_config.end(), [&](OneVec x) -> bool {x.veh_type == cus_type;});
//         if(veh_it != remain_vehs_config.end())  //there are at least one vehicle of the same type as the chosen customer
//         {
//             auto cus_it = remain_cus_config.begin() + pick_pos;
//             SuccTrial(cus_it, veh_it);
//         }
//         else //there are no lefting vehicles of the same type as the selected customer type -> change a customer to create a new route
//         {
//             vector<OneCus>::iterator new_cus_it = find_if(remain_cus_config.begin(), remain_cus_config.end(), [&](OneVec x) -> bool {x.cus_type != cus_type;});
//             if(new_cus_it != remain_cus_config.end())
//             {
//                 auto new_veh_it = remain_vehs_config.begin();
//                 SuccTrial(new_cus_it, new_veh_it);
//             }
//             else //this means that the remaining customers and vehicles are incompatible
//             {
//                 FailTrial();
//             }
//         }
//     }
// }

//create an initial solution for all customers
void InitFeasSol::build_initial_sol(vector<int> &unserved_cus_id, vector<int> &unused_vehs_id)
{
    //unserved_cus = cus_set; unused_vehs = veh_set;
    while(!unserved_cus_id.empty() && !unused_vehs_id.empty())
    {
        int new_route_id, new_route_type;
        vector<int> new_route = build_new_compact_route(unserved_cus_id, unused_vehs_id, new_route_id, new_route_type);
        if(new_route_id > -1) 
        {
            TimeWindowUpdater tw_updater(vector<int> new_route, nodeset);
            int route_load = 0;
            double total_mile = 0;
        }
        else //in case that the remaining customers and vehicles are incompatible
        {
            cerr << "No initial solution can be built from the given set of customers and vehicles.";
            break;
        }
    }
    while(!unserved_set.empty())
    {
        vector<int> new_route = build_new_route(unserved_set);
        int route_load = 0;
        double total_mile = 0;
        //calculate the initial load and total mileage for the new route
        for(auto iter = new_route.begin(); iter != new_route.end(); iter++)
        {
            route_load += node_dmd[*iter];
            if(iter < new_route.end()-1)
            {
                total_mile += dist_mat[*iter][*(iter+1)];
            }
        }
        //for all uninserted customers, insert all possible customers in the current route (start with the customer with the least insertion cost)
        while(!unserved_set.empty()) //route_load <= cveh && total_mile <= mdist
        {
            vector<double> insert_costs_set;    //to store the cheapest insertion costs of all uninserted customers in the route
            vector<int> insert_pos_set;     //to store the cheapest insertion position of all uninserted customers in the route
            for(auto iter = unserved_set.begin(); iter != unserved_set.end(); iter++)
            {
                int cus = *iter;
                tuple<double, int> cus_insert = cal_cheapest_insert_cost(new_route, cus);
                insert_costs_set.push_back(get<0>(cus_insert));
                insert_pos_set.push_back(get<1>(cus_insert));
            }
            auto selected_cost_pos = min_element(insert_costs_set.begin(), insert_costs_set.end());
            int selected_cost_index = selected_cost_pos - insert_costs_set.begin();
            double selected_cost = *selected_cost_pos;  //the minimum insertion costs among all customers
            int selected_insert_pos = insert_pos_set[selected_cost_index];  //the minimum insertion position of the selected customer
            int selected_node = unserved_set[selected_cost_index];    //the customer with the minium insertion cost
            if(route_load + node_dmd[selected_node] <= cveh && total_mile + selected_cost <= mdist)
            {
                new_route.emplace(new_route.begin()+selected_insert_pos, selected_node);    //insert the customer into the route
                unserved_set.erase(unserved_set.begin() + selected_cost_index);     //remove the customer from the set of uninserted customers
                route_load += node_dmd[selected_node];  //update the vehicle load
                total_mile += selected_cost;    //update the total mileage
            }
            else
            {
                break;
            }
        }
        best_solution.push_back(new_route);
    }
}

void InitFeasSol::run_algorithm()
{
    double start = clock(); // get current time
    vector<int> uninserted_cus = cus_set;
    build_initial_sol(uninserted_cus);
    cpu_time = (clock() - start ) / (double) CLOCKS_PER_SEC;
}