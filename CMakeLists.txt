# minimum version required
cmake_minimum_required (VERSION 3.26.4)

# # project name
# project(ImprovedALNS)

# # debug setting
# set(CMAKE_BUILD_TYPE "Debug")

# # include paths of header files 
# include_directories(${PROJECT_SOURCE_DIR})
# include_directories(${PROJECT_SOURCE_DIR}/couplingVRP)
# # include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/model/basic)
# # include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/model/establish)
# # include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/operators)
# # include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/operators/nodeDestroyOps)
# # include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/operators/nodeRepairOps)
# # include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/operators/pathDestroyOps)
# # include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/operators/pathRepairOps)
# # include_directories(${PROJECT_SOURCE_DIR}/couplingVRP/operators/tabuSearch)
# include_directories(${PROJECT_SOURCE_DIR}/src)
# # include_directories(${PROJECT_SOURCE_DIR}/src/acceptanceModule)
# # include_directories(${PROJECT_SOURCE_DIR}/src/improvedALNS)
# # include_directories(${PROJECT_SOURCE_DIR}/src/lib)
# # include_directories(${PROJECT_SOURCE_DIR}/src/localsearch)
# # include_directories(${PROJECT_SOURCE_DIR}/src/statistics)

# # add source directories of cpp files
# aux_source_directory(./couplingVRP VRPFiles)
# aux_source_directory(./couplingVRP/model/basic VRPFiles)
# aux_source_directory(./couplingVRP/model/establish VRPFiles)
# aux_source_directory(./couplingVRP/operators OpsFiles)
# aux_source_directory(./couplingVRP/operators/nodeDestroyOps OpsFiles)
# aux_source_directory(./couplingVRP/operators/nodeRepairOps OpsFiles)
# aux_source_directory(./couplingVRP/operators/pathDestroyOps OpsFiles)
# aux_source_directory(./couplingVRP/operators/pathRepairOps OpsFiles)
# aux_source_directory(./couplingVRP/operators/tabuSearch OpsFiles)
# aux_source_directory(./src SrcFiles)
# aux_source_directory(./src/acceptanceModule SrcFiles)
# aux_source_directory(./src/improvedALNS SrcFiles)
# aux_source_directory(./src/lib SrcFiles)
# aux_source_directory(./src/localsearch SrcFiles)
# aux_source_directory(./src/statistics SrcFiles)

# # set executable file output path
# set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build)

# # add the name of executable file
# add_executable(main ${VRPFiles} ${OpsFiles} ${SrcFiles})

# 定义项目名称及使用的语言
project(MyProject LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 自动收集 src 目录及其子目录下的所有 .cpp 文件
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src-code/*.cpp)

# # 自动收集 include 目录中的头文件路径
# file(GLOB_RECURSE HEADER_DIRS ${PROJECT_SOURCE_DIR}/src-code/*.h)

# 添加可执行文件并使用自动收集的源文件列表
add_executable(MyApp ${SOURCES})

# 为可执行文件指定递归包含的头文件目录
target_include_directories(MyApp PRIVATE 
    ${PROJECT_SOURCE_DIR}/src-code
    ${PROJECT_SOURCE_DIR}/src-code/couplingVRP
    ${PROJECT_SOURCE_DIR}/src-code/couplingVRP/model/basic
    ${PROJECT_SOURCE_DIR}/src-code/couplingVRP/model/establish
    ${PROJECT_SOURCE_DIR}/src-code/couplingVRP/operators
    ${PROJECT_SOURCE_DIR}/src-code/couplingVRP/operators/nodeDestroyOps
    ${PROJECT_SOURCE_DIR}/src-code/couplingVRP/operators/nodeRepairOps
    ${PROJECT_SOURCE_DIR}/src-code/couplingVRP/operators/pathDestroyOps
    ${PROJECT_SOURCE_DIR}/src-code/couplingVRP/operators/pathRepairOps
    ${PROJECT_SOURCE_DIR}/src-code/couplingVRP/operators/tabuSearch
    ${PROJECT_SOURCE_DIR}/src-code/src
    ${PROJECT_SOURCE_DIR}/src-code/src/acceptanceModule
    ${PROJECT_SOURCE_DIR}/src-code/src/improvedALNS
    ${PROJECT_SOURCE_DIR}/src-code/src/lib
    ${PROJECT_SOURCE_DIR}/src-code/src/localsearch
    ${PROJECT_SOURCE_DIR}/src-code/src/statistics
)
