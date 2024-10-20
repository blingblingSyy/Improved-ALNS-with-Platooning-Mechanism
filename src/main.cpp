#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include <fstream> 
#include <tuple>
#include "alns/ALNS_inc.h"
#include "example/VRP_inc.h"
#include "example/Operators_inc.h"
#include "utility.h"
using namespace std;  


int main()
{
    // //! generate large dataset
    // vector<int> nodesizes = {50, 100, 250, 500};
    // vector<bool> coord_cluster_modes = {true, false};
    // vector<bool> sertime_peak_modes = {true, false};
    // vector<string> all_instances;
    // const int instanceNumPerMode = 3;
    // for(int i = 0; i < nodesizes.size(); i++)
    // {
    //     for(int j = 0; j < coord_cluster_modes.size(); j++)
    //     {
    //         for(int k = 0; k < sertime_peak_modes.size(); k++)
    //         {
    //             for(int z = 0; z < instanceNumPerMode; z++)
    //             {
    //                 string coord = (coord_cluster_modes[j] == true) ? "cluster" : "uniform";
    //                 string sertw = (sertime_peak_modes[k] == true) ? "peak" : "even";
    //                 string filename = to_string(nodesizes[i]) + "_" + coord + "_" + sertw + "_" + to_string(z+1);
    //                 all_instances.push_back(filename);
    //                 RawInstance instance_i(nodesizes[i], coord_cluster_modes[j], sertime_peak_modes[k]);
    //                 instance_i.writedata(dataset_dir + "Large_Dataset/", filename);
    //             }
    //         }
    //     }
    // }

    // //! small test data
    // string test_filename = "test2";
    // string filepath = dataset_dir + "Small_Dataset/" + test_filename + ".txt"; //"C:/Users/SYY/Improved_ALNS_Git/Small_Dataset/xxx.txt"
    // RawInstance data1(filepath);
    // vector<vector<int>> disconnected_links;
    // disconnected_links.push_back({0,1});
    // disconnected_links.push_back({0,3});
    // disconnected_links.push_back({1,4});
    // disconnected_links.push_back({2,4});
    // disconnected_links.push_back({2,5});
    // disconnected_links.push_back({3,5});
    // disconnected_links.push_back({4,5});
    // Nodes nodeset(data1, disconnected_links);
    // Vehicles vehset(data1);

    //! large test data
    string test_filename = "100_cluster_peak_1";
    string filepath = dataset_dir + "Large_Dataset/" + test_filename + ".txt"; //"C:/Users/SYY/Improved_ALNS_Git/Large_Dataset/xxx.txt"
    RawInstance data1(filepath);
    Nodes nodeset(data1);
    Vehicles vehset(data1);

    //! build initial solution
    VRPSolution initialsol(nodeset, vehset);
    initialsol.buildInitialSol();
    //! write down the results
    ResultWriter writer1(initialsol, test_filename, "initial_solution", initialsol.getCpuAfterPlatooning()); //mip_optimal_solution
    writer1.write_result();

    //! add operators
    Operators_Parameters ops_param;
    ops_param.loadXMLParameters(base_dir + "src/param.xml"); // src/param.xml 

    PathTabu pathtabu(ops_param.getTabuTenure(), nodeset.getAllAvailPathSize());
    Node_Random_Removal nodeRandomR("Node Random Removal", ops_param, nodeset.getCusNum());
    Node_RouteBased_Removal nodeRouteBasedR("Node Route-based Removal");
    Node_Shaw_Removal nodeShawR("Node Shaw Removal", ops_param, nodeset);
    Node_Worst_Removal nodeWorstR("Node Worst Removal", ops_param, nodeset.getCusNum());
    Node_Greedy_Insert nodeGreedyI("Node Greedy Insert");
    Node_Random_Insert nodeRandomI("Node Random Insert");
    Node_Regret_Insert nodeRegret2I("Node Regret-2 Insert", 2);
    Path_AvgLenDiff_Removal pathAvgLenDiffR("Path Average-Length-Difference Removal", ops_param, pathtabu, nodeset);
    Path_AvgLen_Removal pathAvgLenR("Path Average-Length Removal", ops_param, pathtabu, nodeset);
    Path_Cardinality_Removal pathCardiR("Path Cardinality Removal", ops_param, pathtabu, nodeset);
    Path_NodeFreq_Removal pathNFreqR("Path Node-Frequency Removal", ops_param, pathtabu, nodeset);
    Path_Random_Removal pathRandomR("Path Random Removal", ops_param, pathtabu, nodeset);
    Path_KMaxLen_Insert pathKLenI("Path K-max Length Insert", ops_param, nodeset);
    Path_NodeFreq_Insert pathNFreqI("Path Node-Frequency Insert", ops_param, nodeset);
    Path_Random_Insert pathRandomI("Path Random Insert", nodeset);

	ALNS_Parameters alnsParam;
	alnsParam.loadXMLParameters(base_dir + "src/param.xml");

	CoolingSchedule_Parameters csParam(alnsParam);
	csParam.loadXMLParameters(base_dir + "src/param.xml");
	ICoolingSchedule* cs = CoolingScheduleFactory::makeCoolingSchedule(dynamic_cast<ISolution&>(initialsol),csParam);
	SimulatedAnnealing sa(*cs);

	StrategyManager stMan(alnsParam);
    
	stMan.addNodeDestroyOperator(dynamic_cast<ANodeDestroyOperator&>(nodeRandomR));
	// stMan.addNodeDestroyOperator(dynamic_cast<ANodeDestroyOperator&>(nodeShawR));
	// stMan.addNodeDestroyOperator(dynamic_cast<ANodeDestroyOperator&>(nodeWorstR));
	// stMan.addNodeDestroyOperator(dynamic_cast<ANodeDestroyOperator&>(nodeRouteBasedR));

	// stMan.addNodeRepairOperator(dynamic_cast<ANodeRepairOperator&>(nodeGreedyI));
	stMan.addNodeRepairOperator(dynamic_cast<ANodeRepairOperator&>(nodeRandomI));
	// stMan.addNodeRepairOperator(dynamic_cast<ANodeRepairOperator&>(nodeRegret2I));

	// stMan.addPathDestroyOperator(dynamic_cast<APathDestroyOperator&>(pathAvgLenDiffR));
	// stMan.addPathDestroyOperator(dynamic_cast<APathDestroyOperator&>(pathAvgLenR));
	// stMan.addPathDestroyOperator(dynamic_cast<APathDestroyOperator&>(pathCardiR));
	// stMan.addPathDestroyOperator(dynamic_cast<APathDestroyOperator&>(pathNFreqR));
	stMan.addPathDestroyOperator(dynamic_cast<APathDestroyOperator&>(pathRandomR));

	// stMan.addPathRepairOperator(dynamic_cast<APathRepairOperator&>(pathKLenI));
	// stMan.addPathRepairOperator(dynamic_cast<APathRepairOperator&>(pathNFreqI));
	stMan.addPathRepairOperator(dynamic_cast<APathRepairOperator&>(pathRandomI));

	SimpleBestSolutionManager bestSM(alnsParam);
	SimpleLocalSearchManager simpleLsManager(alnsParam);

	// simpleLsManager.addLocalSearchOperator(dynamic_cast<ILocalSearch&>(none));

	ALNS alns(test_filename, dynamic_cast<ISolution&>(initialsol), dynamic_cast<IAcceptanceModule&>(sa), alnsParam, dynamic_cast<AStrategyManager&>(stMan), dynamic_cast<IBestSolutionManager&>(bestSM), dynamic_cast<ILocalSearchManager&>(simpleLsManager));

	alns.addUpdatable(dynamic_cast<IUpdatable&>(pathtabu));

	alns.solve();

    //! write down the results
    // VRPSolution& alnsSol = dynamic_cast<VRPSolution&>(**(alns.getBestSolutionManager()->begin()));
    VRPSolution& alnsSol = dynamic_cast<VRPSolution&>(*alns.getBestSolution());
    string stage_name = "alns_solution";
    stage_name += (alnsParam.getGlobalNoise()) ? "_wt_noise" : "_wo_noise";
    ResultWriter writer2(alnsSol, test_filename, stage_name, alns.getCpuTime());
    writer2.write_result();

    // alns.end();

    return 0;
}