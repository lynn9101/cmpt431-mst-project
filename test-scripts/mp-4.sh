#!/bin/bash
#
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks=4
#SBATCH --partition=slow
#SBATCH --mem=5G

echo "mpi prims 250, 4 processes"
srun ./prim_mpi --inputFile ./graph_inputs/random_250.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_250.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_250.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_250.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_250.txt

echo "mpi prims 500, 4 processes"
srun ./prim_mpi --inputFile ./graph_inputs/random_500.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_500.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_500.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_500.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_500.txt

echo "mpi prims 1000, 4 processes"
srun ./prim_mpi --inputFile ./graph_inputs/random_1000.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_1000.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_1000.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_1000.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_1000.txt

echo "mpi prims 2000, 4 processes"
srun ./prim_mpi --inputFile ./graph_inputs/random_2000.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_2000.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_2000.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_2000.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_2000.txt

echo "mpi prims 4000, 4 processes"
srun ./prim_mpi --inputFile ./graph_inputs/random_4000.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_4000.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_4000.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_4000.txt
srun ./prim_mpi --inputFile ./graph_inputs/random_4000.txt
