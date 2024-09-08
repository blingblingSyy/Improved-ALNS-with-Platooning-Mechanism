#ifndef VRP_INC_H_
#define VRP_INC_H_

#include "example/model/basic/ADijkstraSol.h"
#include "example/model/basic/config.h"
#include "example/model/basic/KSPBuilder.h"
#include "example/model/basic/Nodes.h"
#include "example/model/basic/RawInstance.h"
#include "example/model/basic/Vehicles.h"
#include "example/model/establish/APlatoon.h"
#include "example/model/establish/ARoute.h"
#include "example/model/establish/PlatoonMaker.h"
#include "example/model/establish/ResultWriter.h"
#include "example/model/establish/TimeWindowUpdater.h"
#include "example/model/establish/VRPSolution.h"
#include "example/operators/nodeDestroyOps/Node_Random_Removal.h"
#include "example/operators/nodeDestroyOps/Node_Route_Based_Removal.h"
#include "example/operators/nodeDestroyOps/Node_Shaw_Removal.h"
#include "example/operators/nodeDestroyOps/Node_Worst_Removal.h"
#include "example/operators/nodeRepairOps/Node_Greedy_Insert.h"
#include "example/operators/nodeRepairOps/Node_Random_Insert.h"
#include "example/operators/nodeRepairOps/Node_Regret_Insert.h"
#include "example/operators/pathDestroyOps/Path_AvgLenDiff_Removal.h"
#include "example/operators/pathDestroyOps/Path_Cardinality_Removal.h"
#include "example/operators/pathDestroyOps/Path_NodeFreq_Removal.h"
#include "example/operators/pathDestroyOps/Path_Random_Removal.h"
#include "example/operators/pathRepairOps/Path_KMaxLen_Insert.h"
#include "example/operators/pathRepairOps/Path_NodeFreq_Insert.h"
#include "example/operators/pathRepairOps/Path_Random_Insert.h"
#include "example/operators/tabuSearch/pathTabu.h"
#include "example/operators/Operators_Parameters.h"

#endif

