# Distributed-In-Memory-Data-Storage-and-Loader

# Distributed In-Memory Key-Value Storage & Loading System

A highly performant, modular C++ implementation of a distributed key-value storage and processing cluster. The system partitions data using standard consistent routing methods, allowing cluster nodes to ingest local file shards concurrently while routing records to their deterministic owner nodes over a simulated low-latency network mesh.

---

##  Project Architecture & Component Boundaries

The project features a decoupled layout that cleanly isolates interface declarations (`.hpp`) from concrete implementations (`.cpp`), ensuring fast compilation times and clear separations of concerns:

```text
distributed_kv/
├── CMakeLists.txt              # Build configuration orchestrator
├── include/                    # Public API Interface Declarations
│   ├── Record.hpp             # Common data structures and packet wire types
│   ├── StorageRouter.hpp      # Consistent placement calculation engine
│   ├── StorageNode.hpp        # In-memory storage engine bucket with shared read/write locks
│   └── ClusterCoordinator.hpp # Cluster cluster-wide lifecycle supervisor
├── src/                        # Implementation Logic Definitions
│   ├── StorageRouter.cpp      
│   ├── StorageNode.cpp        
│   └── ClusterCoordinator.cpp  
└── apps/                       
    └── main.cpp                # High-level entry point simulating local file systems
