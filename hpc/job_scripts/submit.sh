#!/bin/bash

#SBATCH -J Umbridge-Server
#SBATCH -o logs/%x_%j.out
#SBATCH -e logs/%x_%j.err

#SBATCH -p normal
#SBATCH -A EAR23026

#SBATCH --nodes=61
#SBATCH -n 61

#SBATCH -D /home1/09160/sebwolf/UQ/umbridge/hpc/build

#SBATCH --mail-type=BEGIN,END,ARRAY_TASKS
#SBATCH --mail-user=wolf.sebastian@cit.tum.de

#SBATCH --time=02:00:00

module load python/3.9.2
module load gcc/9.1.0

rm -rf urls/*
rm -rf servers/*

# First start UQ client
cd $HOME/UQ/mtmlda/
python3 run.py --application seissol.sebastian &

# Then shart load-client
cd $HOME/UQ/umbridge/hpc
./load-client 20 &

# Then start load-server
cd $HOME/UQ/umbridge/hpc
ibrun ./load-server 20 3
