// Enable OpenCL exceptions
#define __CL_ENABLE_EXCEPTIONS

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

#define PROGRAM_FILE "data_transfer.cl" // Define the name of the kernel file

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
        program.build({device}, "-cl-finite-math-only -cl-no-signed-zeros");
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
        cl::Device device = create_device(); // Assuming create_device returns a cl::Device
        cl::Context context{device};
        cl::Program program = build_program(context, device, PROGRAM_FILE); // Function to build OpenCL program
        cl::CommandQueue queue{context, device};

        // Create kernel
        cl::Kernel kernel{program, KERNEL_NAME, &err}; // KERNEL_NAME is the name of your kernel function
        CHECK_CL_ERROR(err);                           // Macro or function to check OpenCL errors

        // Prepare data
        std::vector<float> check(24); // Vector to store data read from OpenCL buffer

        // Create buffer
        cl::Buffer check_buffer{context, CL_MEM_WRITE_ONLY, sizeof(float) * check.size()}; // Create OpenCL buffer
        CHECK_CL_ERROR(kernel.setArg(0, sizeof(check_buffer), &check_buffer));             // Set kernel argument

        // Define work dimensions
        cl::NDRange global_offset{3, 5}; // Global offset for NDRange kernel
        cl::NDRange global_size{6, 4};   // Global size (total number of work items)
        cl::NDRange local_size{3, 2};    // Local size (number of work items per work group)

        // Enqueue kernel execution
        CHECK_CL_ERROR(queue.enqueueNDRangeKernel(kernel, global_offset, global_size, local_size));

        // Read buffer into vector
        CHECK_CL_ERROR(queue.enqueueReadBuffer(check_buffer, CL_TRUE, 0, sizeof(float) * check.size(), check.data()));

        // Print the results
        for (int i = 0; i < 24; i += 6)
        {
            std::cout << std::fixed << std::setprecision(2) << std::setw(8)
                      << check[i] << "     " << check[i + 1] << "     "
                      << check[i + 2] << "     " << check[i + 3] << "     "
                      << check[i + 4] << "     " << check[i + 5] << std::endl;
        }
    }
    catch (cl::Error &e)
    {
        // Catch OpenCL exceptions
        std::cerr << "OpenCL error: " << e.what() << " (" << e.err() << ")" << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::exception &e)
    {
        // Catch standard exceptions
        std::cerr << "Standard exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        // Catch any other exceptions
        std::cerr << "Unknown error occurred" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
