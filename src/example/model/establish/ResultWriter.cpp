#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include <fstream>
#include "example/VRP_inc.h"
#include "utility.h"
using namespace std;  


ResultWriter::ResultWriter(VRPSolution& solution, string instancename, string stage_name, double cpu)
{
    result_filepath = "result/" + instancename + "/" + stage_name + ".txt";
    instance_name = instancename;
    input_sol = solution;
    cpu = cpu;
    solution_stage = stage_name;  //! e.g.: initial feasible solution
}

//write the result to file 
void ResultWriter::write_result()
{
    outfile.open(result_filepath, ios::out | ios::trunc);
    outfile << "Instance name: " << instance_name << endl;
    outfile << "Algorithm Stage: " << solution_stage << endl;
    outfile << "* All routes:" << endl;
    for(int i = 0; i < input_sol.getRoutesNum(); i++)
    {
        outfile << "vehicle id: " << input_sol.getOneRoute(i)->getVehID() << "\t";
        outfile << "vehicle type: " << input_sol.getOneRoute(i)->getVehType() << endl;
        outfile << "compact route: ";
        for(int r = 0; r < input_sol.getOneRoute(i)->getCompactRoute().size(); r++)
        {
            outfile << input_sol.getOneRoute(i)->getCompactRoute()[r] << "\t";
        }
        outfile << endl;
        outfile << "used paths between two served nodes: ";
        for(int p = 0; p < input_sol.getOneRoute(i)->getCompactRoute().size()-1; p++)
        {
            outfile << input_sol.getOneRoute(i)->getUsedPathsAtArc(p) << "\t";
        }
        outfile << endl;
        outfile << "extended route: ";
        for(int r = 0; r < input_sol.getOneRoute(i)->getExtendedRoute().size(); r++)
        {
            outfile << input_sol.getOneRoute(i)->getExtendedRoute()[r] << "\t";
        }
        outfile << endl;
        outfile << "node labels: ";
        for(int s = 0; s < input_sol.getOneRoute(i)->getNodeLables().size(); s++)
        {
            outfile << input_sol.getOneRoute(i)->getNodeLables()[s] << "\t";
        }
        outfile << endl;
        outfile << "arrival time windows: ";
        for(int t = 0; t < input_sol.getOneRoute(i)->getRouteExpectedArrTW().size(); t++)
        {
            outfile << "{" << input_sol.getOneRoute(i)->getNodeExpectedArrTW(t)[0] << ", " << input_sol.getOneRoute(i)->getNodeExpectedArrTW(t)[1] << "}" << "\t";
        }
        outfile << endl;
        outfile << "departure time windows: ";
        for(int t = 0; t < input_sol.getOneRoute(i)->getRouteExpectedDepTW().size(); t++)
        {
            outfile << "{" << input_sol.getOneRoute(i)->getNodeExpectedDepTW(t)[0] << ", " << input_sol.getOneRoute(i)->getNodeExpectedDepTW(t)[1] << "}" << "\t";
        }
        outfile << endl;
        outfile << "arrival time: ";
        for(int t = 0; t < input_sol.getOneRoute(i)->getFinalArrTime().size(); t++)
        {
            outfile << input_sol.getOneRoute(i)->getFinalArrTime()[t] << "\t";
        }
        outfile << endl;
        outfile << "departure time: ";
        for(int t = 0; t < input_sol.getOneRoute(i)->getFinalDepTime().size(); t++)
        {
            outfile << input_sol.getOneRoute(i)->getFinalDepTime()[t] << "\t";
        }
        outfile << endl;
        outfile << "route load: ";
        for(int r = 0; r < input_sol.getOneRoute(i)->getRouteLoad().size(); r++)
        {
            outfile << input_sol.getOneRoute(i)->getRouteLoad()[r] << "\t";
        }
        outfile << endl;
        outfile << "route mileage: ";
        for(int m = 0; m < input_sol.getOneRoute(i)->getRouteMileage().size(); m++)
        {
            outfile << input_sol.getOneRoute(i)->getRouteMileage()[m] << "\t";
        }
        outfile << endl;
    }
    outfile << endl;
    outfile << "* All platoons: " << endl;
    vector<APlatoon*> allplatoons = input_sol.getAllPlatoons();
    for(int i = 0; i < allplatoons.size(); i++)
    {
        outfile << "platooning arc: {" << allplatoons[i]->getArc()[0] << ", " << allplatoons[i]->getArc()[1] << "}" <<endl;
        outfile << "\tplatoons on this arc: " << endl;
        for(int p = 0; p < allplatoons[i]->getPlatoonConfig().size(); p++)
        {
            outfile << "\t\tplatoon " << p+1 << ": ";
            outfile << "\t\troute id: " << allplatoons[i]->getPlatoonConfig()[p].first << "\t" << "arc position: " << allplatoons[i]->getPlatoonConfig()[p].second << endl;
        }
        outfile << "energy saving on this arc:" << allplatoons[i]->getEnergySaving() << endl;
    }
    outfile << endl;
    outfile << "total distance before platooning: " << input_sol.getTotalDistBeforePlatooning() << endl;
    outfile << "total energy-related distance cost: " << input_sol.getTotalDistAfterPlatooning() << endl;
    outfile << "total trip duration: " << input_sol.getTotalTripDuration() << endl;
    outfile << "total unserved requests: " << input_sol.getTotalUnservedRequests() << endl;
    // outfile << "CPU time before platooning: " << cpu[0] << endl;
    outfile << "CPU time: " << cpu << endl;

    outfile.close();
}

void ResultWriter::record_solution()
{
    outfile.open(result_filepath, ios::out | ios::trunc);
    outfile << "Instance name: " << instance_name << endl;
    outfile << "Algorithm Stage: " << solution_stage << endl;

    outfile << "total distance before platooning: " << input_sol.getTotalDistBeforePlatooning() << endl;
    outfile << "total energy-related distance cost: " << input_sol.getTotalDistAfterPlatooning() << endl;
    outfile << "total trip duration: " << input_sol.getTotalTripDuration() << endl;
    outfile << "total unserved requests: " << input_sol.getTotalUnservedRequests() << endl;
    // outfile << "CPU time before platooning: " << cpu[0] << endl;
    outfile << "CPU time: " << cpu << endl;

    outfile.close();
}