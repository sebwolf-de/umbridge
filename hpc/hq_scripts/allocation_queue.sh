#! /bin/bash

# Note: For runs on systems without SLURM, replace the slurm allocator by
# hq worker start &


hq alloc add slurm --time-limit 60m \
                   --idle-timeout 3m \
                   --backlog 1 \
                   --workers-per-alloc 4 \
                   --max-worker-count 4 \
                   -- -p "development" \
                      -A "EAR22007" \
                      --ntasks 8 \
                      --nodes 8
# Any parameters after -- will be passed directly to sbatch (e.g. credentials, partition, mem, etc.)
# ntasks == nodes!
# workers-per-alloc == max-worker-cound == ntasks / SeisSol-nodes
# Currently this works for 8 nodes, 4 workers, 2 nodes per worker
