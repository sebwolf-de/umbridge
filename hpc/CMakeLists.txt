cmake_minimum_required(VERSION 3.10)

project(Load-Balancer)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(load-balancer
src/main.cpp
src/JobQueue.cpp
src/LoadBalancer.cpp
src/QueuingModel.cpp
src/Worker.cpp
src/WorkerList.cpp
)

target_include_directories(load-balancer PUBLIC ../lib)
target_include_directories(load-balancer PUBLIC external)

add_library(umbridge ../lib/umbridge.cpp)
target_link_libraries(load-balancer PUBLIC umbridge)


