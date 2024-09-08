# minimum version required
cmake_minimum_required (VERSION 3.26.4)

# define project name and the used languages
project(MyProject LANGUAGES CXX)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# automatically collect all .cpp files under src files and the sub-files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# add executable files and use the collected source cpp files
add_executable(MyApp ${SOURCES})

# specify the header directories for the executable files
# search all the paths to the header files under the "include" directory
target_include_directories(MyApp PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
)
