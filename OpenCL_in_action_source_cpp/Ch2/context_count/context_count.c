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

    /* Create the context */
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS)
    {
        perror("Couldn't create a context");
        exit(EXIT_FAILURE);
    }

    /* Determine the reference count */
    cl_uint ref_cnt;
    err = clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(cl_uint), &ref_cnt, NULL);
    if (err != CL_SUCCESS)
    {
        perror("Couldn't read the reference count.");
        exit(EXIT_FAILURE);
    }
    printf("Initial reference count: %u\n", ref_cnt);

    /* Update and display the reference count */
    clRetainContext(context);
    clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(ref_cnt), &ref_cnt, NULL);
    printf("Reference count: %u\n", ref_cnt);

    clReleaseContext(context);
    clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(ref_cnt), &ref_cnt, NULL);
    printf("Reference count: %u\n", ref_cnt);

    /* Release context*/
    clReleaseContext(context);

    return EXIT_SUCCESS;
}
