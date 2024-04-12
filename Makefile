ifdef USE_INT
MACRO = -DUSE_INT
endif

#compiler setup
CXX = g++
MPICXX = mpic++
CXXFLAGS = -std=c++14 -O3 -pthread $(MACRO)

COMMON= core/utils.h core/graph.h core/edge.h core/cxxopts.h core/get_time.h
SCRIPT= graph_generator
SERIAL= spanning_tree_serial
PARALLEL= spanning_tree_parallel
MPI= spanning_tree_mpi
ALL= $(SCRIPT) $(SERIAL) $(PARALLEL) $(MPI)


all : $(ALL)

$(MPI): %: %.cpp
	$(MPICXX) $(CXXFLAGS) -o $@ $<

# $(SERIAL): %: %.cpp
# 	$(CXX) $(CXXFLAGS) -o $@ $<
% : %.cpp $(COMMON)
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean :
	rm -f *.o *.obj $(ALL)