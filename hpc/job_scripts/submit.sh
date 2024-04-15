#!/bin/bash

#SBATCH -J Umbridge-Server
#SBATCH -o logs/%x_%j.out
#SBATCH -e logs/%x_%j.err

#SBATCH -p normal
#SBATCH -A EAR23026

#SBATCH --nodes=301
#SBATCH -n 301

#SBATCH -D /home1/09160/sebwolf/UQ/umbridge/hpc

#SBATCH --mail-type=BEGIN,END,ARRAY_TASKS
#SBATCH --mail-user=wolf.sebastian@cit.tum.de

#SBATCH --time=20:00:00

export N=100
export M=3

module load python/3.9.2
module load gcc/9.1.0

rm -rf urls/*
rm -rf servers/*

# First start UQ client
cd $HOME/UQ/mtmlda/
python3 run.py --application seissol.sebastian &

# Then shart load-client
cd $HOME/UQ/umbridge/hpc
./load-client $N &

# Then start load-server
cd $HOME/UQ/umbridge/hpc
ibrun ./load-server $N $M
