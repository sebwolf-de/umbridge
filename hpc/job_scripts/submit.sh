#!/bin/bash

#SBATCH --job-name=SeisSol-UQ   # Job name
#SBATCH --output=logs/SeisSol-UQ_%j.out # Name of stdout output file
#SBATCH --error=logs/SeisSol-UQ_%j.err  # Name of stderr error file
#SBATCH --partition=standard-g  # partition name
#SBATCH --nodes=3               # Total number of nodes
#SBATCH --ntasks-per-node=20    # 8 MPI ranks per node, 16 total (2x8)
#SBATCH --time=00:30:00       # Run time (d-hh:mm:ss)
#SBATCH --account=project_465000643
#SBATCH --mail-type=BEGIN,END
#SBATCH --mail-user=wolf.sebastian@cit.tum.de

export N=59
export M=1

cd $HOME/surrogate
python3 server.py --app seissol.sebastian&

cd $HOME/umbridge/hpc
rm -rf urls/*
rm -rf servers/*
srun ./load-server $N $M &
./load-client $N &

cd $HOME/mtmlda
python3 run.py --app seissol.sebastian
