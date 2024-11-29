# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17

# Target executable name
TARGET = wifi.exe

# Source file
SRC = wifi.cpp

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Run the program
run: $(TARGET)
	$(TARGET)

# Clean up generated files
clean:
	del /q *.exe

# Phony targets (not associated with actual files)
.PHONY: all run clean
