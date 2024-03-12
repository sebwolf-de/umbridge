#!/bin/bash
# Note: For runs on systems without SLURM, replace the slurm allocator by
# hq worker start &


### We will run N SeisSol models in parallel, each on M nodes.
### This setup will run SeisSol with one rank per node, be carful to set 
### OMP_NUM_THREADS=55, OMP_PLACES="cores(55)" in the SeisBridge server.
export N=3
export M=3
export n_nodes=$(expr $N \* $M)

hq alloc add slurm --time-limit 120m \
                   --idle-timeout 5m \
                   --backlog 3 \
                   --workers-per-alloc $n_nodes \
                   --max-worker-count $n_nodes \
                   -- -p "development" \
                      -A "EAR23026" \
                      --ntasks $n_nodes \
                      --nodes $n_nodes
# Any parameters after -- will be passed directly to sbatch (e.g. credentials, partition, mem, etc.)
