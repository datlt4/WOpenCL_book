// Enable OpenCL exceptions
#define __CL_ENABLE_EXCEPTIONS

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

#define PROGRAM_FILE "blank.cl"

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
        cl::Program::Sources sources(1, std::make_pair(programBuffer.c_str(), programBuffer.length() + 1));
        cl::Program program(context, sources);
        // Build the OpenCL program
        if (program.build({device}) != CL_SUCCESS)
        {
            std::cerr << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "OpenCL program built successfully!" << std::endl;

        // Create kernels
        std::vector<cl::Kernel> kernels;
        program.createKernels(&kernels);
        if (kernels.empty())
        {
            std::cerr << "Couldn't find any kernels" << std::endl;
            return EXIT_FAILURE;
        }

        // Print kernel function names
        std::string functionName = kernels.front().getInfo<CL_KERNEL_FUNCTION_NAME>();
        std::cout << "Kernel name: " << functionName << std::endl;

        // Create command queue
        cl::CommandQueue queue(context, device);
        if (queue.enqueueTask(kernels.front()) != CL_SUCCESS)
        {
            std::cerr << "Couldn't enqueue the kernel execution command" << std::endl;
        }
        else
        {
            std::cout << "Function " << functionName << " was enqueued to command queue." << std::endl;
            queue.finish(); // Ensure the kernel execution completes
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
