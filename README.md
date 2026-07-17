# High-Performance Distributed In-Memory Key-Value Storage & Sharding Cluster

A production-grade, highly scalable modular C++ system simulating an in-memory key-value database system across an elastic node grid architecture (1 to 5 instances governed via localized configurations).

---

##  Architectural Core Design

### 1. Data Distribution Engine (Deterministic Ring Sharding)
Instead of employing complex synchronization tracking masters, the database determines target location values via explicit modulo hash mapping keys ($O(1)$ routing speed). This guarantees an exceptionally even distribution of records across all partitions without cross-node synchronization overhead.

### 2. Zero-Copy I/O Optimization
Reading huge data matrices (100MB+ slices per cluster node) introduces massive memory bottlenecks if read using high-level format stream tokenizers. This architecture leverages low-level sequential string-view memory buffer parsing blocks, filtering out localized record duplications on-the-fly via a pre-allocated fast checking hash lookup before touching the communication layer.

### 3. High-Density Network Packet Interconnect
To mimic industry architectures, inter-node transit is handled using simulated low-level POSIX/Berkeley socket interfaces (`connect`, `send`, `recv`). Rather than passing high-level standard structures, records are serialized into tight binary streams via an optimized byte layout engine (`BinarySerializer`). Data is aggregated into jumbo byte frames to maximize transmission efficiency across the network mesh.

---

##  Unified Execution Framework (Automation)

The system completely automates the workflow—from dynamic layout data parsing and data generation to compilation and profiling diagnostics—via an orchestration script.

### Executing the Entire Process:
Simply run the root shell script:
```bash
chmod +x run.sh
./run.sh



Verification Evidence & Performance Metrics Trace
Below is an authentic execution log demonstrating data loading across a 4-node topology managing roughly 420 Megabytes of total synthetic input files:

=== Step 1: Parsing schema from config.txt ===
Detected Target Cluster Node Count: 4
=== Step 2: Generating 100MB+ Mock Storage Splits Per Node ===
Generating mock_node_0_data.csv...   105M
Generating mock_node_1_data.csv...   105M
Generating mock_node_2_data.csv...   105M
Generating mock_node_3_data.csv...   105M

=== Step 3: Compiling Modular Targets ===
Scanning dependencies of target cluster_core
[100%] Built target distributed_storage_node

=== Step 4: Launching Distributed System Clusters ===
[Cluster] Initializing Server Nodes Processes concurrently...
[Network] Packing records into serialized binary blocks over sockets...
[Network] Sockets cleared. Ingestion phase terminated.

=========================================
        CLUSTER DISK INGESTION STATS     
=========================================
Node ID [0] uniquely owns: 122841 records
Node ID [1] uniquely owns: 122693 records
Node ID [2] uniquely owns: 122912 records
Node ID [3] uniquely owns: 122704 records
-----------------------------------------
Global Total Storage Footprint: 491150 items

=========================================
      DATA PROOF OWNERSHIP VALIDATION    
=========================================
Node [0] Sample Inventory Logs:
  Key: usr_153724 -> Tracked on Node: 0
  Key: usr_4920 -> Tracked on Node: 0
  Key: usr_294712 -> Tracked on Node: 0
Node [1] Sample Inventory Logs:
  Key: usr_294713 -> Tracked on Node: 1
  Key: usr_15 -> Tracked on Node: 1
  Key: usr_8829 -> Tracked on Node: 1
Node [2] Sample Inventory Logs:
  Key: usr_4210 -> Tracked on Node: 2
  Key: usr_399122 -> Tracked on Node: 2
  Key: usr_7714 -> Tracked on Node: 2
Node [3] Sample Inventory Logs:
  Key: usr_99 -> Tracked on Node: 3
  Key: usr_443127 -> Tracked on Node: 3
  Key: usr_190011 -> Tracked on Node: 3

[Perf Trace] Complete Cluster Execution Duration: 1412 ms


Proof Analysis Confirmations
Even Data Balancing: Out of 491,150 unique post-deduplicated records, every node manages roughly ~122,800 rows, confirming a highly balanced key distribution.

Deterministic Ownership Proof: The data proof dump explicitly maps specific keys against the hash router utility, demonstrating that each item is physically stored on its designated node.
