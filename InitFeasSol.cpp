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
#include "HeuristicFramework.h"
#include "utility.h"
#include "config.h"
using namespace std;  

//initialize the algorithm
InitFeasSol::InitFeasSol(Nodes &nodes, Vehicles &vehs): HeuristicFramework(nodes, vehs)
{
    for(int i = 0; i < nodeset.nodenum; i++)
    {
        cus_id_set.push_back(i);
    }
    for(int v = 0; v < vehs.veh_num; v++)
    {
        veh_id_set.push_back(v);
    }
    cpu_time = 0;
}

//build a new route with randomly selected node from the set of uninserted customers -> mainly focus on whether vehicle type and customer type match
/* should consider the following aspects: 1. whether there are remaining customers or remaining vehicles; 
2. whether the newly inserted customers have the same customer type as the vehicle type; 
3. whether there are remaining vehicles of the same type as the first inserted customer type*/
// vector<int> InitFeasSol::build_new_compact_route(vector<int> &remain_cus_id, vector<int> &remain_vehs_id, int &route_veh_id, int &route_veh_type) //remain_cus_type and remain_cus have equal size
// {
//     auto FailTrial = [&]() -> vector<int> {route_veh_id = -1; route_veh_type = -1; return {};};
//     auto SuccTrial = [&](auto cus_iter, auto veh_iter) -> vector<int> 
//     {
//         vector<int> new_compact_route = {0, (*cus_iter), 0};
//         route_veh_id = (*veh_iter);
//         route_veh_type = nodeset.nodetype[(*cus_iter)];
//         remain_cus_id.erase(cus_iter);
//         remain_vehs_id.erase(veh_iter);
//         return new_compact_route;
//     };
//     if(remain_cus_id.empty() || remain_vehs_id.empty()) //if any set is empty, return no new route
//     {
//         FailTrial();
//     }
//     else //if neither set is empty, do the following
//     {
//         //randomly pick a customer to be inserted
//         RandomNumber r;
//         int pick_pos = r.get_rint(0, static_cast<int>(remain_cus_id.size())-1);
//         int cus_type = nodeset.nodetype[remain_cus_id[pick_pos]];
//         //after picking the customer, check whether there are lefting vehicles of the same type as the customer type to be inserted
//         vector<int>::iterator veh_it = find_if(remain_vehs_id.begin(), remain_vehs_id.end(), [&](int x) -> bool {return vehset.veh_type[x] == cus_type;});
//         if(veh_it != remain_vehs_id.end())  //there are at least one vehicle of the same type as the chosen customer
//         {
//             auto cus_it = remain_cus_id.begin() + pick_pos;
//             // vector<int> new_compact_route = {0, (*cus_it), 0};
//             // route_veh_id = (*veh_it);
//             // route_veh_type = nodeset.nodetype[(*cus_it)];
//             // Route new_route_config = build_new_route_struct(route_veh_id, route_veh_type, new_compact_route);
//             // TimeWindowUpdater twupdater(new_route_config, nodeset);
//             // bool isfeas = check_route_insert_feas(twupdater, new_route_config, 1, (*cus_it));
//             // if(isfeas)
//             // {
//             //     remain_cus_id.erase(cus_it);
//             //     remain_vehs_id.erase(veh_it);
//             // }
//             SuccTrial(cus_it, veh_it);
//         }
//         else //there are no lefting vehicles of the same type as the selected customer type -> change a customer to create a new route
//         {
//             vector<int>::iterator new_cus_it = find_if(remain_cus_id.begin(), remain_cus_id.end(), [&](int x) -> bool {return nodeset.nodetype[x] != cus_type;});
//             if(new_cus_it != remain_cus_id.end())
//             {
//                 auto new_veh_it = remain_vehs_id.begin();
//                 SuccTrial(new_cus_it, new_veh_it);
//             }
//             else //this means that the remaining customers and vehicles are incompatible
//             {
//                 FailTrial();
//             }
//         }
//     }
// }

// vector<int> InitFeasSol::build_new_compact_route(vector<OneCus> &remain_cus, vector<OneVeh> &remain_vehs, int &route_id, int &route_type) //remain_cus_type and remain_cus have equal size
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

Route InitFeasSol::build_new_compact_route(vector<int> &remain_cus_id, vector<int> &remain_vehs_id) //remain_cus_type and remain_cus have equal size
{
    int init_vehid = -1, init_vehtype = -1; vector<int> init_compact_route = {0, 0};
    Route init_route = build_new_route_struct(init_vehid, init_vehtype, init_compact_route);
    auto FailTrial = [&]() -> Route {return init_route;};
    auto OneTrial = [&](auto cus_it, auto veh_it) -> Route
    {
        bool isfeas = check_route_insert_feas(init_route, 1, (*cus_it));
        if(!isfeas)
        {
            return FailTrial();
        }
        else
        {
            int route_veh_id = (*veh_it);
            int route_veh_type = nodeset.nodetype[(*cus_it)];
            init_route.veh_id = route_veh_id;
            init_route.veh_type = route_veh_type;
            modify_route_insert_node(init_route, 1, (*cus_it));
            set_route_loads(init_route);
            set_route_mileage(init_route); //not setting the time
            remain_cus_id.erase(cus_it);
            remain_vehs_id.erase(veh_it);
            return init_route;
        }
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
            OneTrial(cus_it, veh_it);
        }
        else //there are no lefting vehicles of the same type as the selected customer type -> change a customer to create a new route
        {
            vector<int>::iterator new_cus_it = find_if(remain_cus_id.begin(), remain_cus_id.end(), [&](int x) -> bool {return nodeset.nodetype[x] != cus_type;});
            if(new_cus_it != remain_cus_id.end())
            {
                auto new_veh_it = remain_vehs_id.begin();
                OneTrial(new_cus_it, new_veh_it);
            }
            else //this means that the remaining customers and vehicles are incompatible
            {
                FailTrial();
            }
        }
    }
}

//create an initial solution for all customers
void InitFeasSol::build_initial_sol(vector<int> &unserved_cus_id, vector<int> &unused_vehs_id)
{
    //unserved_cus = cus_set; unused_vehs = veh_set;
    while(!unserved_cus_id.empty() && !unused_vehs_id.empty())
    {
        Route new_route_config = build_new_compact_route(unserved_cus_id, unused_vehs_id);
        if(new_route_config.veh_id > -1)
        {
            // Route new_route_config = init_route;
            vector<int> unserved_cus_id_same_type;
            for(auto iter = unserved_cus_id.begin(); iter != unserved_cus_id.end(); iter++)
            {
                if(nodeset.nodetype[(*iter)] == new_route_config.veh_type)
                {
                    unserved_cus_id_same_type.push_back((*iter));
                }
            }
            while(!unserved_cus_id_same_type.empty())
            {
                //for all uninserted customers, insert all possible customers in the current route (start with the customer with the least insertion cost)
                vector<pair<double, int>> insert_costs_set;    //to store the cheapest insertion costs of all uninserted customers in the route
                for(auto iter = unserved_cus_id_same_type.begin(); iter != unserved_cus_id_same_type.end(); iter++)
                {
                    pair<double, int> cus_insert = cal_cheapest_insert_cost(new_route_config, (*iter));
                    insert_costs_set.push_back(cus_insert);
                }
                auto selected_cost_iter = min_element(insert_costs_set.begin(), insert_costs_set.end(), [&](pair<double, int> x, pair<double, int> y){return x.first < y.first;});
                double selected_cost = (*selected_cost_iter).first;  //the minimum insertion costs among all customers
                int selected_insert_pos = (*selected_cost_iter).second;
                int selected_cost_index = selected_cost_iter - insert_costs_set.begin();
                int selected_node = unserved_cus_id_same_type[selected_cost_index];    //the customer with the minium insertion cost
                if(selected_cost < INF) //selected_cost < INF -> feasible
                {
                    modify_route_insert_node(new_route_config, selected_insert_pos, selected_node);
                    set_route_loads(new_route_config);
                    set_route_mileage(new_route_config);
                    unserved_cus_id_same_type.erase(unserved_cus_id_same_type.begin() + selected_cost_index);  //remove the customer from the set of uninserted customers
                }
                else
                {
                    break;
                }
            }
            set_route_arrdep_tw(new_route_config);
            best_sol.sol_config.push_back(new_route_config);
        }
        else //in case that the remaining customers and vehicles are incompatible
        {
            cerr << "No initial solution can be built from the given set of customers and vehicles.";
            break;
        }
    }
    build_complete_sol(best_sol);
}

void InitFeasSol::run_algorithm()
{
    double start = clock(); // get current time
    vector<int> uninserted_cus = cus_id_set;
    vector<int> unused_vehs = veh_id_set;
    build_initial_sol(uninserted_cus, unused_vehs);
    cpu_time = (clock() - start ) / (double) CLOCKS_PER_SEC;
}