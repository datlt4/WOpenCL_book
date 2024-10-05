# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(hello_kernel)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source files
set(CPP_SRC ${PROJECT_NAME}.cpp)
set(C_SRC ${PROJECT_NAME}.c)

# Check if building on macOS
if(APPLE)
    # Define a preprocessor macro
    add_definitions(-DAPPLE)
    # Link the OpenCL framework on macOS
    set(LIB ${LIB} "-framework OpenCL")
    message(STATUS "define APPLE")
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
    # Detect architecture
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
        # Define the CL_VERSION_2_x macro to indicate OpenCL 2.x support
        add_definitions(-DCL_VERSION_2_x)
        message(STATUS "Architecture is ARMv8, define CL_VERSION_2_x")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
        # Define the CL_VERSION_3_x macro to indicate OpenCL 3.x support
        add_definitions(-DCL_VERSION_3_x)
        message(STATUS "Architecture is x64, define CL_VERSION_3_x")
    else()
        message(STATUS "Unknown architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    endif()
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
