#!/bin/bash
#
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=slow
#SBATCH --mem=5G

echo "serial kruskals 250"
srun ./boruvka_serial --inputFile ./graph_inputs/random_250.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_250.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_250.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_250.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_250.txt

echo "serial boruvka 500"
srun ./boruvka_serial --inputFile ./graph_inputs/random_500.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_500.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_500.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_500.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_500.txt

echo "serial boruvka 1000"
srun ./boruvka_serial --inputFile ./graph_inputs/random_1000.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_1000.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_1000.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_1000.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_1000.txt

echo "serial boruvka 2000"
srun ./boruvka_serial --inputFile ./graph_inputs/random_2000.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_2000.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_2000.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_2000.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_2000.txt

echo "serial boruvka 4000"
srun ./boruvka_serial --inputFile ./graph_inputs/random_4000.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_4000.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_4000.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_4000.txt
srun ./boruvka_serial --inputFile ./graph_inputs/random_4000.txt
