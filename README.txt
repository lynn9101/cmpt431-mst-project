# General Instructions to Run Program
1. Sample input graph with 10 vertices is under ```./graph_inputs/random_10.txt```

-> If you want to generate your own input graph, run:
```./graph_generator --nVertices <num vertices>```
Default number of vertices is set to 10.

2. Run serial codes:
```./kruskal_serial --inputFile <path to the graph input>```
```./boruvka_serial --inputFile <path to the graph input>```
For example,
```./kruskal_serial --inputFile ./graph_inputs/random_10.txt```

3. Run parallel C++ thread code:
```./kruskal_parallel --nThreads 4  --inputFile <path to the graph input>```
Default number of threads is set to 2.

4. Run distrubuted code:
```./prim_mpi --inputFile <path to the graph input>```