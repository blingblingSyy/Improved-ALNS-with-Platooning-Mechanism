#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include <fstream>
#include <iomanip>
#include "example/VRP_inc.h"
#include "utility.h"
using namespace std;  


ResultWriter::ResultWriter(VRPSolution& solution, string instancename, string stage_name, double cpu) : input_sol(solution)
{
    this->result_filepath = result_dir + "solutions/" + instancename + "_" + stage_name + ".txt"; // result_dir = "C:/Users/SYY/Improved_ALNS_Git/result/" (in config.h)
    this->instance_name = instancename;
    // this->input_sol = solution;
    this->cpu = cpu;
    this->solution_stage = stage_name;  //! e.g.: initial feasible solution
}

//write the result to file 
void ResultWriter::write_result()
{
    outfile.open(result_filepath, ios::out | ios::trunc);
    outfile << "Instance name: " << instance_name << endl;
    outfile << "Algorithm Stage: " << solution_stage << endl;
    outfile << endl;
    
    outfile << "total objective value: " << input_sol.getObjectiveValue() << endl;
    outfile << "total distance before platooning: " << input_sol.getTotalDistBeforePlatooning() << endl;
    outfile << "total energy-related distance cost: " << input_sol.getTotalDistAfterPlatooning() << endl;
    outfile << "total trip duration: " << input_sol.getTotalTripDuration() << endl;
    outfile << "total unserved passenger requests: " << input_sol.getTotalUnservedRequests(0) << endl;
    outfile << "total unserved freight requests: " << input_sol.getTotalUnservedRequests(1) << endl;
    // outfile << "CPU time before platooning: " << cpu[0] << endl;
    outfile << "CPU time: " << cpu << endl;
    outfile << endl;

    outfile << "* All routes:" << endl;
    for(int i = 0; i < input_sol.getRoutesNum(); i++)
    {
        outfile << "vehicle id: " << input_sol.getOneRoute(i)->getVehID() << "  ";
        outfile << "vehicle type: " << input_sol.getOneRoute(i)->getVehType() << endl;
        outfile << "\tcompact route: ";
        for(int r = 0; r < input_sol.getOneRoute(i)->getCompactRoute().size(); r++)
        {
            outfile << input_sol.getOneRoute(i)->getCompactRoute()[r] << "  ";
        }
        outfile << endl;
        outfile << "\tused paths between two served nodes: ";
        for(int p = 0; p < input_sol.getOneRoute(i)->getCompactRoute().size()-1; p++)
        {
            outfile << input_sol.getOneRoute(i)->getUsedPathsAtArc(p) << "  ";
        }
        outfile << endl;
        outfile << "\textended route: ";
        for(int r = 0; r < input_sol.getOneRoute(i)->getExtendedRoute().size(); r++)
        {
            outfile << input_sol.getOneRoute(i)->getExtendedRoute()[r] << "  ";
        }
        outfile << endl;
        outfile << "\tnode labels: ";
        for(int s = 0; s < input_sol.getOneRoute(i)->getNodeLables().size(); s++)
        {
            outfile << input_sol.getOneRoute(i)->getNodeLables()[s] << "  ";
        }
        outfile << endl;
        outfile << "\tarrival time windows: ";
        for(int t = 0; t < input_sol.getOneRoute(i)->getRouteExpectedArrTW().size(); t++)
        {
            outfile << "{" << input_sol.getOneRoute(i)->getNodeExpectedArrTW(t)[0] << ", " << input_sol.getOneRoute(i)->getNodeExpectedArrTW(t)[1] << "}" << "  ";
        }
        outfile << endl;
        outfile << "\tdeparture time windows: ";
        for(int t = 0; t < input_sol.getOneRoute(i)->getRouteExpectedDepTW().size(); t++)
        {
            outfile << "{" << input_sol.getOneRoute(i)->getNodeExpectedDepTW(t)[0] << ", " << input_sol.getOneRoute(i)->getNodeExpectedDepTW(t)[1] << "}" << "  ";
        }
        outfile << endl;
        outfile << "\tarrival time: ";
        for(int t = 0; t < input_sol.getOneRoute(i)->getFinalArrTime().size(); t++)
        {
            outfile << input_sol.getOneRoute(i)->getFinalArrTime()[t] << "  ";
        }
        outfile << endl;
        outfile << "\tdeparture time: ";
        for(int t = 0; t < input_sol.getOneRoute(i)->getFinalDepTime().size(); t++)
        {
            outfile << input_sol.getOneRoute(i)->getFinalDepTime()[t] << "  ";
        }
        outfile << endl;
        outfile << "\twaiting time: ";
        for(int t = 0; t < input_sol.getOneRoute(i)->getFinalWaitTime().size(); t++)
        {
            outfile << input_sol.getOneRoute(i)->getFinalWaitTime()[t] << "  ";
        }
        outfile << endl;
        outfile << "\ttotal waiting time: ";
        outfile << input_sol.getOneRoute(i)->getFinalTotalWaitTime() << "  ";
        outfile << endl;
        outfile << "\troute load: ";
        for(int r = 0; r < input_sol.getOneRoute(i)->getRouteLoad().size(); r++)
        {
            outfile << input_sol.getOneRoute(i)->getRouteLoad()[r] << "  ";
        }
        outfile << endl;
        outfile << "\troute mileage: ";
        for(int m = 0; m < input_sol.getOneRoute(i)->getRouteMileage().size(); m++)
        {
            outfile << input_sol.getOneRoute(i)->getRouteMileage()[m] << "  ";
        }
        outfile << endl;
    }
    outfile << endl;
    outfile << "* All platoons: " << endl;
    vector<APlatoon*> allplatoons = input_sol.getAllPlatoons();
    for(int i = 0; i < allplatoons.size(); i++)
    {
        outfile << "platoon " << i+1 << "\t";
        outfile << "platooning arc: {" << allplatoons[i]->getArc()[0] << ", " << allplatoons[i]->getArc()[1] << "}" <<endl;
        outfile << "\tplatoons on this arc: " << endl;
        for(int p = 0; p < allplatoons[i]->getPlatoonConfig().size(); p++)
        {
            int routeid = allplatoons[i]->getPlatoonConfig()[p].first;
            outfile << "\t\tvehicle id: " << input_sol.getOneRoute(routeid)->getVehID() << "  " << "arc position: " << allplatoons[i]->getPlatoonConfig()[p].second << endl;
        }
        outfile << "\tenergy saving on this arc: " << allplatoons[i]->getEnergySaving() << endl;
    }

    outfile.close();
}

void ResultWriter::record_solution()
{
    outfile.open(result_filepath, ios::out | ios::trunc);
    outfile << "Instance name: " << instance_name << endl;
    outfile << "Algorithm Stage: " << solution_stage << endl;
    outfile << endl;
    
    outfile << "total distance before platooning: " << input_sol.getTotalDistBeforePlatooning() << endl;
    outfile << "total energy-related distance cost: " << input_sol.getTotalDistAfterPlatooning() << endl;
    outfile << "total trip duration: " << input_sol.getTotalTripDuration() << endl;
    outfile << "total unserved requests: " << input_sol.getTotalUnservedRequests() << endl;
    // outfile << "CPU time before platooning: " << cpu[0] << endl;
    outfile << "CPU time: " << cpu << endl;

    outfile.close();
}