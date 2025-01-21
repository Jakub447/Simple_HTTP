# Main Makefile

# Variables
SRC_DIR := src
LIB_DIR := liblogger
BUILD_DIR := $(abspath build)
TARGET := $(BUILD_DIR)/HTTPServer

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -fPIC -g   # -fPIC needed for shared libraries
LDFLAGS := -L$(BUILD_DIR) -lHTTPServer -llogger -Wl,-rpath=${BUILD_DIR} # Link to the program and logger libraries

# Dynamic library targets
PROGRAM_SO := $(BUILD_DIR)/libHTTPServer.so
LOGGER_SO := $(BUILD_DIR)/liblogger.so

.PHONY: all clean $(SRC_DIR) $(LIB_DIR)

# Default target: Build everything
all: $(BUILD_DIR) $(LOGGER_SO) $(PROGRAM_SO) $(TARGET)

# Ensure the build directory exists
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Build the program library and logger library
$(LOGGER_SO):
	@echo "Building logger library..."
	$(MAKE) -C $(LIB_DIR) BUILD_DIR=$(BUILD_DIR)

$(PROGRAM_SO):
	@echo "Building program library..."
	$(MAKE) -C $(SRC_DIR) BUILD_DIR=$(BUILD_DIR)

# Compile main.cpp and link it with the dynamic libraries
$(TARGET): main.cpp $(PROGRAM_SO) $(LOGGER_SO)
	@echo "Compiling main.cpp and linking to libraries..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp $(LDFLAGS)

# Clean up all build artifacts
clean:
	@echo "Cleaning up..."
	$(MAKE) -C $(SRC_DIR) clean
	$(MAKE) -C $(LIB_DIR) clean
	rm -rf $(BUILD_DIR)/*.so $(TARGET)