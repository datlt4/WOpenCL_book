// Enable OpenCL exceptions and disable secure warnings in Visual Studio

// #define __NO_STD_STRING
#define __CL_ENABLE_EXCEPTIONS  // Enable OpenCL exceptions
#define _CRT_SECURE_NO_WARNINGS // Disable secure warnings

#include <stdio.h>     // For standard input/output functions
#include <stdlib.h>    // For memory allocation functions
#include <sys/types.h> // For defining data types
#include <inttypes.h>  // For printing fixed-width integer types

#ifdef __APPLE__
#include <OpenCL/cl.h> // Include OpenCL headers for macOS
#else
#include <CL/cl.h> // Include OpenCL headers for other platforms
#endif

#define PROGRAM_FILE "blank.cl"
#define KERNEL_NAME "blank"

// Macro to safely free memory
#define free_ptr(ptr) \
    if (ptr != NULL)  \
        free(ptr);

int main(int argc, char **argv)
{
    /* Identify a platform */
    cl_platform_id platform;
    cl_int err;
    err = clGetPlatformIDs(1, &platform, NULL);

    if (err != CL_SUCCESS)
    {
        // Print error message if no platforms are found
        fprintf(stderr, "Couldn't find any platforms\n");
        exit(EXIT_FAILURE);
    }

    /* Access a device, preferably a GPU */
    cl_device_id device;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err != CL_SUCCESS)
    {
        // If no GPU is found, try to find a CPU
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
        if (err != CL_SUCCESS)
        {
            // Print error message if no devices are found
            fprintf(stderr, "Couldn't find any devices in platform\n");
            exit(EXIT_FAILURE);
        }
    }
    char device_name[100];
    clGetDeviceInfo(device, CL_DEVICE_NAME, 100, device_name, NULL);
    printf("Device: %s\n", device_name);

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS)
    {
        // Print error message if context creation fails
        perror("Couldn't create a context\n");
        exit(EXIT_FAILURE);
    }

    // Compiler options for building the OpenCL program
    const char options[] = "-cl-finite-math-only -cl-no-signed-zeros";
    char *program_buffer;
    size_t program_size;
    cl_bool load_program_success = CL_TRUE;

    // Open the file in reading mode
    FILE *program_handle = fopen(PROGRAM_FILE, "r");

    if (program_handle == NULL)
    {
        // Print error message if file opening fails
        fprintf(stderr, "Error opening file :\"%s\"\n", PROGRAM_FILE);
        return EXIT_FAILURE;
    }

    // Seek to the end of the file to determine its size
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle); // Go back to the beginning of the file

    // Allocate memory for the buffer
    program_buffer = (char *)malloc((program_size + 1) * sizeof(char));
    if (program_buffer == NULL)
    {
        // Print error message if memory allocation fails
        fprintf(stderr, "Memory allocation failed\n");
        fclose(program_handle);
        free(program_buffer);
        clReleaseContext(context);
        exit(EXIT_FAILURE);
    }

    // Read the entire file into the buffer
    size_t read_size = fread(program_buffer, sizeof(char), program_size, program_handle);
    if (read_size != program_size)
    {
        // Print error message if file reading fails
        fprintf(stderr, "Error reading file\n");
        fclose(program_handle);
        free(program_buffer);
        clReleaseContext(context);
        exit(EXIT_FAILURE);
    }
    fclose(program_handle);

    // Create an OpenCL program from the source code
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&program_buffer, &program_size, &err);
    if (err < 0)
    {
        // Print error message if program creation fails
        perror("Couldn't create the program\n");
        free(program_buffer);
        clReleaseContext(context);
        exit(EXIT_FAILURE);
    }

    // Build the OpenCL program
    err = clBuildProgram(program, 1, &device, options, NULL, NULL);
    if (err < 0)
    {
        // Get the build log in case of an error
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char *program_log = (char *)malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
        // Print the build log
        printf("Compile Log: %s\n", program_log);
        free(program_log);
        free_ptr(program_buffer);
        clReleaseProgram(program);
        clReleaseContext(context);
        exit(EXIT_FAILURE);
    }
    // Free allocated memory after successful program build
    free_ptr(program_buffer);

    printf("OpenCL program built successfully!\n");

    cl_kernel kernel;
    kernel = clCreateKernel(program, KERNEL_NAME, &err);
    if (err != CL_SUCCESS)
    {
        perror("Couldn't find any kernels\n");
        clReleaseProgram(program);
        clReleaseContext(context);
        exit(EXIT_FAILURE);
    }

    // Determine OpenCL version
    char version_str[128];
    clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(version_str), version_str, NULL);
    int major_version = 1; // default to OpenCL 1.x
    int minor_version = 2;
    if (sscanf(version_str, "OpenCL %d.%d", &major_version, &minor_version) == 2)
    {
        printf("OpenCL version: %d.%d\n", major_version, minor_version);
    }

    // Create a command queue based on the OpenCL version
    cl_command_queue queue;
#if defined(CL_VERSION_2_x) || defined(CL_VERSION_3_x)
    cl_queue_properties properties[] = {CL_QUEUE_PROPERTIES, 0, 0};
    queue = clCreateCommandQueueWithProperties(context, device, properties, &err);

#else
    queue = clCreateCommandQueue(context, device, 0, &err);
#endif

    if (err < 0)
    {
        perror("Couldn't create the command queue\n");
        exit(EXIT_FAILURE);
    }

    char fn_name[100];
    clGetKernelInfo(kernel, CL_KERNEL_FUNCTION_NAME, sizeof(fn_name), fn_name, NULL);
    printf("Kernel %s.\n", fn_name);

    err = clEnqueueTask(queue, kernel, 0, NULL, NULL); // clEnqueueTask is deprecated
    // clEnqueueNDRangeKernel(queue, kernel, )
    if (err != CL_SUCCESS)
    {
        perror("Couldn't enqueue the kernel execution command\n");
        clReleaseCommandQueue(queue);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseContext(context);
        exit(EXIT_FAILURE);
    }
    else
    {
        char fn_name[100];
        clGetKernelInfo(kernel, CL_KERNEL_FUNCTION_NAME, sizeof(fn_name), fn_name, NULL);
        printf("Function %s was enqueued to command queue.\n", KERNEL_NAME);
    }

    clReleaseCommandQueue(queue);

    // Release kernels
    clReleaseKernel(kernel);

    // Release OpenCL program and context
    clReleaseProgram(program);
    clReleaseContext(context);
    return EXIT_SUCCESS;
}
