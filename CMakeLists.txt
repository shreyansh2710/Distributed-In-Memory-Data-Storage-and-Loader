cmake_minimum_required(VERSION 3.12)
project(DistributedKVStorage VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define library from implementation sources
add_library(kv_cluster 
    src/StorageRouter.cpp
    src/StorageNode.cpp
    src/ClusterCoordinator.cpp
)

# Bind header locations
target_include_directories(kv_cluster PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define executable application
add_executable(distributed_storage_node apps/main.cpp)
target_link_libraries(distributed_storage_node PRIVATE kv_cluster)
