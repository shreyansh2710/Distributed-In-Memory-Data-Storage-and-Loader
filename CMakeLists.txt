cmake_minimum_required(VERSION 3.12)
project(DistributedKVStorage VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread")

include_directories(include)

add_library(cluster_core
    src/ConfigParser.cpp
    src/NetworkMock.cpp
    src/StorageRouter.cpp
    src/StorageNode.cpp
    src/ClusterCoordinator.cpp
)

add_executable(distributed_storage_node apps/main.cpp)
target_link_libraries(distributed_storage_node PRIVATE cluster_core)
