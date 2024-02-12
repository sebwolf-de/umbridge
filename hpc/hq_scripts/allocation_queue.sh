#! /bin/bash

# Note: For runs on systems without SLURM, replace the slurm allocator by
# hq worker start &


### TODO
### We will run N SeisSol models in parallel, each on M nodes.
### This setup will run SeisSol with one rank per node, be carful to set 
### OMP_NUM_THREADS=55, OMP_PLACES="cores(55)" in the SeisBridge server.
export N=20
export M=144
export n_nodes=$(expr $N *$M)

### TODO Fill in the name of the queue and the project below

hq alloc add slurm --time-limit 180m \
                   --idle-timeout 175m \
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
