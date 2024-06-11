// Enable OpenCL exceptions and disable secure warnings in Visual Studio

// #define __NO_STD_STRING
#define __CL_ENABLE_EXCEPTIONS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>     // For standard input/output functions
#include <stdlib.h>    // For memory allocation functions
#include <sys/types.h> // For defining data types
#include <inttypes.h>  // This header provides the correct macros for printing fixed-width integer types.
#include <string.h>

#ifdef __APPLE__
#include <OpenCL/cl.h> // Include OpenCL headers for macOS
#else
#include <CL/cl.h> // Include OpenCL headers for other platforms
#endif

int main(int argc, char **argv)
{
    /* Host data structures */
    cl_platform_id *platforms;
    cl_uint num_platforms;
    cl_int i, err, platform_index = -1;

    /* Extension data */
    char *ext_data;
    size_t ext_size;
    const char icd_ext[] = "cl_khr_icd";

    /* Find number of platforms */
    err = clGetPlatformIDs(1, NULL, &num_platforms);
    if (err < 0)
    {
        perror("Couldn't find any platforms.");
        exit(EXIT_FAILURE);
    }

    /* Access all installed platforms */
    platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id) * num_platforms);
    clGetPlatformIDs(num_platforms, platforms, NULL);

    /* Find extensions of all platforms */
    for (i = 0; i < num_platforms; i++)
    {

        /* Find size of extension data */
        err = clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, 0, NULL, &ext_size);
        if (err < 0)
        {
            perror("Couldn't read extension data.");
            exit(EXIT_FAILURE);
        }

        /* Access extension data */
        ext_data = (char *)malloc(ext_size);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, ext_size, ext_data, NULL);
        printf("Platform %d supports extensions: %s\n", i, ext_data);

        /* Look for ICD extension */
        if (strstr(ext_data, icd_ext) != NULL)
        {
            free(ext_data);
            platform_index = i;
            break;
        }
        free(ext_data);
    }

    /* Display whether ICD extension is supported */
    if (platform_index > -1)
    {
        printf("Platform %d supports the %s extension.\n", platform_index, icd_ext);
    }
    else
    {
        printf("No platforms support the %s extension.\n", icd_ext);
    }

    /* Deallocate resources */
    free(platforms);
    return EXIT_SUCCESS;
}
