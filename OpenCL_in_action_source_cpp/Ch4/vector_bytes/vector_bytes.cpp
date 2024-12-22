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

#define PROGRAM_FILE "vector_bytes.cl" // Define the name of the kernel file
#define KERNEL_NAME "vector_bytes"     // Define the default kernel name

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
        cl::Device device = create_device();

        cl::Context context{device};
        cl::Program program = build_program(context, device, PROGRAM_FILE);
        cl::CommandQueue queue{context, device};

        cl::Kernel kernel{program, KERNEL_NAME, &err};
        CHECK_CL_ERROR(err);
        u_char bytes[16];
        cl::Buffer bytes_buffer{context, CL_MEM_WRITE_ONLY, 16};
        kernel.setArg(0, sizeof(bytes_buffer), &bytes_buffer);
#ifdef APPLE
        CHECK_CL_ERROR(queue.enqueueTask(kernel));
#else
        cl::NDRange global_size(1024); // Adjust according to your kernel's requirements
        CHECK_CL_ERROR(queue.enqueueNDRangeKernel(kernel, cl::NullRange, global_size, cl::NullRange, nullptr, nullptr));
        CHECK_CL_ERROR(queue.enqueueReadBuffer(bytes_buffer, CL_TRUE, 0, 16, bytes));
        std::cout << std::hex << "Bytes: 0x" << (u_int)bytes[0] << "  0x" << (u_int)bytes[1] << "  0x" << (u_int)bytes[2] << "  0x" << (u_int)bytes[3]
                  << "  0x" << (u_int)bytes[4] << "  0x" << (u_int)bytes[5] << "  0x" << (u_int)bytes[6] << "  0x" << (u_int)bytes[7] << "  0x" << (u_int)bytes[8]
                  << "  0x" << (u_int)bytes[9] << "  0x" << (u_int)bytes[10] << "  0x" << (u_int)bytes[11] << "  0x" << (u_int)bytes[12] << "  0x" << (u_int)bytes[13]
                  << "  0x" << (u_int)bytes[14] << "  0x" << (u_int)bytes[15] << std::endl;
#endif // APPLE
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
