#! /bin/bash

# Note: For runs on systems without SLURM, replace the slurm allocator by
# hq worker start &

hq alloc add slurm --time-limit 4h \
                   --idle-timeout 10m \
                   --backlog 2 \
                   --workers-per-alloc 8 \
                   -- -p "standard-g" \
                      --nodes 1 \
		      --gpus-per-node=8 \
		      --account="project_465000643"
# Any parameters after -- will be passed directly to sbatch (e.g. credentials, partition, mem, etc.)
# Note that the hq maintainers suggest to not use the `--nodes` argument, but on LUMI it is required, because of the non-flexible srun.
