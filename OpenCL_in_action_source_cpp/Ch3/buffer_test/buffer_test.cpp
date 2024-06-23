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

#define PROGRAM_FILE "blank.cl"
#define KERNEL_NAME "blank"
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

int main()
{
    try
    {
        cl::Device device = create_device();
        cl::Context context(device);

        cl::Program program = build_program(context, device, PROGRAM_FILE);
        cl::Kernel kernel(program, KERNEL_NAME);

        float full_data[80];
        float zero_data[80];

        for (int i = 0; i < 80; ++i)
        {
            full_data[i] = i * 1.0f;
            zero_data[i] = 0.0f;
        }

        cl_int err;
        cl::Buffer full_buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(full_data), full_data, &err);
        CHECK_CL_ERROR(err);

        kernel.setArg(0, full_buffer);

        cl::CommandQueue queue(context, device);

#ifdef APPLE
        CHECK_CL_ERROR(queue.enqueueTask(kernel));
#else
        cl::NDRange global_size(100); // Adjust according to your kernel's requirements
        CHECK_CL_ERROR(queue.enqueueNDRangeKernel(kernel, cl::NullRange, global_size, cl::NullRange, nullptr, nullptr));
#endif // APPLE
        queue.enqueueWriteBuffer(full_buffer, CL_TRUE, 0, sizeof(full_data), full_data);

        cl::size_t<3> buffer_origin; // The (x in bytes, y in rows, z in slices) offset in the memory region associated with `buffer`
        buffer_origin[0] = 5 * FLOAT_SIZE;
        buffer_origin[1] = 3;
        buffer_origin[2] = 0;
        cl::size_t<3> host_origin; // The (x in bytes, y in rows, z in slices) offset in the memory region pointed to by `ptr`.
        host_origin[0] = 1 * FLOAT_SIZE;
        host_origin[1] = 1;
        host_origin[2] = 0;
        cl::size_t<3> region; // The (width in bytes, height in rows, depth in slices) of the 2D or 3D rectangle.
        region[0] = 4 * FLOAT_SIZE;
        region[1] = 4;
        region[2] = 1;
        queue.enqueueReadBufferRect(full_buffer, CL_TRUE, buffer_origin, host_origin, region, 10 * FLOAT_SIZE, 0, 10 * FLOAT_SIZE, 0, zero_data);

        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 10; ++j)
            {
                std::cout << std::setw(6) << zero_data[j + i * 10];
            }
            std::cout << std::endl;
        }

        queue.enqueueReadBuffer(full_buffer, CL_TRUE, 0, sizeof(zero_data), zero_data);

        std::cout << std::endl;
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 10; ++j)
            {
                std::cout << std::setw(6) << zero_data[j + i * 10];
            }
            std::cout << std::endl;
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

    return 0;
}
