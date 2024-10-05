// Enable OpenCL exceptions
#define __CL_ENABLE_EXCEPTIONS

#include <vector>   // For using std::vector
#include <iostream> // For standard input/output
#include <string>
#ifdef __APPLE__
#include <OpenCL/cl.hpp> // OpenCL 1.2 for macOS
#else
#include "CL/cl.hpp" // OpenCL for other platforms
#endif

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

    /* Access device name */
    std::string device_name;
    err = devices.back().getInfo(CL_DEVICE_NAME, &device_name);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Couldn't read extension data" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "NAME: " << device_name << std::endl;
    }

    /* Access device address size */
    cl_uint address_bit;
    err = devices.back().getInfo(CL_DEVICE_ADDRESS_BITS, &address_bit);
    std::cout << "ADDRESS_WIDTH: " << address_bit << std::endl;

    /* Access device extensions */
    std::string extensions_data;
    err = devices.back().getInfo(CL_DEVICE_EXTENSIONS, &extensions_data);
    std::cout << "EXTENSIONS: " << extensions_data << std::endl;

    return EXIT_SUCCESS;
}
