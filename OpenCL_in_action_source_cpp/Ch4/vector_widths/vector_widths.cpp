// Enable OpenCL exceptions
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 300

// Standard C++ headers
#include <vector>    // For using std::vector
#include <iostream>  // For standard input/output
#include <string>    // For std::string operations
#include <fstream>   // For file input/output operations
#include <utility>   // For std::make_pair
#include <stdexcept> // For std::exception handling
#include <iomanip>   // Include this header for setw
#include <cstdio>

#ifdef __APPLE__
#include <OpenCL/cl.hpp> // OpenCL 1.2 for macOS using C++ bindings
#else
#include "CL/cl.hpp" // OpenCL for other platforms using C++ bindings
#endif

// Helper function to check OpenCL error codes
#define CHECK_CL_ERROR(err)                                                                    \
    if (err != CL_SUCCESS)                                                                     \
    {                                                                                          \
        std::cerr << "OpenCL error with code " << err << " at line " << __LINE__ << std::endl; \
        exit(EXIT_FAILURE);                                                                    \
    }

cl::Device create_device()
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty())
    {
        std::cerr << "No platforms found" << std::endl;
        exit(EXIT_FAILURE);
    }

    cl::Platform platform = platforms.front();
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if (devices.empty())
    {
        platform.getDevices(CL_DEVICE_TYPE_CPU, &devices);
        if (devices.empty())
        {
            std::cerr << "No devices found" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    ;
    std::cout << "Device: " << devices.front().getInfo<CL_DEVICE_NAME>() << std::endl;

    std::string version_str = devices.front().getInfo<CL_DEVICE_VERSION>();
    int major_version = 1; // default to OpenCL 1.x
    int minor_version = 2;
    if (sscanf(version_str.c_str(), "OpenCL %d.%d", &major_version, &minor_version) == 2)
    {
        std::cout << "OpenCL version: " << major_version << "." << minor_version << std::endl;
    }
    else
    {
        std::cerr << "Failed to parse OpenCL version" << std::endl;
    }

    return devices.front();
}

cl::Program build_program(const cl::Context &context, const cl::Device &device, const std::string &filename)
{
    std::ifstream program_file(filename);
    if (!program_file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string program_source((std::istreambuf_iterator<char>(program_file)),
                               std::istreambuf_iterator<char>());
    cl::Program program(context, program_source);

    try
    {
        std::string extensions = device.getInfo<CL_DEVICE_EXTENSIONS>();
        if (extensions.find("cl_khr_fp64", 0) != std::string::npos)
        {
            std::cout << "The `cl_khr_fp64` extension is supported." << std::endl;
            program.build({device}, "-cl-finite-math-only -cl-no-signed-zeros -DFP_64");
        }
        else
        {
            std::cout << "The `cl_khr_fp64` extension is not supported." << std::endl;
            program.build({device}, "-cl-finite-math-only -cl-no-signed-zeros");
        }
    }
    catch (const cl::Error &)
    {
        std::string build_log;
        program.getBuildInfo(device, CL_PROGRAM_BUILD_LOG, &build_log);
        std::cerr << "Build Log:\n"
                  << build_log << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "OpenCL program built successfully!" << std::endl;
    return program;
}

int main(int argc, char **argv)
{
    try
    {
        cl_int err;
        // Create OpenCL device
        cl::Device device = create_device();

        cl_device_fp_config flag = device.getInfo<CL_DEVICE_SINGLE_FP_CONFIG>(&err);
        CHECK_CL_ERROR(err);
        /* Obtain the device data */
        device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR>();
        std::cout << "Preferred vector width in chars: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR>() << std::endl;
        std::cout << "Preferred vector width in shorts: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT>() << std::endl;
        std::cout << "Preferred vector width in ints: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT>() << std::endl;
        std::cout << "Preferred vector width in longs: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG>() << std::endl;
        std::cout << "Preferred vector width in floats: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT>() << std::endl;
        std::cout << "Preferred vector width in doubles: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE>() << std::endl;

#ifndef MAC
        std::cout << "Preferred vector width in halfs: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF>() << std::endl;
#endif
    }
    catch (cl::Error &e)
    {
        std::cerr << "OpenCL error: " << e.what() << " (" << e.err() << ")" << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::exception &e)
    {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
