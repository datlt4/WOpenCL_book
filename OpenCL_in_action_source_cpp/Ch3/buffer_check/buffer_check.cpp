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

#ifdef __APPLE__
#include <OpenCL/cl.hpp> // OpenCL 1.2 for macOS using C++ bindings
#else
#include "CL/cl.hpp" // OpenCL for other platforms using C++ bindings
#endif

/* Function to find a GPU or CPU associated with the first available platform */
cl::Device create_device()
{
    // Vector to store available platforms
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms); // Get available OpenCL platforms

    if (platforms.empty())
    {
        std::cerr << "Couldn't find any platforms" << std::endl;
        exit(EXIT_FAILURE);
    }

    cl::Device device;

    // Iterate through each platform to find GPU or CPU device
    for (auto &platform : platforms)
    {
        std::vector<cl::Device> devices;

        // Check for GPU devices first
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        if (!devices.empty())
        {
            device = devices.back(); // Choose the last GPU device
            break;
        }

        // If no GPU device found, check for CPU devices
        platform.getDevices(CL_DEVICE_TYPE_CPU, &devices);
        if (!devices.empty())
        {
            device = devices.front(); // Choose the first CPU device
            break;
        }
    }

    if (!device())
    {
        std::cerr << "Couldn't find any devices in platform" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    }

    return device;
}

/* Function to create OpenCL program from a file and compile it */
cl::Program build_program(cl::Context &context, cl::Device &device, const char *filename)
{
    // Read the OpenCL program source code from file
    std::ifstream programFile(filename);
    if (!programFile.is_open())
    {
        std::cerr << "Error opening file \"" << filename << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Read the program source into a string
    std::string programBuffer((std::istreambuf_iterator<char>(programFile)),
                              std::istreambuf_iterator<char>());
    programFile.close();

    // Create OpenCL program from the source code
    cl::Program::Sources sources;
    sources.emplace_back(programBuffer.c_str(), programBuffer.length() + 1);
    cl::Program program(context, sources);

    // Build the OpenCL program
    try
    {
        if (program.build({device}) != CL_SUCCESS)
        {
            std::cerr << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    catch (cl::Error &e)
    {
        std::cerr << "OpenCL error: " << e.what() << " (" << e.err() << ")" << std::endl;
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

        /* Example main data array */
        float main_data[100]; // Example: array of 100 floats

        cl_int err;
        // Create main buffer
        cl::Buffer main_buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                               sizeof(main_data), &main_data, &err);
        if (err != CL_SUCCESS)
        {
            std::cerr << "Couldn't create a buffer" << std::endl;
            exit(EXIT_FAILURE);
        }

        /* Create a sub-buffer */
        size_t origin = 5 * sizeof(float); // Example origin
        size_t size = 30 * sizeof(float);  // Example size

        // Check if size exceeds the parent buffer's size
        if (origin + size > sizeof(main_data))
        {
            printf("Error: Sub-buffer size exceeds parent buffer size\n");
            exit(EXIT_FAILURE);
        }

        // Check if size exceeds the parent buffer's size
        if (origin + size > sizeof(main_data))
        {
            std::cerr << "Error: Sub-buffer size exceeds parent buffer size" << std::endl;
            exit(EXIT_FAILURE);
        }

        cl_buffer_region region = {.origin = origin, .size = size};
        cl::Buffer sub_buffer = main_buffer.createSubBuffer(CL_MEM_READ_ONLY, CL_BUFFER_CREATE_TYPE_REGION, &region, &err);
        if (err != CL_SUCCESS)
        {
            std::cerr << "Couldn't create a sub-buffer, code: " << err << std::endl;
            exit(EXIT_FAILURE);
        }

        /* Obtain size information about the buffers */
        std::cout << "Main buffer size: " << main_buffer.getInfo<CL_MEM_SIZE>() << std::endl;
        std::cout << "Sub-buffer size: " << sub_buffer.getInfo<CL_MEM_SIZE>() << std::endl;

        /* Obtain the host pointers */
        std::cout << "Main buffer memory address: " << main_buffer.getInfo<CL_MEM_HOST_PTR>() << std::endl;
        std::cout << "Sub-buffer memory address: " << sub_buffer.getInfo<CL_MEM_HOST_PTR>() << std::endl;

        /* Print the address of the main data */
        std::cout << std::hex << "Main array address: " << main_data << std::endl;
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
