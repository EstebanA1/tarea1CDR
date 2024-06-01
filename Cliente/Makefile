# Makefile for the client

# Compiler
CXX = g++

# Flags
CXXFLAGS = -std=c++11 -Wall

# Target executable
TARGET = cliente

# Source files
SRCS = cliente.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default rule
all: $(TARGET)

# Rule to link the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile the source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove the compiled files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
