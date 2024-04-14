ifdef USE_INT
MACRO = -DUSE_INT
endif

#compiler setup
CXX = g++
CXXFLAGS = -std=c++14 -O3 -pthread $(MACRO)

COMMON= core/utils.h core/graph.h core/edge.h core/cxxopts.h core/get_time.h
SCRIPT= graph_generator
SERIAL= kruskal_serial boruvka_serial
PARALLEL= kruskal_parallel 
ALL= $(SCRIPT) $(SERIAL) $(PARALLEL)


all : $(ALL)

# $(SERIAL): %: %.cpp
# 	$(CXX) $(CXXFLAGS) -o $@ $<
% : %.cpp $(COMMON)
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean :
	rm -f *.o *.obj $(ALL)