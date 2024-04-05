ifdef USE_INT
MACRO = -DUSE_INT
endif

#compiler setup
CXX = g++
CXXFLAGS = -std=c++14 -O3 -pthread $(MACRO)

COMMON= core/utils.h core/graph.h core/edge.h
SERIAL= spanning_tree_serial
PARALLEL= spanning_tree_parallel
ALL= $(SERIAL) $(PARALLEL)


all : $(ALL)

# $(SERIAL): %: %.cpp
# 	$(CXX) $(CXXFLAGS) -o $@ $<
% : %.cpp $(COMMON)
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean :
	rm -f *.o *.obj $(ALL)