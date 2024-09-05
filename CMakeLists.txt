# minimum version required
cmake_minimum_required (VERSION 3.26.4)

# project name
project(ImprovedALNS)

# debug setting
set(CMAKE_BUILD_TYPE "Debug")

# include paths of header files 
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/couplingVRP)
# include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/model/basic)
# include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/model/establish)
# include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/operators)
# include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/operators/nodeDestroyOps)
# include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/operators/nodeRepairOps)
# include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/operators/pathDestroyOps)
# include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/operators/pathRepairOps)
# include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/operators/tabuSearch)
include_directories(${PROJECT_SOURCE_DIR}/src)
# include_directories(${PROJECT_SOURCE_DIR}/src/acceptanceModule)
# include_directories(${PROJECT_SOURCE_DIR}/src/improvedALNS)
# include_directories(${PROJECT_SOURCE_DIR}/src/lib)
# include_directories(${PROJECT_SOURCE_DIR}/src/localsearch)
# include_directories(${PROJECT_SOURCE_DIR}/src/statistics)

# add source directories of cpp files
aux_source_directory(./couplingVRP VRPFiles)
aux_source_directory(./couplingVRP/model/basic VRPFiles)
aux_source_directory(./couplingVRP/model/establish VRPFiles)
aux_source_directory(./couplingVRP/operators OpsFiles)
aux_source_directory(./couplingVRP/operators/nodeDestroyOps OpsFiles)
aux_source_directory(./couplingVRP/operators/nodeRepairOps OpsFiles)
aux_source_directory(./couplingVRP/operators/pathDestroyOps OpsFiles)
aux_source_directory(./couplingVRP/operators/pathRepairOps OpsFiles)
aux_source_directory(./couplingVRP/operators/tabuSearch OpsFiles)
aux_source_directory(./src SrcFiles)
aux_source_directory(./src/acceptanceModule SrcFiles)
aux_source_directory(./src/improvedALNS SrcFiles)
aux_source_directory(./src/lib SrcFiles)
aux_source_directory(./src/localsearch SrcFiles)
aux_source_directory(./src/statistics SrcFiles)

# set executable file output path
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build)

# add the name of executable file
add_executable(main ${VRPFiles} ${OpsFiles} ${SrcFiles})
