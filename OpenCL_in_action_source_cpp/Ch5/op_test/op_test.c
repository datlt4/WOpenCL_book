// Enable OpenCL exceptions and disable secure warnings in Visual Studio

// #define __NO_STD_STRING
#define __CL_ENABLE_EXCEPTIONS  // Enable OpenCL exceptions
#define _CRT_SECURE_NO_WARNINGS // Disable secure warnings

#include <stdio.h>     // For standard input/output functions
#include <stdlib.h>    // For memory allocation functions
#include <sys/types.h> // For defining data types
#include <inttypes.h>  // For printing fixed-width integer types
#include <string.h>    // For string manipulation functions

#ifdef __APPLE__
#include <OpenCL/cl.h> // Include OpenCL headers for macOS
#else
#include <CL/cl.h> // Include OpenCL headers for other platforms
#endif

#define PROGRAM_FILE "op_test.cl" // Define the name of the kernel file

// Macro to safely free memory
#define free_ptr(ptr) \
    if (ptr != NULL)  \
        free(ptr);

// Helper function to check OpenCL error codes and exit on error
#define CHECK_CL_ERROR(err)                                                       \
    if (err != CL_SUCCESS)                                                        \
    {                                                                             \
        fprintf(stderr, "OpenCL error with code %i at line %i\n", err, __LINE__); \
        exit(EXIT_FAILURE);                                                       \
    }

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device()
{
    cl_platform_id platform;
    cl_int err;

    // Identify a platform
    err = clGetPlatformIDs(1, &platform, NULL);
    CHECK_CL_ERROR(err);

    cl_device_id device;

    // Access a device, preferably a GPU
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err != CL_SUCCESS)
    {
        // If no GPU is found, try to find a CPU
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
        CHECK_CL_ERROR(err);
    }

    // Print device name
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
    else
    {
        // perr "Failed to parse OpenCL version"
        fprintf(stderr, "Failed to parse OpenCL version");
    }

    return device;
}

/* Create program from a file and compile it */
cl_program build_program(const cl_context *context, const cl_device_id *device, const char *filename)
{
    cl_int err;
    char options[100] = "-cl-finite-math-only -cl-no-signed-zeros";

    size_t len_extensions;
    char *extensions;
    CHECK_CL_ERROR(clGetDeviceInfo(*device, CL_DEVICE_EXTENSIONS, 0, NULL, &len_extensions));
    extensions = malloc(len_extensions + 1);
    extensions[len_extensions] = '\0';
    CHECK_CL_ERROR(clGetDeviceInfo(*device, CL_DEVICE_EXTENSIONS, len_extensions + 1, extensions, NULL));

    // Compiler options for building the OpenCL program
    char *program_buffer;
    size_t program_size;

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CL/cl.h>

int main(int argc, char **argv)
{
    cl_int err; // Variable to store OpenCL error codes

    // Create OpenCL device
    cl_device_id device = create_device();                                    // Function to find and create an OpenCL device
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err); // Create OpenCL context
    CHECK_CL_ERROR(err);                                                      // Check for errors during context creation
    cl_program program = build_program(&context, &device, PROGRAM_FILE);      // Build the OpenCL program from a file
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);  // Create command queue
    CHECK_CL_ERROR(err);                                                      // Check for errors during command queue creation

    // Array to hold the results read from the buffer
    int vector[4] = {0}; // Initialize vector with zeroes

    // Create and set up the first kernel
    cl_kernel kn_op_test1 = clCreateKernel(program, "op_test1", &err);                                                    // Create kernel from the program
    CHECK_CL_ERROR(err);                                                                                                  // Check for errors during kernel creation
    cl_mem vec_buffer1 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(vector), vector, &err); // Create buffer for the kernel
    CHECK_CL_ERROR(err);                                                                                                  // Check for errors during buffer creation
    CHECK_CL_ERROR(clSetKernelArg(kn_op_test1, 0, sizeof(cl_mem), &vec_buffer1));                                         // Set buffer as kernel argument
    CHECK_CL_ERROR(clEnqueueTask(queue, kn_op_test1, 0, NULL, NULL));                                                     // Enqueue kernel execution
    memset((void *)vector, 0, sizeof(vector));                                                                            // Clear the vector
    CHECK_CL_ERROR(clEnqueueReadBuffer(queue, vec_buffer1, CL_TRUE, 0, sizeof(vector), vector, 0, NULL, NULL));           // Read buffer back to vector
    printf("output: [0]: %d, [1]: %d, [2]: %d, [3]: %d\n", vector[0], vector[1], vector[2], vector[3]);                   // Print the results

    // Create and set up the second kernel
    cl_kernel kn_op_test2 = clCreateKernel(program, "op_test2", &err);                                          // Create kernel from the program
    CHECK_CL_ERROR(err);                                                                                        // Check for errors during kernel creation
    cl_mem vec_buffer2 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(vector), NULL, &err);                // Create buffer for the kernel
    CHECK_CL_ERROR(err);                                                                                        // Check for errors during buffer creation
    CHECK_CL_ERROR(clSetKernelArg(kn_op_test2, 0, sizeof(cl_mem), &vec_buffer2));                               // Set buffer as kernel argument
    CHECK_CL_ERROR(clEnqueueCopyBuffer(queue, vec_buffer1, vec_buffer2, 0, 0, sizeof(vector), 0, NULL, NULL));  // Copy data from vec_buffer1 to vec_buffer2
    CHECK_CL_ERROR(clEnqueueTask(queue, kn_op_test2, 0, NULL, NULL));                                           // Enqueue kernel execution
    memset((void *)vector, 0, sizeof(vector));                                                                  // Clear the vector
    CHECK_CL_ERROR(clEnqueueReadBuffer(queue, vec_buffer2, CL_TRUE, 0, sizeof(vector), vector, 0, NULL, NULL)); // Read buffer back to vector
    printf("output: [0]: %d, [1]: %d, [2]: %d, [3]: %d\n", vector[0], vector[1], vector[2], vector[3]);         // Print the results

    // Create and set up the third kernel
    cl_kernel kn_op_test3 = clCreateKernel(program, "op_test3", &err);                                          // Create kernel from the program
    CHECK_CL_ERROR(err);                                                                                        // Check for errors during kernel creation
    cl_mem vec_buffer3 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(vector), NULL, &err);                // Create buffer for the kernel
    CHECK_CL_ERROR(err);                                                                                        // Check for errors during buffer creation
    CHECK_CL_ERROR(clSetKernelArg(kn_op_test3, 0, sizeof(cl_mem), &vec_buffer3));                               // Set buffer as kernel argument
    CHECK_CL_ERROR(clEnqueueCopyBuffer(queue, vec_buffer2, vec_buffer3, 0, 0, sizeof(vector), 0, NULL, NULL));  // Copy data from vec_buffer2 to vec_buffer3
    CHECK_CL_ERROR(clEnqueueTask(queue, kn_op_test3, 0, NULL, NULL));                                           // Enqueue kernel execution
    memset((void *)vector, 0, sizeof(vector));                                                                  // Clear the vector
    CHECK_CL_ERROR(clEnqueueReadBuffer(queue, vec_buffer3, CL_TRUE, 0, sizeof(vector), vector, 0, NULL, NULL)); // Read buffer back to vector
    printf("output: [0]: %d, [1]: %d, [2]: %d, [3]: %d\n", vector[0], vector[1], vector[2], vector[3]);         // Print the results

    // Create and set up the fourth kernel
    cl_kernel kn_op_test4 = clCreateKernel(program, "op_test4", &err);                                          // Create kernel from the program
    CHECK_CL_ERROR(err);                                                                                        // Check for errors during kernel creation
    cl_mem vec_buffer4 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(vector), NULL, &err);                // Create buffer for the kernel
    CHECK_CL_ERROR(err);                                                                                        // Check for errors during buffer creation
    CHECK_CL_ERROR(clSetKernelArg(kn_op_test4, 0, sizeof(cl_mem), &vec_buffer4));                               // Set buffer as kernel argument
    CHECK_CL_ERROR(clEnqueueCopyBuffer(queue, vec_buffer3, vec_buffer4, 0, 0, sizeof(vector), 0, NULL, NULL));  // Copy data from vec_buffer3 to vec_buffer4
    CHECK_CL_ERROR(clEnqueueTask(queue, kn_op_test4, 0, NULL, NULL));                                           // Enqueue kernel execution
    memset((void *)vector, 0, sizeof(vector));                                                                  // Clear the vector
    CHECK_CL_ERROR(clEnqueueReadBuffer(queue, vec_buffer4, CL_TRUE, 0, sizeof(vector), vector, 0, NULL, NULL)); // Read buffer back to vector
    printf("output: [0]: %d, [1]: %d, [2]: %d, [3]: %d\n", vector[0], vector[1], vector[2], vector[3]);         // Print the results

    // Release OpenCL resources
    CHECK_CL_ERROR(clReleaseMemObject(vec_buffer4)); // Release buffer
    CHECK_CL_ERROR(clReleaseMemObject(vec_buffer3)); // Release buffer
    CHECK_CL_ERROR(clReleaseMemObject(vec_buffer2)); // Release buffer
    CHECK_CL_ERROR(clReleaseMemObject(vec_buffer1)); // Release buffer
    CHECK_CL_ERROR(clReleaseKernel(kn_op_test4));    // Release kernel
    CHECK_CL_ERROR(clReleaseKernel(kn_op_test3));    // Release kernel
    CHECK_CL_ERROR(clReleaseKernel(kn_op_test2));    // Release kernel
    CHECK_CL_ERROR(clReleaseKernel(kn_op_test1));    // Release kernel
    CHECK_CL_ERROR(clReleaseCommandQueue(queue));    // Release command queue
    CHECK_CL_ERROR(clReleaseProgram(program));       // Release program
    CHECK_CL_ERROR(clReleaseContext(context));       // Release context

    return EXIT_SUCCESS; // Return success
}
