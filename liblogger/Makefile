# lib/Makefile

# Variables
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -fPIC -g
LIB_FILES := $(wildcard *.cpp)
OBJ_FILES := $(LIB_FILES:.cpp=.o)
#BUILD_DIR := ../build
TARGET := $(BUILD_DIR)/liblogger.so

# Print BUILD_DIR for debugging
$(info BUILD_DIR is $(BUILD_DIR))

.PHONY: all clean

# Default target: Compile library files and create a shared library
all: $(TARGET)

# Compile each .cpp file into an object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create a shared library from object files
$(TARGET): $(OBJ_FILES)
	@echo "Creating shared library $(TARGET)..."
	$(CXX) -shared -o $(TARGET) $(OBJ_FILES)

# Clean up object files and shared library
clean:
	@echo "Cleaning up liblogger directory..."
	rm -f $(OBJ_FILES) $(TARGET)