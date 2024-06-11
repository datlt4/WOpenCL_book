# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(platform_ext_test)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source files
set(CPP_SRC platform_ext_test.cpp)
set(C_SRC platform_ext_test.c)

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

# Add the executable target for the C++ application
add_executable(${PROJECT_NAME}.cpp_app ${CPP_SRC})
# Link the OpenCL library for the C++ application
target_link_libraries(${PROJECT_NAME}.cpp_app ${LIB})

# Add the executable target for the C application
add_executable(${PROJECT_NAME}.c_app ${C_SRC})
# Link the OpenCL library for the C application
target_link_libraries(${PROJECT_NAME}.c_app ${LIB})
