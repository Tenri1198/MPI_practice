# Makefile for Sieve of Eratosthenes using MPI with C++

# Compiler
MPICXX = mpicxx

# Executable name
TARGET = sieve

# Source file
SRCS = sieve.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Compiler flags
CXXFLAGS = -Wall -O2

# Libraries
LIBS = -lm

# Number of processes (default is 4)
PROCESSES ?= 4

# Upper limit for the sieve algorithm, required as a command line argument for the program
UPPER_LIMIT ?= 1000000

# The build target executable:
all: $(TARGET)

$(TARGET): $(OBJS)
	$(MPICXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

.cpp.o:
	$(MPICXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	mpirun -np $(PROCESSES) ./$(TARGET) $(UPPER_LIMIT)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean
