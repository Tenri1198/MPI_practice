# Makefile for MPI gather/scatter matrix-vector multiplication

# Compiler
MPICXX = mpic++

# Flags
CXXFLAGS = -O2 -Wall -Wextra

# Executable name
# TARGET = mpi_matrix_vector
TARGET = circular
# Source file
#SRCS = MPI_grouping.cpp
SRCS = circular.cpp

# Default target
all: $(TARGET)

# Compile the source file
$(TARGET): $(SRCS)
	$(MPICXX) $(CXXFLAGS) -o $@ $^

# Run the executable with 4 processes
run: $(TARGET)
	mpirun -np 8 ./$(TARGET)

# Clean up
clean:
	rm -f $(TARGET)

.PHONY: all run clean