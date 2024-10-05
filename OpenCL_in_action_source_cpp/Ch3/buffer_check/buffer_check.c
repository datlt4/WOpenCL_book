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

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device()
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

    // Determine OpenCL version
    char version_str[128];
    clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(version_str), version_str, NULL);
    int major_version = 1; // default to OpenCL 1.x
    int minor_version = 2;
    if (sscanf(version_str, "OpenCL %d.%d", &major_version, &minor_version) == 2)
    {
        printf("OpenCL version: %d.%d\n", major_version, minor_version);
    }

    return device;
}

/* Create program from a file and compile it */
cl_program build_program(cl_context *context, cl_device_id *device, const char *filename)
{
    cl_int err;
    // Compiler options for building the OpenCL program
    const char options[] = "-cl-finite-math-only -cl-no-signed-zeros";
    char *program_buffer;
    size_t program_size;
    cl_bool load_program_success = CL_TRUE;

    // Open the file in reading mode
    FILE *program_handle = fopen(filename, "r");

    if (program_handle == NULL)
    {
        // Print error message if file opening fails
        fprintf(stderr, "Error opening file :\"%s\"\n", filename);
        clReleaseContext(*context);
        exit(EXIT_FAILURE);
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
        clReleaseContext(*context);
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
        clReleaseContext(*context);
        exit(EXIT_FAILURE);
    }
    fclose(program_handle);

    // Create an OpenCL program from the source code
    cl_program program = clCreateProgramWithSource(*context, 1, (const char **)&program_buffer, &program_size, &err);
    if (err < 0)
    {
        // Print error message if program creation fails
        perror("Couldn't create the program\n");
        free(program_buffer);
        clReleaseContext(*context);
        exit(EXIT_FAILURE);
    }

    // Build the OpenCL program
    err = clBuildProgram(program, 1, device, options, NULL, NULL);
    if (err < 0)
    {
        // Get the build log in case of an error
        size_t log_size;
        clGetProgramBuildInfo(program, *device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char *program_log = (char *)malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, *device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
        // Print the build log
        printf("Compile Log: %s\n", program_log);
        free(program_log);
        free_ptr(program_buffer);
        clReleaseProgram(program);
        clReleaseContext(*context);
        exit(EXIT_FAILURE);
    }
    // Free allocated memory after successful program build
    free_ptr(program_buffer);

    printf("OpenCL program built successfully!\n");

    return program;
}

int main(int argc, char **argv)
{
    cl_device_id device = create_device();
    cl_int err;

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS)
    {
        // Print error message if context creation fails
        perror("Couldn't create a context\n");
        exit(EXIT_FAILURE);
    }

    /* Example main data array */
    float main_data[100]; // Example: array of 100 floats

    /* Create a buffer to hold 100 floating-point values */
    cl_mem main_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(main_data), main_data, &err);
    if (err != CL_SUCCESS)
    {
        perror("Couldn't create a buffer\n");
        exit(EXIT_FAILURE);
    }

    /* Create a sub-buffer */
    size_t origin = 5 * sizeof(float); // Example origin
    size_t size = 30 * sizeof(float);  // Example size

    // Check if size exceeds the parent buffer's size
    if (origin + size > sizeof(main_data))
    {
        printf("Error: Sub-buffer size exceeds parent buffer size\n");
        exit(EXIT_FAILURE);
    }

    cl_buffer_region region = {.origin = origin, .size = size};
    cl_mem sub_buffer = clCreateSubBuffer(main_buffer, CL_MEM_READ_ONLY, CL_BUFFER_CREATE_TYPE_REGION, &region, &err);
    if (err != CL_SUCCESS)
    {
        perror("Couldn't create a sub-buffer");
        printf("\n\t%i\n", err);
        exit(EXIT_FAILURE);
    }

    /* Obtain size information about the buffers */
    size_t main_buffer_size, sub_buffer_size;
    clGetMemObjectInfo(main_buffer, CL_MEM_SIZE, sizeof(main_buffer_size), &main_buffer_size, NULL);
    clGetMemObjectInfo(sub_buffer, CL_MEM_SIZE, sizeof(sub_buffer_size), &sub_buffer_size, NULL);
    printf("Main buffer size: %lu\n", main_buffer_size);
    printf("Sub-buffer size:  %lu\n", sub_buffer_size);

    /* Obtain the host pointers */
    size_t main_buffer_mem, sub_buffer_mem;
    clGetMemObjectInfo(main_buffer, CL_MEM_HOST_PTR, sizeof(main_buffer_mem), &main_buffer_mem, NULL);
    clGetMemObjectInfo(sub_buffer, CL_MEM_HOST_PTR, sizeof(sub_buffer_mem), &sub_buffer_mem, NULL);
    printf("Main buffer memory address: %p\n", main_buffer_mem);
    printf("Sub-buffer memory address:  %p\n", sub_buffer_mem);

    /* Print the address of the main data */
    printf("Main array address: %p\n", main_data);

    /* Deallocate resources */
    clReleaseMemObject(main_buffer);
    clReleaseMemObject(sub_buffer);
    clReleaseContext(context);

    return EXIT_SUCCESS;
}
