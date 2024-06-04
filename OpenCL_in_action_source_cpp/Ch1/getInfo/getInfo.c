// Enable OpenCL exceptions and disable secure warnings in Visual Studio

// #define __NO_STD_STRING
#define __CL_ENABLE_EXCEPTIONS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>     // For standard input/output functions
#include <stdlib.h>    // For memory allocation functions
#include <sys/types.h> // For defining data types

#ifdef __APPLE__
#include <OpenCL/cl.h> // Include OpenCL headers for macOS
#else
#include <CL/cl.h> // Include OpenCL headers for other platforms
#endif

int main(int argc, char **argv)
{
    cl_platform_id *platforms; // Pointer to hold the list of platform IDs
    cl_uint num_platforms;     // Variable to store the number of platforms

    // Get the number of available OpenCL platforms (maximum 5)
    clGetPlatformIDs(5, NULL, &num_platforms);

    printf("NUMBER OF OPENCL PLATFORMS: %i\n", num_platforms);

    // Allocate memory to store the platform IDs
    platforms = malloc(num_platforms * sizeof(cl_platform_id));

    // Get the platform IDs and store them in the allocated memory
    clGetPlatformIDs(num_platforms, platforms, NULL);

    for (int i = 0; i < num_platforms; ++i)
    {
        printf("+ Platform %i\n", i);
        char platform_info_char[1000];
        size_t platform_info_char_len;
        cl_int err;

// Define a macro to reduce redundancy when fetching platform info
#define GET_PLATFORM_INFO(param)                                                                                           \
    err = clGetPlatformInfo(platforms[i], param, sizeof(platform_info_char), platform_info_char, &platform_info_char_len); \
    if (err != CL_SUCCESS)                                                                                                 \
    {                                                                                                                      \
        fprintf(stderr, "\t- Error getting " #param ": %d\n", err);                                                        \
    }                                                                                                                      \
    else                                                                                                                   \
    {                                                                                                                      \
        printf("\t- " #param ": %s (%lu)\n", platform_info_char, platform_info_char_len);                                  \
    }

        // Retrieve and print platform info
        GET_PLATFORM_INFO(CL_PLATFORM_PROFILE);
        GET_PLATFORM_INFO(CL_PLATFORM_VERSION);
        GET_PLATFORM_INFO(CL_PLATFORM_NAME);
        GET_PLATFORM_INFO(CL_PLATFORM_VENDOR);
        GET_PLATFORM_INFO(CL_PLATFORM_EXTENSIONS);
#undef GET_PLATFORM_INFO

        cl_uint num_devices;
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 32, NULL, &num_devices);
        printf("\n\t- [ DEVICE ] NUMBER OF DEVICES IN PLATFORM: %i\n", num_devices);
        cl_device_id *devices = malloc(num_devices * sizeof(cl_device_id));
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 32, devices, NULL);

        for (int j = 0; j < num_devices; ++j)
        {
            char device_info_char[1000];
            size_t device_info_char_len;
            printf("\n\t\t. [ DEVICE ] %d\n", j);

#define GET_DEVICE_INFO(param)                                                                                                  \
    {                                                                                                                           \
        clGetDeviceInfo(devices[j], param, sizeof(device_info_char), device_info_char, &device_info_char_len);                  \
        switch (param)                                                                                                          \
        {                                                                                                                       \
        case CL_DEVICE_AVAILABLE:                                                                                               \
        case CL_DEVICE_COMPILER_AVAILABLE:                                                                                      \
        case CL_DEVICE_ENDIAN_LITTLE:                                                                                           \
        case CL_DEVICE_ERROR_CORRECTION_SUPPORT:                                                                                \
        case CL_DEVICE_IMAGE_SUPPORT:                                                                                           \
        case CL_DEVICE_LINKER_AVAILABLE:                                                                                        \
        case CL_DEVICE_PREFERRED_INTEROP_USER_SYNC:                                                                             \
            printf("\t\t. %s: %s (%lu)\n", #param, *((cl_bool *)device_info_char) ? "True" : "False", device_info_char_len);    \
            break;                                                                                                              \
        case CL_DEVICE_BUILT_IN_KERNELS:                                                                                        \
        case CL_DEVICE_EXTENSIONS:                                                                                              \
        case CL_DEVICE_NAME:                                                                                                    \
        case CL_DEVICE_OPENCL_C_VERSION:                                                                                        \
        case CL_DEVICE_PROFILE:                                                                                                 \
        case CL_DEVICE_VENDOR:                                                                                                  \
            printf("\t\t. %s: %s (%lu)\n", #param, device_info_char, device_info_char_len);                                     \
            break;                                                                                                              \
        case CL_DEVICE_IMAGE2D_MAX_HEIGHT:                                                                                      \
        case CL_DEVICE_IMAGE2D_MAX_WIDTH:                                                                                       \
        case CL_DEVICE_IMAGE3D_MAX_DEPTH:                                                                                       \
        case CL_DEVICE_IMAGE3D_MAX_HEIGHT:                                                                                      \
        case CL_DEVICE_IMAGE3D_MAX_WIDTH:                                                                                       \
        case CL_DEVICE_IMAGE_MAX_ARRAY_SIZE:                                                                                    \
        case CL_DEVICE_IMAGE_MAX_BUFFER_SIZE:                                                                                   \
        case CL_DEVICE_MAX_PARAMETER_SIZE:                                                                                      \
        case CL_DEVICE_MAX_WORK_GROUP_SIZE:                                                                                     \
        case CL_DEVICE_PRINTF_BUFFER_SIZE:                                                                                      \
        case CL_DEVICE_PROFILING_TIMER_RESOLUTION:                                                                              \
            printf("\t\t. %s: %lu (%lu)\n", #param, *((size_t *)device_info_char), device_info_char_len);                       \
            break;                                                                                                              \
        case CL_DEVICE_GLOBAL_MEM_CACHE_SIZE:                                                                                   \
        case CL_DEVICE_GLOBAL_MEM_SIZE:                                                                                         \
        case CL_DEVICE_LOCAL_MEM_SIZE:                                                                                          \
        case CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:                                                                                \
        case CL_DEVICE_MAX_MEM_ALLOC_SIZE:                                                                                      \
            printf("\t\t. %s: %llu (%lu)\n", #param, *((cl_ulong *)device_info_char), device_info_char_len);                    \
            break;                                                                                                              \
        case CL_DEVICE_ADDRESS_BITS:                                                                                            \
        case CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE:                                                                               \
        case CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT:                                                                            \
        case CL_DEVICE_IMAGE_PITCH_ALIGNMENT:                                                                                   \
        case CL_DEVICE_MAX_CLOCK_FREQUENCY:                                                                                     \
        case CL_DEVICE_MAX_COMPUTE_UNITS:                                                                                       \
        case CL_DEVICE_MAX_CONSTANT_ARGS:                                                                                       \
        case CL_DEVICE_MAX_READ_IMAGE_ARGS:                                                                                     \
        case CL_DEVICE_MAX_SAMPLERS:                                                                                            \
        case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:                                                                                \
        case CL_DEVICE_MAX_WRITE_IMAGE_ARGS:                                                                                    \
        case CL_DEVICE_MEM_BASE_ADDR_ALIGN:                                                                                     \
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR:                                                                                \
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT:                                                                               \
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_INT:                                                                                 \
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG:                                                                                \
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT:                                                                               \
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE:                                                                              \
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF:                                                                                \
        case CL_DEVICE_PARTITION_MAX_SUB_DEVICES:                                                                               \
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:                                                                             \
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:                                                                            \
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT:                                                                              \
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG:                                                                             \
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:                                                                            \
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:                                                                           \
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF:                                                                             \
        case CL_DEVICE_REFERENCE_COUNT:                                                                                         \
            printf("\t\t. %s: %u (%lu)\n", #param, *((cl_uint *)device_info_char), device_info_char_len);                       \
            break;                                                                                                              \
        case CL_DEVICE_TYPE:                                                                                                    \
            switch (*((cl_device_type *)device_info_char))                                                                      \
            {                                                                                                                   \
            case CL_DEVICE_TYPE_DEFAULT:                                                                                        \
                printf("\t\t. %s: %s (%lu)\n", #param, "CL_DEVICE_TYPE_DEFAULT", device_info_char_len);                         \
                break;                                                                                                          \
            case CL_DEVICE_TYPE_CPU:                                                                                            \
                printf("\t\t. %s: %s (%lu)\n", #param, "CL_DEVICE_TYPE_DEFAULT", device_info_char_len);                         \
                break;                                                                                                          \
            case CL_DEVICE_TYPE_GPU:                                                                                            \
                printf("\t\t. %s: %s (%lu)\n", #param, "CL_DEVICE_TYPE_DEFAULT", device_info_char_len);                         \
                break;                                                                                                          \
            case CL_DEVICE_TYPE_ACCELERATOR:                                                                                    \
                printf("\t\t. %s: %s (%lu)\n", #param, "CL_DEVICE_TYPE_DEFAULT", device_info_char_len);                         \
                break;                                                                                                          \
            case CL_DEVICE_TYPE_CUSTOM:                                                                                         \
                printf("\t\t. %s: %s (%lu)\n", #param, "CL_DEVICE_TYPE_DEFAULT", device_info_char_len);                         \
                break;                                                                                                          \
            default:                                                                                                            \
                printf("\t\t. %s: %s (%lu)\n", #param, "UNKNOWN", device_info_char_len);                                        \
                break;                                                                                                          \
            }                                                                                                                   \
            break;                                                                                                              \
        case CL_DEVICE_PLATFORM:                                                                                                \
            printf("\t\t. %s: %p (%lu)\n", #param, (cl_platform_id *)device_info_char, device_info_char_len);                   \
            break;                                                                                                              \
        case CL_DEVICE_GLOBAL_MEM_CACHE_TYPE:                                                                                   \
            printf("\t\t. %s: %u (%lu)\n", #param, *((cl_device_mem_cache_type *)device_info_char), device_info_char_len);      \
            break;                                                                                                              \
        case CL_DEVICE_LOCAL_MEM_TYPE:                                                                                          \
            printf("\t\t. %s: %u (%lu)\n", #param, *((cl_device_local_mem_type *)device_info_char), device_info_char_len);      \
            break;                                                                                                              \
        case CL_DEVICE_PARENT_DEVICE:                                                                                           \
            printf("\t\t. %s: %p (%lu)\n", #param, (cl_device_id *)device_info_char, device_info_char_len);                     \
            break;                                                                                                              \
        case CL_DEVICE_SINGLE_FP_CONFIG:                                                                                        \
        case CL_DEVICE_DOUBLE_FP_CONFIG:                                                                                        \
            printf("\t\t. %s: %llu (%lu)\n", #param, *((cl_device_fp_config *)device_info_char), device_info_char_len);         \
            break;                                                                                                              \
        case CL_DEVICE_EXECUTION_CAPABILITIES:                                                                                  \
            printf("\t\t. %s: %llu (%lu)\n", #param, *((cl_device_exec_capabilities *)device_info_char), device_info_char_len); \
            break;                                                                                                              \
        case CL_DEVICE_PARTITION_AFFINITY_DOMAIN:                                                                               \
            printf("\t\t. %s: %llu (%lu)\n", #param, *((cl_device_type *)device_info_char), device_info_char_len);              \
            break;                                                                                                              \
        default:                                                                                                                \
            printf("\t\t. %s: Not support for querying\n", #param);                                                             \
            break;                                                                                                              \
        }                                                                                                                       \
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
            GET_DEVICE_INFO(CL_DEVICE_PARENT_DEVICE);
            GET_DEVICE_INFO(CL_DEVICE_AVAILABLE);
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
            // GET_DEVICE_INFO(CL_DEVICE_IMAGE_MAX_ARRAY_SIZE);
            // GET_DEVICE_INFO(CL_DEVICE_IMAGE_MAX_BUFFER_SIZE);
            // GET_DEVICE_INFO(CL_DEVICE_IMAGE_PITCH_ALIGNMENT);
            GET_DEVICE_INFO(CL_DEVICE_IMAGE_SUPPORT);
            // GET_DEVICE_INFO(CL_DEVICE_LINKER_AVAILABLE);
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
            // GET_DEVICE_INFO(CL_DEVICE_PARTITION_MAX_SUB_DEVICES);
            // GET_DEVICE_INFO(CL_DEVICE_PARTITION_PROPERTIES);
            // GET_DEVICE_INFO(CL_DEVICE_PARTITION_TYPE);
            GET_DEVICE_INFO(CL_DEVICE_PLATFORM);
            // GET_DEVICE_INFO(CL_DEVICE_PRINTF_BUFFER_SIZE);
            GET_DEVICE_INFO(CL_DEVICE_PROFILE);
            GET_DEVICE_INFO(CL_DEVICE_PROFILING_TIMER_RESOLUTION);
            GET_DEVICE_INFO(CL_DEVICE_REFERENCE_COUNT);
            GET_DEVICE_INFO(CL_DEVICE_SINGLE_FP_CONFIG);
#undef GET_DEVICE_INFO
        }
    }

    // Clean up: free the allocated memory for platform IDs
    free(platforms);

    // End of the program
    return 0;
}
