# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(getInfo)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source file
set(SRC getInfo.cpp)

# Check if building on macOS
if(APPLE)
    # Define a preprocessor macro
    add_definitions(-DAPPLE)
    # Link the OpenCL framework on macOS
    set(LIB ${LIB} "-framework OpenCL")
else() # GPU NVIDIA
    # Include directories (add as needed)
    set(INC ${INC} /usr/local/cuda/include)
    # Link directories for non-macOS
    link_directories(/usr/local/cuda/lib64)
    # Link the OpenCL library for non-macOS
    set(LIB ${LIB} OpenCL)
endif()

# Include directories (add as needed)
include_directories($INC)
# Add the executable target
add_executable(getInfo ${SRC})
# Link the OpenCL library
target_link_libraries(getInfo ${LIB})
