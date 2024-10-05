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

    cl_program program = build_program(&context, &device, PROGRAM_FILE);
    cl_kernel kernel = clCreateKernel(program, KERNEL_NAME, &err);
    if (err != CL_SUCCESS)
    {
        perror("Couldn't create a kernel");
        exit(EXIT_FAILURE);
    };

    /* Data and buffers */
    float full_matrix[80], zero_matrix[80];
    cl_mem matrix_buffer;

    /* Initialize data */
    for (int i = 0; i < 80; i++)
    {
        full_matrix[i] = i * 1.0f;
        zero_matrix[i] = 0.0;
    }

    /* Create a buffer to hold 80 floats */
    matrix_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(full_matrix), full_matrix, &err);
    if (err != CL_SUCCESS)
    {
        perror("Couldn't create a buffer object\n");
        exit(EXIT_FAILURE);
    };

    /* Set buffer as argument to the kernel */
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &matrix_buffer);
    if (err < 0)
    {
        perror("Couldn't set the buffer as the kernel argument");
        exit(1);
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
        perror("Couldn't create a command queue");
        exit(1);
    };

    /* Enqueue kernel */
    err = clEnqueueTask(queue, kernel, 0, NULL, NULL);
    if (err < 0)
    {
        perror("Couldn't enqueue the kernel");
        exit(1);
    }

    /**
     * Enqueue command to write to buffer
     * Ensuring Data Synchronization: clEnqueueWriteBuffer ensures that the data in the buffer is up-to-date before the kernel executes.
     * This is particularly important if full_matrix has been modified on the host after the buffer was created.
     */
    err = clEnqueueWriteBuffer(queue, matrix_buffer, CL_TRUE, 0, sizeof(full_matrix), full_matrix, 0, NULL, NULL);
    if (err < 0)
    {
        perror("Couldn't write to the buffer object");
        exit(1);
    }

    /* Enqueue command to read rectangle of data */
    const size_t buffer_origin[3] = {5 * sizeof(float), 3, 0}; // The (x in bytes, y in rows, z in slices) offset in the memory region associated with `buffer`
    const size_t host_origin[3] = {1 * sizeof(float), 1, 0};   // The (x in bytes, y in rows, z in slices) offset in the memory region pointed to by `ptr`.
    const size_t region[3] = {4 * sizeof(float), 4, 1};        // The (width in bytes, height in rows, depth in slices) of the 2D or 3D rectangle.
    err = clEnqueueReadBufferRect(queue, matrix_buffer, CL_TRUE, buffer_origin, host_origin, region, 10 * sizeof(float), 0, 10 * sizeof(float), 0, zero_matrix, 0, NULL, NULL);
    if (err < 0)
    {
        perror("Couldn't read the rectangle from the buffer object");
        exit(1);
    }

    /* Display updated buffer */
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            printf("%6.1f", zero_matrix[j + i * 10]);
        }
        printf("\n");
    }

    /* Enqueue command to read data */
    err = clEnqueueReadBuffer(queue, matrix_buffer, CL_TRUE, 0, sizeof(zero_matrix), zero_matrix, 0, NULL, NULL);
    if (err < 0)
    {
        perror("Couldn't read the rectangle from the buffer object\n");
        exit(1);
    }

    /* Display updated buffer */
    /* Display updated buffer */
    printf("\n");
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            printf("%6.1f", zero_matrix[j + i * 10]);
        }
        printf("\n");
    }

    /* Deallocate resources */
    clReleaseMemObject(matrix_buffer);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clRetainProgram(program);
    clReleaseContext(context);

    return EXIT_SUCCESS;
}
