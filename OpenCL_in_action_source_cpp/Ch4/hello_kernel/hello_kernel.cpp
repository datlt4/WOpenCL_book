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

#ifdef __APPLE__
#include <OpenCL/cl.hpp> // OpenCL 1.2 for macOS using C++ bindings
#else
#include "CL/cl.hpp" // OpenCL for other platforms using C++ bindings
#endif

#define PROGRAM_FILE "hello_kernel.cl"

#define FLOAT_SIZE sizeof(float)
#define DATA_LEN_BYTES 100 * FLOAT_SIZE

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
        // Create OpenCL device
        cl::Device device = create_device();

        // Create OpenCL context
        cl::Context context(device);
        cl_int err;

        // Build OpenCL program
        cl::Program program = build_program(context, device, PROGRAM_FILE);

        cl::CommandQueue queue(context, device);

        std::vector<std::string> kernel_names{"hello_kernel_16", "hello_kernel_hi", "hello_kernel_lo", "hello_kernel_odd", "hello_kernel_even", "hello_kernel_s045e"};
        // std::vector<std::string> kernel_names{"hello_kernel_16"};
        for (std::string &kernel_name : kernel_names)
        {
            cl::Kernel kernel(program, kernel_name.c_str());
            char getting_started_msg[16];
            cl::Buffer getting_started_buffer(context, CL_MEM_WRITE_ONLY, sizeof(getting_started_msg), nullptr);
            CHECK_CL_ERROR(kernel.setArg(0, getting_started_buffer));
#ifdef APPLE
            CHECK_CL_ERROR(queue.enqueueTask(kernel));
#else
            cl::NDRange global_size(1024); // Adjust according to your kernel's requirements
            CHECK_CL_ERROR(queue.enqueueNDRangeKernel(kernel, cl::NullRange, global_size, cl::NullRange, nullptr, nullptr));
#endif // APPLE
            memset(getting_started_msg, 0, sizeof(getting_started_msg));
            CHECK_CL_ERROR(queue.enqueueReadBuffer(getting_started_buffer, CL_TRUE, 0, sizeof(getting_started_msg), getting_started_msg, nullptr, nullptr));
            std::cout << "Kernel: " << kernel_name << "\tMessage: \"" << getting_started_msg << "\"" << std::endl;
        }
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
