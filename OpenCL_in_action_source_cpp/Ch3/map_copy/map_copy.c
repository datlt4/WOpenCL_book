// Enable OpenCL exceptions and disable secure warnings in Visual Studio

// #define __NO_STD_STRING
#define __CL_ENABLE_EXCEPTIONS  // Enable OpenCL exceptions
#define _CRT_SECURE_NO_WARNINGS // Disable secure warnings

#include <stdio.h>     // For standard input/output functions
#include <stdlib.h>    // For memory allocation functions
#include <sys/types.h> // For defining data types
#include <inttypes.h>  // For printing fixed-width integer types
#include <string.h>

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

// Helper function to check OpenCL error codes
#define CHECK_CL_ERROR(err)                                                       \
    if (err != CL_SUCCESS)                                                        \
    {                                                                             \
        fprintf(stderr, "OpenCL error with code %i at line %i\n", err, __LINE__); \
        exit(EXIT_FAILURE);                                                       \
    }

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device()
{
    /* Identify a platform */
    cl_platform_id platform;
    cl_int err;
    err = clGetPlatformIDs(1, &platform, NULL);
    CHECK_CL_ERROR(err);

    /* Access a device, preferably a GPU */
    cl_device_id device;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err != CL_SUCCESS)
    {
        // If no GPU is found, try to find a CPU
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
        CHECK_CL_ERROR(err);
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
    if (err != CL_SUCCESS)
    {
        // Print error message if program creation fails
        perror("Couldn't create the program\n");
        free(program_buffer);
        clReleaseContext(*context);
        exit(EXIT_FAILURE);
    }

    // Build the OpenCL program
    err = clBuildProgram(program, 1, device, options, NULL, NULL);
    if (err != CL_SUCCESS)
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
    // Create OpenCL device
    cl_device_id device = create_device();
    cl_int err;

    // Create OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    CHECK_CL_ERROR(err);

    // Build OpenCL program
    cl_program program = build_program(&context, &device, PROGRAM_FILE);
    cl_kernel kernel = clCreateKernel(program, KERNEL_NAME, &err);
    CHECK_CL_ERROR(err);

    /* Data and buffers */
    float data_one[100], data_two[100], result_array[100];

    /* Initialize arrays */
    for (int i = 0; i < 100; i++)
    {
        data_one[i] = 1.0f * i;
        data_two[i] = -1.0f * i;
        result_array[i] = 0.0f;
    }

    // Create OpenCL buffers
    cl_mem buffer_one = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(data_one), data_one, &err);
    CHECK_CL_ERROR(err);
    cl_mem buffer_two = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(data_two), data_two, &err);
    CHECK_CL_ERROR(err);

    // Set kernel arguments
    CHECK_CL_ERROR(clSetKernelArg(kernel, 0, sizeof(buffer_one), &buffer_one));
    CHECK_CL_ERROR(clSetKernelArg(kernel, 1, sizeof(buffer_two), &buffer_two));

    // Create command queue
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
    CHECK_CL_ERROR(err);

    // Execute kernel
    CHECK_CL_ERROR(clEnqueueTask(queue, kernel, 0, NULL, NULL));

    // Read buffer_one back to result_array and print
    CHECK_CL_ERROR(clEnqueueReadBuffer(queue, buffer_one, CL_TRUE, 0, 100 * sizeof(float), (void *)result_array, 0, NULL, NULL));
    printf("\nResult from buffer_one:\n");
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            printf("%6.1f", result_array[j + i * 10]);
        }
        printf("\n");
    }

    // Read buffer_two back to result_array and print
    CHECK_CL_ERROR(clEnqueueReadBuffer(queue, buffer_two, CL_TRUE, 0, 100 * sizeof(float), (void *)result_array, 0, NULL, NULL));
    printf("\nResult from buffer_two:\n");
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            printf("%6.1f", result_array[j + i * 10]);
        }
        printf("\n");
    }

    // Perform buffer copy operation using clEnqueueCopyBufferRect
    size_t src_origin[3] = {1 * sizeof(float), 1, 0};
    size_t dst_origin[3] = {1 * sizeof(float), 1, 0};
    size_t regions[3] = {4 * sizeof(float), 4, 1};
    CHECK_CL_ERROR(clEnqueueCopyBufferRect(queue, buffer_one, buffer_two,
                                           src_origin, dst_origin, regions,
                                           10 * sizeof(float), 0, 10 * sizeof(float), 0,
                                           0, NULL, NULL));

    // Map buffer_two, perform memcpy, and unmap
    void *mapped_memory = clEnqueueMapBuffer(queue, buffer_two, CL_TRUE, CL_MAP_WRITE, 0, 100 * sizeof(float), 0, NULL, NULL, &err);
    memcpy((void *)result_array, mapped_memory, 100 * sizeof(float));
    /**
     * When you map a buffer, it provides a snapshot of the buffer's state at that moment.
     * If you modify the buffer on the device after mapping it, those changes might not be
     * immediately visible in the mapped host memory unless you unmap and remap the buffer.
     */
    CHECK_CL_ERROR(clEnqueueUnmapMemObject(queue, buffer_two, mapped_memory, 0, NULL, NULL));
    CHECK_CL_ERROR(err);
    printf("\nResult after buffer copy operation:\n");
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            printf("%6.1f", result_array[j + i * 10]);
        }
        printf("\n");
    }

    // Perform another buffer copy operation
    size_t src_origin2[3] = {1 * sizeof(float), 1, 0};
    size_t dst_origin2[3] = {5 * sizeof(float), 5, 0};
    size_t regions2[3] = {4 * sizeof(float), 4, 1};
    CHECK_CL_ERROR(clEnqueueCopyBufferRect(queue, buffer_one, buffer_two,
                                           src_origin2, dst_origin2, regions2,
                                           10 * sizeof(float), 0, 10 * sizeof(float), 0,
                                           0, NULL, NULL));

    // Map buffer_two, perform memcpy, and unmap again
    mapped_memory = clEnqueueMapBuffer(queue, buffer_two, CL_TRUE, CL_MAP_WRITE, 0, 100 * sizeof(float), 0, NULL, NULL, &err);
    memcpy((void *)result_array, mapped_memory, 100 * sizeof(float));
    CHECK_CL_ERROR(clEnqueueUnmapMemObject(queue, buffer_two, mapped_memory, 0, NULL, NULL));
    CHECK_CL_ERROR(err);
    printf("\nResult after second buffer copy operation:\n");
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            printf("%6.1f", result_array[j + i * 10]);
        }
        printf("\n");
    }

    // Clean up resources
    CHECK_CL_ERROR(clReleaseCommandQueue(queue));
    CHECK_CL_ERROR(clReleaseMemObject(buffer_one));
    CHECK_CL_ERROR(clReleaseMemObject(buffer_two));
    CHECK_CL_ERROR(clReleaseKernel(kernel));
    CHECK_CL_ERROR(clReleaseProgram(program));
    CHECK_CL_ERROR(clReleaseContext(context));

    return EXIT_SUCCESS;
}
