#!/bin/bash

# Note: For runs on systems without SLURM, replace the slurm allocator by
# hq worker start &


### We will run N SeisSol models in parallel, each on M nodes.
### This setup will run SeisSol with one rank per node, be carful to set 
### OMP_NUM_THREADS=55, OMP_PLACES="cores(55)" in the SeisBridge server.
export N=2
export M=4
export n_nodes=$(expr $N \* $M)

hq alloc add slurm --time-limit 60m \
                   --idle-timeout 55m \
                   --backlog 1 \
                   --workers-per-alloc $N \
                   --max-worker-count $N \
                   -- -p "development" \
                      -A "EAR23026" \
                      --ntasks $n_nodes \
                      --nodes $n_nodes
# Any parameters after -- will be passed directly to sbatch (e.g. credentials, partition, mem, etc.)
