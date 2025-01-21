# src/Makefile

# Variables
CXX := g++
CXXFLAGS := -std=c++20 -Wall -fPIC -g
SRC_FILES := $(wildcard *.cpp)
OBJ_FILES := $(SRC_FILES:.cpp=.o)
#BUILD_DIR ?= ../build
TARGET := $(BUILD_DIR)/libHTTPServer.so

# Link with the logger library
LDFLAGS := -L$(BUILD_DIR) -llogger -lssl -lcrypto -Wl,-rpath=${BUILD_DIR}

.PHONY: all clean

# Default target: Compile source files and create a shared library
all: $(TARGET)

# Compile each .cpp file into an object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create a shared library from object files
$(TARGET): $(OBJ_FILES)
	@echo "Creating shared library $(TARGET)..."
	$(CXX) -shared -o $(TARGET) $(OBJ_FILES) $(LDFLAGS)

# Clean up object files and shared library
clean:
	@echo "Cleaning up src directory..."
	rm -f $(OBJ_FILES) $(TARGET)