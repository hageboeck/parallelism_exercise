CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Object files
OBJ = dndSim.o testSuite.o

# Executable name
EXEC = testSuite

# Default target
all: $(EXEC)

# Link the test suite executable
$(EXEC): dndSim.o testSuite.o
	$(CXX) $(CXXFLAGS) -o $(EXEC) testSuite.o dndSim.o

# Compile the dndSim library
dndSim.o: dndSim.cpp
	$(CXX) $(CXXFLAGS) -c dndSim.cpp

# Compile the test suite
testSuite.o: testSuite.cpp
	$(CXX) $(CXXFLAGS) -c testSuite.cpp

# Clean up
clean:
	rm -f $(OBJ) $(EXEC)

