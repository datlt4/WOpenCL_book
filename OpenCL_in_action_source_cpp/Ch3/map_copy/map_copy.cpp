// Enable OpenCL exceptions
#define __CL_ENABLE_EXCEPTIONS

// Standard C++ headers
#include <vector>    // For using std::vector
#include <iostream>  // For standard input/output
#include <string>    // For std::string operations
#include <fstream>   // For file input/output operations
#include <utility>   // For std::make_pair
#include <stdexcept> // For std::exception handling

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
        cl::Kernel kernel(program, KERNEL_NAME);

        /* Data and buffers */
        float data_one[100], data_two[100], result_array[100];

        /* Initialize arrays */
        for (int i = 0; i < 100; i++)
        {
            data_one[i] = 1.0f * i;
            data_two[i] = -1.0f * i;
            result_array[i] = 0.0f;
        }

        // Create OpenCL buffers
        cl::Buffer buffer_one(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(data_one), data_one, &err);
        CHECK_CL_ERROR(err);
        cl::Buffer buffer_two(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(data_two), data_two, &err);
        CHECK_CL_ERROR(err);

        // Set kernel arguments
        kernel.setArg(0, buffer_one);
        kernel.setArg(1, buffer_two);

        // Create command queue
        cl::CommandQueue queue(context, 0, &err);
        CHECK_CL_ERROR(err);

        // Enqueue kernel execution
        queue.enqueueTask(kernel);

        // Read buffer_one back to result_array and print
        CHECK_CL_ERROR(queue.enqueueReadBuffer(buffer_one, CL_TRUE, 0, DATA_LEN_BYTES, result_array));
        std::cout << std::endl;
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 10; ++j)
            {
                std::cout << std::setw(6) << result_array[j + i * 10];
            }
            std::cout << std::endl;
        }

        // Read buffer_two back to result_array and print
        CHECK_CL_ERROR(queue.enqueueReadBuffer(buffer_two, CL_TRUE, 0, DATA_LEN_BYTES, result_array));
        std::cout << std::endl;
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 10; ++j)
            {
                std::cout << std::setw(6) << result_array[j + i * 10];
            }
            std::cout << std::endl;
        }

        // Perform buffer copy operation using clEnqueueCopyBufferRect
        cl::size_t<3> src_origin; // The (x in bytes, y in rows, z in slices) offset in the memory region associated with `buffer_one`
        src_origin[0] = 1 * FLOAT_SIZE;
        src_origin[1] = 1;
        src_origin[2] = 0;
        cl::size_t<3> dst_origin; // The (x in bytes, y in rows, z in slices) offset in the memory region pointed to by `buffer_two`
        dst_origin[0] = 1 * FLOAT_SIZE;
        dst_origin[1] = 1;
        dst_origin[2] = 0;
        cl::size_t<3> region; // The (width in bytes, height in rows, depth in slices) of the 2D or 3D rectangle
        region[0] = 4 * FLOAT_SIZE;
        region[1] = 4;
        region[2] = 1;
        CHECK_CL_ERROR(queue.enqueueCopyBufferRect(buffer_one, buffer_two, src_origin, dst_origin, region, 10 * FLOAT_SIZE, 0, 10 * FLOAT_SIZE, 0, nullptr, nullptr));

        // Map buffer_two, perform memcpy, and unmap
        void *mapped_memory = queue.enqueueMapBuffer(buffer_two, CL_TRUE, CL_MAP_WRITE, 0, DATA_LEN_BYTES, nullptr, nullptr, &err);
        CHECK_CL_ERROR(err);
        memcpy((void *)result_array, mapped_memory, DATA_LEN_BYTES);
        /**
         * When you map a buffer, it provides a snapshot of the buffer's state at that moment.
         * If you modify the buffer on the device after mapping it, those changes might not be
         * immediately visible in the mapped host memory unless you unmap and remap the buffer.
         */
        CHECK_CL_ERROR(queue.enqueueUnmapMemObject(buffer_two, mapped_memory));
        std::cout << std::endl;
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 10; ++j)
            {
                std::cout << std::setw(6) << result_array[j + i * 10];
            }
            std::cout << std::endl;
        }

        // Perform another buffer copy operation
        src_origin[0] = 1 * FLOAT_SIZE;
        src_origin[1] = 1;
        src_origin[2] = 0;
        dst_origin[0] = 5 * FLOAT_SIZE;
        dst_origin[1] = 5;
        dst_origin[2] = 0;
        region[0] = 4 * FLOAT_SIZE;
        region[1] = 4;
        region[2] = 1;
        CHECK_CL_ERROR(queue.enqueueCopyBufferRect(buffer_one, buffer_two, src_origin, dst_origin, region, 10 * FLOAT_SIZE, 0, 10 * FLOAT_SIZE, 0, nullptr, nullptr));

        // Map buffer_two, perform memcpy, and unmap again
        mapped_memory = queue.enqueueMapBuffer(buffer_two, CL_TRUE, CL_MAP_WRITE, 0, DATA_LEN_BYTES, nullptr, nullptr, &err);
        CHECK_CL_ERROR(err);
        memcpy((void *)result_array, mapped_memory, DATA_LEN_BYTES);
        CHECK_CL_ERROR(queue.enqueueUnmapMemObject(buffer_two, mapped_memory));
        std::cout << std::endl;
        for (int i = 0; i < 10; ++i)
        {
            for (int j = 0; j < 10; ++j)
            {
                std::cout << std::setw(6) << result_array[j + i * 10];
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

    return EXIT_SUCCESS;
}
