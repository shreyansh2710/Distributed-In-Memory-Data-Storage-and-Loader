#!/bin/bash
# Enable safe abort flags
set -e

echo "=== Step 1: Parsing schema from config.txt ==="
NODES=$(grep -E '^nodes=' config.txt | cut -d= -f2)
echo "Detected Target Cluster Node Count: $NODES"

echo "=== Step 2: Generating 100MB+ Mock Storage Splits Per Node ==="
for ((i=0; i<NODES; i++))
do
  FILE="mock_node_${i}_data.csv"
  echo "Generating $FILE..."
  
  # Python baseline block generator ensuring file sizing properties match requirements
  python3 -c "
import random
with open('$FILE', 'w') as f:
    # 2.3 Million records generates roughly 105 Megabytes of tabular string raw lines
    for item in range(2300000):
        # Programmatic injection of structured key repetitions to challenge deduplicators
        key_id = f'usr_{random.randint(1, 500000)}'
        age = random.randint(18, 80)
        score = random.randint(300, 990)
        f.write(f'{key_id},Alice_Data_Field_Value_Placeholder_Padding_String,{age},{score}\n')
"
  ls -lh "$FILE"
done

echo "=== Step 3: Compiling Modular Targets ==="
mkdir -p build && cd build
cmake ..
make

echo "=== Step 4: Launching Distributed System Clusters ==="
./distributed_storage_node

echo "=== Step 5: Post Run File System Maintenance Cleanups ==="
cd ..
rm -f mock_node_*_data.csv
echo "System cleared successfully."
