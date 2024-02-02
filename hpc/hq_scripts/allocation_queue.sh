#! /bin/bash

# Note: For runs on systems without SLURM, replace the slurm allocator by
# hq worker start &


hq alloc add slurm --time-limit 10m \
                   --idle-timeout 3m \
                   --backlog 1 \
                   --workers-per-alloc 2 \
                   --max-worker-count 8 \
                   --resource "model=range(1-1)" \
                   --cpus=4 \
                   --no-dry-run \
                   -- -p "development" \
                      -A "EAR22007" \
                      -n 4 \
                      -N 4
# Any parameters after -- will be passed directly to sbatch (e.g. credentials, partition, mem, etc.)
