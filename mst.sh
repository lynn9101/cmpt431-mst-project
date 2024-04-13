#!/bin/bash
#
#SBATCH --cpus-per-task=1
#SBATCH --time=02:00
#SBATCH --mem=1G
#SBATCH --partition=slow

srun ./spanning_tree_mpi --inputFile=./graph_inputs/random_10.txt