#!/bin/bash
#
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=slow
#SBATCH --mem=5G

echo "serial kruskal 250"
srun ./kruskal_serial --inputFile ./graph_inputs/random_250.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_250.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_250.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_250.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_250.txt

echo "serial kruskal 500"
srun ./kruskal_serial --inputFile ./graph_inputs/random_500.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_500.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_500.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_500.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_500.txt

echo "serial kruskal 1000"
srun ./kruskal_serial --inputFile ./graph_inputs/random_1000.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_1000.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_1000.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_1000.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_1000.txt

echo "serial kruskal 2000"
srun ./kruskal_serial --inputFile ./graph_inputs/random_2000.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_2000.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_2000.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_2000.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_2000.txt

echo "serial kruskal 4000"
srun ./kruskal_serial --inputFile ./graph_inputs/random_4000.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_4000.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_4000.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_4000.txt
srun ./kruskal_serial --inputFile ./graph_inputs/random_4000.txt
