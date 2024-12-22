// Enable OpenCL exceptions
// Set OpenCL version to 2.0
#define CL_HPP_TARGET_OPENCL_VERSION 200
// #define CL_HPP_TARGET_OPENCL_VERSION 300
// #define __CL_ENABLE_EXCEPTIONS
#define CL_HPP_ENABLE_EXCEPTIONS

#include <vector>   // For using std::vector
#include <iostream> // For standard input/output
#ifdef __APPLE__
#include <OpenCL/cl.hpp> // OpenCL 1.2 for macOS
#else
#include "CL/cl.hpp" // OpenCL for other platforms
#endif

int main(int argc, char **argv)
{
    try
    {
        // Create a vector to store the available OpenCL platforms
        std::vector<cl::Platform> platforms;

        // Get all available OpenCL platforms and store them in the vector
        cl::Platform::get(&platforms);

        if (platforms.empty())
        {
            std::cerr << "No platform found!" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Output the number of platforms found
        std::cout << "NUMBER OF OPENCL PLATFORMS: " << platforms.size() << std::endl;

        unsigned int platform_counter = 0;

        for (cl::Platform &platform : platforms)
        {
            std::cout << "+ Platform " << platform_counter++ << std::endl;

            // Error code for platform info retrieval
            cl_int ec;

            // Define a macro to reduce redundancy when fetching platform info

#define GET_PLATFORM_INFO(param)                                                    \
    {                                                                               \
        std::string param_info = platform.getInfo<param>(&ec);                      \
        if (ec == CL_SUCCESS)                                                       \
        {                                                                           \
            std::cout << "\t- " << #param << ": " << param_info << std::endl;       \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            std::cerr << "\t- Error getting " << #param << ": " << ec << std::endl; \
        }                                                                           \
    }

            // Retrieve and print platform info
            GET_PLATFORM_INFO(CL_PLATFORM_PROFILE);
            GET_PLATFORM_INFO(CL_PLATFORM_VERSION);
            GET_PLATFORM_INFO(CL_PLATFORM_NAME);
            GET_PLATFORM_INFO(CL_PLATFORM_VENDOR);
            GET_PLATFORM_INFO(CL_PLATFORM_EXTENSIONS);
#undef GET_PLATFORM_INFO

            std::vector<cl::Device> devices;
            platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
            if (devices.empty())
            {
                std::cerr << "No devices found on platform" << std::endl;
                exit(EXIT_FAILURE);
            }
            cl_uint device_counter = 0;
            std::cout << "\n\t- [ DEVICE ] NUMBER OF DEVICES IN PLATFORM: " << devices.size() << std::endl;
            for (cl::Device &device : devices)
            {
                std::cout << "\n\t\t. [ DEVICE ] " << device_counter++ << std::endl;

#define GET_DEVICE_INFO(param)                                                                                                                     \
    {                                                                                                                                              \
        auto param_info = device.getInfo<param>(&ec);                                                                                              \
        if (ec == CL_SUCCESS)                                                                                                                      \
        {                                                                                                                                          \
            if (param == CL_DEVICE_TYPE)                                                                                                           \
            {                                                                                                                                      \
                std::cout << "\t\t. " << #param << ": " << param_info << " (" << CL_DEVICE_TYPE_DEFAULT << " : CL_DEVICE_TYPE_DEFAULT, "           \
                          << CL_DEVICE_TYPE_CPU << " : CL_DEVICE_TYPE_CPU, " << CL_DEVICE_TYPE_GPU << " : CL_DEVICE_TYPE_GPU, "                    \
                          << CL_DEVICE_TYPE_ACCELERATOR << " : CL_DEVICE_TYPE_ACCELERATOR" << CL_DEVICE_TYPE_CUSTOM << " : CL_DEVICE_TYPE_CUSTOM, " \
                          << "other : UNKNOWN)" << std::endl;                                                                                       \
            }                                                                                                                                      \
            else                                                                                                                                   \
            {                                                                                                                                      \
                std::cout << "\t\t. " << #param << ": " << param_info << std::endl;                                                                \
            }                                                                                                                                      \
        }                                                                                                                                          \
        else                                                                                                                                       \
        {                                                                                                                                          \
            std::cerr << "\t\t. Error getting " << #param << ": " << ec << std::endl;                                                              \
        }                                                                                                                                          \
    }

                // Retrieve and print device info
                GET_DEVICE_INFO(CL_DEVICE_TYPE);
                GET_DEVICE_INFO(CL_DEVICE_VENDOR);
                GET_DEVICE_INFO(CL_DEVICE_VENDOR_ID);
                GET_DEVICE_INFO(CL_DEVICE_VERSION);
                GET_DEVICE_INFO(CL_DRIVER_VERSION);
                GET_DEVICE_INFO(CL_DEVICE_OPENCL_C_VERSION);
                GET_DEVICE_INFO(CL_DEVICE_ADDRESS_BITS);
                GET_DEVICE_INFO(CL_DEVICE_NAME);
                // GET_DEVICE_INFO(CL_DEVICE_PARENT_DEVICE);
                // GET_DEVICE_INFO(CL_DEVICE_AVAILABLE);
                GET_DEVICE_INFO(CL_DEVICE_BUILT_IN_KERNELS);
                GET_DEVICE_INFO(CL_DEVICE_COMPILER_AVAILABLE);
                GET_DEVICE_INFO(CL_DEVICE_DOUBLE_FP_CONFIG);
                GET_DEVICE_INFO(CL_DEVICE_ENDIAN_LITTLE);
                GET_DEVICE_INFO(CL_DEVICE_ERROR_CORRECTION_SUPPORT);
                GET_DEVICE_INFO(CL_DEVICE_EXECUTION_CAPABILITIES);
                GET_DEVICE_INFO(CL_DEVICE_EXTENSIONS);
                GET_DEVICE_INFO(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
                GET_DEVICE_INFO(CL_DEVICE_GLOBAL_MEM_CACHE_TYPE);
                GET_DEVICE_INFO(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
                GET_DEVICE_INFO(CL_DEVICE_GLOBAL_MEM_SIZE);
                GET_DEVICE_INFO(CL_DEVICE_IMAGE2D_MAX_HEIGHT);
                GET_DEVICE_INFO(CL_DEVICE_IMAGE2D_MAX_WIDTH);
                GET_DEVICE_INFO(CL_DEVICE_IMAGE3D_MAX_DEPTH);
                GET_DEVICE_INFO(CL_DEVICE_IMAGE3D_MAX_HEIGHT);
                GET_DEVICE_INFO(CL_DEVICE_IMAGE3D_MAX_WIDTH);
                GET_DEVICE_INFO(CL_DEVICE_IMAGE_MAX_ARRAY_SIZE);
                GET_DEVICE_INFO(CL_DEVICE_IMAGE_MAX_BUFFER_SIZE);
                GET_DEVICE_INFO(CL_DEVICE_IMAGE_PITCH_ALIGNMENT);
                GET_DEVICE_INFO(CL_DEVICE_IMAGE_SUPPORT);
                GET_DEVICE_INFO(CL_DEVICE_LINKER_AVAILABLE);
                GET_DEVICE_INFO(CL_DEVICE_LOCAL_MEM_SIZE);
                GET_DEVICE_INFO(CL_DEVICE_LOCAL_MEM_TYPE);
                GET_DEVICE_INFO(CL_DEVICE_MAX_CLOCK_FREQUENCY);
                GET_DEVICE_INFO(CL_DEVICE_MAX_COMPUTE_UNITS);
                GET_DEVICE_INFO(CL_DEVICE_MAX_CONSTANT_ARGS);
                GET_DEVICE_INFO(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);
                GET_DEVICE_INFO(CL_DEVICE_MAX_MEM_ALLOC_SIZE);
                GET_DEVICE_INFO(CL_DEVICE_MAX_PARAMETER_SIZE);
                GET_DEVICE_INFO(CL_DEVICE_MAX_READ_IMAGE_ARGS);
                GET_DEVICE_INFO(CL_DEVICE_MAX_SAMPLERS);
                GET_DEVICE_INFO(CL_DEVICE_MAX_WORK_GROUP_SIZE);
                GET_DEVICE_INFO(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
                // GET_DEVICE_INFO(CL_DEVICE_MAX_WORK_ITEM_SIZES);
                GET_DEVICE_INFO(CL_DEVICE_MAX_WRITE_IMAGE_ARGS);
                GET_DEVICE_INFO(CL_DEVICE_MEM_BASE_ADDR_ALIGN);
                GET_DEVICE_INFO(CL_DEVICE_PARTITION_AFFINITY_DOMAIN);
                GET_DEVICE_INFO(CL_DEVICE_PARTITION_MAX_SUB_DEVICES);
                // GET_DEVICE_INFO(CL_DEVICE_PARTITION_PROPERTIES);
                // GET_DEVICE_INFO(CL_DEVICE_PARTITION_TYPE);
                // GET_DEVICE_INFO(CL_DEVICE_PLATFORM);
                GET_DEVICE_INFO(CL_DEVICE_PRINTF_BUFFER_SIZE);
                GET_DEVICE_INFO(CL_DEVICE_PROFILE);
                GET_DEVICE_INFO(CL_DEVICE_PROFILING_TIMER_RESOLUTION);
                GET_DEVICE_INFO(CL_DEVICE_REFERENCE_COUNT);
                GET_DEVICE_INFO(CL_DEVICE_SINGLE_FP_CONFIG);
#undef GET_DEVICE_INFO
                // Create context
                cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform(), 0};
                cl::Context context(devices, properties, nullptr, nullptr, &ec);
                // cl::Context context(devices, nullptr, nullptr, nullptr, &ec);
                if (ec != CL_SUCCESS)
                {
                    std::cout << "\t\t[ERROR] couldn't create the context with this device" << std::endl;
                }
                else
                {
                    std::cout << "\t\t[SUCCESS] Create the context successfully!" << std::endl;
                }

                // cl_context context = clCreateContext(NULL, 1, &device(), NULL, NULL, &ec);

                // Use the context for OpenCL operations...
                // Context will be automatically released when it goes out of scope

                /** Devices are typically managed implicitly through contexts.
                 * When you create a context, you specify the devices it will use.
                 * The context manages the devices internally, and releasing the context properly cleans up the associated device resources.
                 * Therefore, you don't need to explicitly release devices in most cases.
                 * // clReleaseDevice(devices[j]); [Unnecessary]
                 */
            }
        }
    }
    catch (const cl::Error &e) // Catch OpenCL specific exceptions
    {
        // Print OpenCL error message and error code
        std::cout << "OpenCL Error: " << e.what() << ": Error code " << e.err() << std::endl;
    }
    catch (const std::exception &e) // Catch standard exceptions
    {
        // Print standard error message
        std::cerr << "Standard Error: " << e.what() << '\n';
    }

    // Return 0 to indicate successful execution
    return 0;
}
