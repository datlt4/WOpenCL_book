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

#define NUM_FILES 2
#define PROGRAM_FILE_1 "good.cl"
#define PROGRAM_FILE_2 "bad.cl"

int main(int argc, char **argv)
{
    /* Identify a platform */
    std::vector<cl::Platform> platforms;
    cl_int err = cl::Platform::get(&platforms);
    if (err != CL_SUCCESS)
    {
        std::cerr << "No platform found!" << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Access a device, preferably a GPU */
    std::vector<cl::Device> devices;
    err = platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if (err != CL_SUCCESS)
    {
        err = platforms[0].getDevices(CL_DEVICE_TYPE_CPU, &devices);
        if (err != CL_SUCCESS)
        {
            std::cerr << "No device found on platform!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    /* Create the context */
    cl::Context context(devices, nullptr, nullptr, nullptr, &err);
    if (err != CL_SUCCESS)
    {
        std::cout << "Couldn't create a context" << std::endl;
    }

    const std::vector<std::string> program_file_list = {PROGRAM_FILE_1, PROGRAM_FILE_2};
    std::string program_buffer;
    for (const std::string &filename : program_file_list)
    {
        // Reading from a file
        std::ifstream infile(filename);
        if (infile.is_open())
        {
            std::string line;
            while (std::getline(infile, line))
            {
                program_buffer += line + "\n"; // Append each line to program_buffer
            }
            infile.close(); // Close the file after reading
        }
        else
        {
            std::cerr << "Unable to open file for reading.\n";
        }
    }

    cl::Program::Sources sources;
    sources.emplace_back(program_buffer.c_str(), program_buffer.length() + 1);
    cl::Program program(context, sources, &err);
    // Build the program
    try
    {
        err = program.build();
    }
    catch (cl::Error)
    {
        err = -1;
        std::cout << "cl::Error" << std::endl;
    }

    if (err != CL_SUCCESS)
    {
        std::cerr << "Error building program: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "OpenCL program built successfully!" << std::endl;

    return EXIT_SUCCESS;
}
