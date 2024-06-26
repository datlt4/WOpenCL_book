// Enable OpenCL exceptions and disable secure warnings in Visual Studio

// #define __NO_STD_STRING
#define __CL_ENABLE_EXCEPTIONS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>     // For standard input/output functions
#include <stdlib.h>    // For memory allocation functions
#include <sys/types.h> // For defining data types
#include <inttypes.h>  // This header provides the correct macros for printing fixed-width integer types.

#ifdef __APPLE__
#include <OpenCL/cl.h> // Include OpenCL headers for macOS
#else
#include <CL/cl.h> // Include OpenCL headers for other platforms
#endif

int main(int argc, char **argv)
{
    /* Identify a platform */
    cl_platform_id platform;
    cl_int err;
    err = clGetPlatformIDs(1, &platform, NULL);

    if (err != CL_SUCCESS)
    {
        perror("Couldn't find any platforms");
        exit(EXIT_FAILURE);
    }

    /* Access a device, preferably a GPU */
    cl_device_id device;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err != CL_SUCCESS)
    {
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
        if (err != CL_SUCCESS)
        {
            perror("Couldn't find any devices in platform");
            exit(EXIT_FAILURE);
        }
    }

    /* Access device name */
    char device_name[100];
    err = clGetDeviceInfo(device, CL_DEVICE_NAME, 100, device_name, NULL);
    if (err != CL_SUCCESS)
    {
        perror("Couldn't read extension data");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("NAME: %s\n", device_name);
    }

    /* Access device address size */
    cl_uint address_bit;
    clGetDeviceInfo(device, CL_DEVICE_ADDRESS_BITS, sizeof(address_bit), &address_bit, NULL);
    printf("ADDRESS_WIDTH: %i\n", address_bit);

    /* Access device extensions */
    char extensions_data[4096];
    clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, 4096 * sizeof(char), extensions_data, NULL);
    printf("EXTENSIONS: %s\n", extensions_data);

    return EXIT_SUCCESS;
}
