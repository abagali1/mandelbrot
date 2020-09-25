#!/bin/bash
#SBATCH --output=zoom.log
#SBATCH --nodes=1           ### Number of Nodes
#SBATCH --ntasks-per-node=1 ### Number of tasks (MPI processes)
#SBATCH --cpus-per-task=32  ### Number of threads per task (OMP threads)
 
export OMP_NUM_THREADS=$(nproc)
 
./a.out
