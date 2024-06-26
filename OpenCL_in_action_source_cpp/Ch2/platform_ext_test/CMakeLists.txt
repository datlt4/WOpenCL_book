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
    set(CUDA_TOOLKIT_ROOT_DIR /usr/local/cuda)
    set(CUDA_INCLUDE_DIRS ${CUDA_TOOLKIT_ROOT_DIR}/include)
    set(CUDA_LIB_DIRS ${CUDA_TOOLKIT_ROOT_DIR}/lib64)
    # Include directories for non-macOS
    include_directories(${CUDA_INCLUDE_DIRS})
    # Link directories for non-macOS
    link_directories(${CUDA_LIB_DIRS})
    # Find the OpenCL library for non-macOS
    find_library(OpenCL_LIBRARY OpenCL PATHS ${CUDA_LIB_DIRS})
    set(LIB ${OpenCL_LIBRARY})
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

# Copy each .cl file to the destination directory
file(GLOB CL_FILES "${CMAKE_SOURCE_DIR}/*.cl")
foreach(CL_FILE ${CL_FILES})
    file(COPY ${CL_FILE} DESTINATION ${CMAKE_BINARY_DIR})
endforeach()
