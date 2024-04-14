CXX = g++
CXXFLAGS = -std=c++17 -g -O0

# Object files
ALLOBJ = rng.o dndSim.o testSuite.o
OBJ = $(filter-out dndSim.o, $(ALLOBJ))

# Executable name
EXEC = testSuite

# Default target
all: $(EXEC)

# Link the test suite executable
$(EXEC): rng.o dndSim.o testSuite.o
	$(CXX) $(CXXFLAGS) -o $(EXEC) testSuite.o dndSim.o rng.o

# Compile the rng library
rng.o: rng.cpp
	$(CXX) $(CXXFLAGS) -c rng.cpp

# Compile the dndSim library
dndSim.o: dndSim.cpp
	$(CXX) $(CXXFLAGS) -c dndSim.cpp

# Compile the test suite
testSuite.o: testSuite.cpp
	$(CXX) $(CXXFLAGS) -c testSuite.cpp

# Clean up
clean:
	rm -f $(OBJ) $(EXEC)

cleanall:
	rm -f $(ALLOBJ) $(EXEC)

# Parallel build target
parallel: CXXFLAGS += -fopenmp
parallel: $(EXEC)