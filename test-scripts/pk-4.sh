#!/bin/bash
#
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=slow
#SBATCH --mem=5G

echo "parallel kruskals 250, 4 threads"
srun ./kruskal_parallel --inputFile ./graph_inputs/random_250.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_250.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_250.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_250.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_250.txt --nThreads 4

echo "parallel kruskals 500, 4 threads"
srun ./kruskal_parallel --inputFile ./graph_inputs/random_500.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_500.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_500.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_500.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_500.txt --nThreads 4

echo "parallel kruskals 1000, 4 threads"
srun ./kruskal_parallel --inputFile ./graph_inputs/random_1000.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_1000.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_1000.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_1000.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_1000.txt --nThreads 4

echo "parallel kruskals 2000, 4 threads"
srun ./kruskal_parallel --inputFile ./graph_inputs/random_2000.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_2000.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_2000.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_2000.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_2000.txt --nThreads 4

echo "parallel kruskals 4000, 4 threads"
srun ./kruskal_parallel --inputFile ./graph_inputs/random_4000.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_4000.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_4000.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_4000.txt --nThreads 4
srun ./kruskal_parallel --inputFile ./graph_inputs/random_4000.txt --nThreads 4
