// Enable OpenCL exceptions
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 300

#include <vector>   // For using std::vector
#include <iostream> // For standard input/output
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <utility>
#ifdef __APPLE__
#include <OpenCL/cl.hpp> // OpenCL 1.2 for macOS
#else
#include "CL/cl.hpp" // OpenCL for other platforms
#endif

#define PROGRAM_FILE "mathematic_operations.cl"

int main(int argc, char **argv)
{
    try
    {
        // Identify a platform
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        if (platforms.empty())
        {
            std::cerr << "Couldn't find any platforms" << std::endl;
            return EXIT_FAILURE;
        }

        // Access a device, preferably a GPU
        cl::Device device;
        for (auto &platform : platforms)
        {
            std::vector<cl::Device> devices;
            platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
            if (!devices.empty())
            {
                device = devices.back();
                break;
            }

            platform.getDevices(CL_DEVICE_TYPE_CPU, &devices);
            if (!devices.empty())
            {
                device = devices.front();
                break;
            }
        }

        if (!device())
        {
            std::cerr << "Couldn't find any devices in platform" << std::endl;
            return EXIT_FAILURE;
        }
        else
        {
            std::cout << "Device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        }

        // Create an OpenCL context
        cl::Context context(device);

        // Read the OpenCL program source code from file
        std::ifstream programFile(PROGRAM_FILE);
        if (!programFile.is_open())
        {
            std::cerr << "Error opening file \"" << PROGRAM_FILE << "\"" << std::endl;
            return EXIT_FAILURE;
        }

        std::string programBuffer((std::istreambuf_iterator<char>(programFile)), std::istreambuf_iterator<char>());
        programFile.close();

        // Create an OpenCL program from the source code
        cl::Program::Sources sources;
        sources.emplace_back(programBuffer.c_str(), programBuffer.length() + 1);
        cl::Program program(context, sources);

        // Build the OpenCL program
        const char options[] = "-cl-finite-math-only -cl-no-signed-zeros";
        program.build({device}, options);

        std::cout << "OpenCL program built successfully!" << std::endl;

        // Determine the number of kernels in the program
        std::vector<cl::Kernel> kernels;
        program.createKernels(&kernels);

        if (kernels.empty())
        {
            std::cerr << "Couldn't find any kernels" << std::endl;
            return EXIT_FAILURE;
        }

        // Print kernel function names
        for (auto &kernel : kernels)
        {
            std::string functionName = kernel.getInfo<CL_KERNEL_FUNCTION_NAME>();
            std::cout << "Function name: " << functionName << std::endl;
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