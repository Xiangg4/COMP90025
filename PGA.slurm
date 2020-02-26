#!/bin/bash
#SBATCH --time=0:04:00
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=2


module load OpenMPI/1.10.2-GCC-4.9.2
mpicc PGA.c -o PGA
mpiexec -n 4 ./PGA