ifdef USE_INT
MACRO = -DUSE_INT
endif

#compiler setup
CXX = g++
CXXFLAGS = -std=c++14 -O3 -pthread $(MACRO)

COMMON= core/utils.h core/cxxopts.h core/get_time.h
SERIAL= spaning_tree_serial
PARALLEL= spanning_tree_parallel
ALL= $(SERIAL) $(PARALLEL)


all : $(ALL)

% : %.cpp $(COMMON)
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean :
	rm -f *.o *.obj $(ALL)