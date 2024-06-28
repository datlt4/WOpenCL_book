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

#define PROGRAM_FILE "op_test.cl" // Define the name of the kernel file

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

        cl::Kernel kn_op_test1{program, "op_test1", &err};
        CHECK_CL_ERROR(err);
        int vector[4];
        cl::Buffer vec_buffer1{context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(vector), &err};

        kn_op_test1.setArg(0, sizeof(vec_buffer1), &vec_buffer1);
#ifdef APPLE
        CHECK_CL_ERROR(queue.enqueueTask(kn_op_test1));
#else
        cl::NDRange global_size(1024); // Adjust according to your kernel's requirements
        CHECK_CL_ERROR(queue.enqueueNDRangeKernel(kn_op_test1, cl::NullRange, global_size, cl::NullRange, nullptr, nullptr));
#endif // APPLE
        memset((void *)vector, 0, sizeof(vector));
        CHECK_CL_ERROR(queue.enqueueReadBuffer(vec_buffer1, CL_TRUE, 0, 16, vector));
        std::cout << std::hex << "Ouput: [0]: " << vector[0] << ", [1]: " << vector[1] << ", [2]: " << vector[2] << ", [3]:" << vector[3] << std::endl;

        cl::Kernel kn_op_test2{program, "op_test2", &err};
        CHECK_CL_ERROR(err);
        cl::Buffer vec_buffer2{context, CL_MEM_READ_WRITE, sizeof(vector), nullptr, &err};
        CHECK_CL_ERROR(err);
        CHECK_CL_ERROR(kn_op_test2.setArg(0, sizeof(vec_buffer2), &vec_buffer2));
        CHECK_CL_ERROR(queue.enqueueCopyBuffer(vec_buffer1, vec_buffer2, 0, 0, sizeof(vector)));
#ifdef APPLE
        CHECK_CL_ERROR(queue.enqueueTask(kn_op_test2));
#else
        CHECK_CL_ERROR(queue.enqueueNDRangeKernel(kn_op_test2, cl::NullRange, global_size, cl::NullRange, nullptr, nullptr));
#endif // APPLE
memset((void *)vector, 0, sizeof(vector));
queue.enqueueReadBuffer(vec_buffer2, CL_TRUE, 0, sizeof(vector), vector);
std::cout << std::hex << "Ouput: [0]: " << vector[0] << ", [1]: " << vector[1] << ", [2]: " << vector[2] << ", [3]:" << vector[3] << std::endl;


      cl::Kernel kn_op_test3{program, "op_test3", &err};
        CHECK_CL_ERROR(err);
        cl::Buffer vec_buffer3{context, CL_MEM_READ_WRITE, sizeof(vector), nullptr, &err};
        CHECK_CL_ERROR(err);
        CHECK_CL_ERROR(kn_op_test3.setArg(0, sizeof(vec_buffer3), &vec_buffer3));
        CHECK_CL_ERROR(queue.enqueueCopyBuffer(vec_buffer2, vec_buffer3, 0, 0, sizeof(vector)));
#ifdef APPLE
        CHECK_CL_ERROR(queue.enqueueTask(kn_op_test3));
#else
        CHECK_CL_ERROR(queue.enqueueNDRangeKernel(kn_op_test3, cl::NullRange, global_size, cl::NullRange, nullptr, nullptr));
#endif // APPLE
memset((void *)vector, 0, sizeof(vector));
queue.enqueueReadBuffer(vec_buffer3, CL_TRUE, 0, sizeof(vector), vector);
std::cout << std::hex << "Ouput: [0]: " << vector[0] << ", [1]: " << vector[1] << ", [2]: " << vector[2] << ", [3]:" << vector[3] << std::endl;


      cl::Kernel kn_op_test4{program, "op_test4", &err};
        CHECK_CL_ERROR(err);
        cl::Buffer vec_buffer4{context, CL_MEM_READ_WRITE, sizeof(vector), nullptr, &err};
        CHECK_CL_ERROR(err);
        CHECK_CL_ERROR(kn_op_test4.setArg(0, sizeof(vec_buffer4), &vec_buffer4));
        CHECK_CL_ERROR(queue.enqueueCopyBuffer(vec_buffer3, vec_buffer4, 0, 0, sizeof(vector)));
#ifdef APPLE
        CHECK_CL_ERROR(queue.enqueueTask(kn_op_test4));
#else
        CHECK_CL_ERROR(queue.enqueueNDRangeKernel(kn_op_test4, cl::NullRange, global_size, cl::NullRange, nullptr, nullptr));
#endif // APPLE
memset((void *)vector, 0, sizeof(vector));
queue.enqueueReadBuffer(vec_buffer4, CL_TRUE, 0, sizeof(vector), vector);
std::cout << std::hex << "Ouput: [0]: " << vector[0] << ", [1]: " << vector[1] << ", [2]: " << vector[2] << ", [3]:" << vector[3] << std::endl;


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
