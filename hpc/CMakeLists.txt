cmake_minimum_required(VERSION 3.10)

project(Load-Balancer)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(load-client
src/client.cpp
src/JobQueue.cpp
src/LoadBalancer.cpp
src/QueuingModel.cpp
src/Worker.cpp
src/WorkerList.cpp
)

add_executable(load-server
src/server.cpp
)

target_include_directories(load-client PUBLIC ../lib)
target_include_directories(load-client PUBLIC external)
target_include_directories(load-server PUBLIC external)

add_library(umbridge ../lib/umbridge.cpp)
target_link_libraries(load-client PUBLIC umbridge)

find_package(MPI REQUIRED)
target_link_libraries(load-server PUBLIC MPI::MPI_CXX)

target_compile_options(load-client PRIVATE -fsanitize=address)
target_link_options(load-client PRIVATE -fsanitize=address)


