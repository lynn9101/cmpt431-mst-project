#!/bin/bash
#
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=slow
#SBATCH --mem=5G

echo "parallel kruskals 250, 8 threads"
srun ./kruskal_parallel --inputFile ./graph_inputs/random_250.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_250.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_250.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_250.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_250.txt --nThreads 8

echo "parallel kruskals 500, 8 threads"
srun ./kruskal_parallel --inputFile ./graph_inputs/random_500.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_500.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_500.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_500.txt --nThreads 8

echo "parallel kruskals 1000, 8 threads"
srun ./kruskal_parallel --inputFile ./graph_inputs/random_1000.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_1000.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_1000.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_1000.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_1000.txt --nThreads 8

echo "parallel kruskals 2000, 8 threads"
srun ./kruskal_parallel --inputFile ./graph_inputs/random_2000.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_2000.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_2000.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_2000.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_2000.txt --nThreads 8

echo "parallel kruskals 4000, 8 threads"
srun ./kruskal_parallel --inputFile ./graph_inputs/random_4000.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_4000.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_4000.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_4000.txt --nThreads 8
srun ./kruskal_parallel --inputFile ./graph_inputs/random_4000.txt --nThreads 8

