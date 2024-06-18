// Enable OpenCL exceptions and disable secure warnings in Visual Studio

// #define __NO_STD_STRING
#define __CL_ENABLE_EXCEPTIONS // Enable OpenCL exceptions
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

#define NUM_FILES 2
#define PROGRAM_FILE_1 "good.cl"
#define PROGRAM_FILE_2 "bad.cl"

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
        fprintf(stderr, "Couldn't find any platforms");
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
            fprintf(stderr, "Couldn't find any devices in platform");
            exit(EXIT_FAILURE);
        }
    }

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS)
    {
        // Print error message if context creation fails
        perror("Couldn't create a context");
        exit(EXIT_FAILURE);
    }

    // Compiler options for building the OpenCL program
    const char options[] = "-cl-finite-math-only -cl-no-signed-zeros";
    const char *program_file_list[] = {PROGRAM_FILE_1, PROGRAM_FILE_2};
    char *program_buffer[NUM_FILES];
    size_t program_size[NUM_FILES];
    cl_bool load_program_success = CL_TRUE;

    for (int i = 0; i < NUM_FILES; ++i)
    {
        // Open the file in reading mode
        FILE *program_handle = fopen(program_file_list[i], "r");

        if (program_handle == NULL)
        {
            // Print error message if file opening fails
            fprintf(stderr, "Error opening file \"%s\": ", program_file_list[i]);
            return EXIT_FAILURE;
        }

        // Seek to the end of the file to determine its size
        fseek(program_handle, 0, SEEK_END);
        program_size[i] = ftell(program_handle);
        rewind(program_handle); // Go back to the beginning of the file

        // Allocate memory for the buffer
        program_buffer[i] = (char *)malloc((program_size[i] + 1) * sizeof(char));
        if (program_buffer[i] == NULL)
        {
            // Print error message if memory allocation fails
            fprintf(stderr, "Memory allocation failed");
            load_program_success = CL_FALSE;
            fclose(program_handle);
            break;
        }

        // Read the entire file into the buffer
        size_t read_size = fread(program_buffer[i], sizeof(char), program_size[i], program_handle);
        if (read_size != program_size[i])
        {
            // Print error message if file reading fails
            fprintf(stderr, "Error reading file");
            load_program_success = CL_FALSE;
            fclose(program_handle);
            break;
        }
        fclose(program_handle);
    }

    // Check if all programs were loaded successfully
    if (!load_program_success)
    {
        // Free allocated memory if loading failed
        for (int i = 0; i < NUM_FILES; ++i)
        {
            free_ptr(program_buffer[i]);
        }
        return EXIT_FAILURE;
    }

    // Create an OpenCL program from the source code
    cl_program program = clCreateProgramWithSource(context, NUM_FILES, (const char **)program_buffer, program_size, &err);
    if (err < 0)
    {
        // Print error message if program creation fails
        perror("Couldn't create the program");
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
        printf("%s\n", program_log);
        free(program_log);
        exit(EXIT_FAILURE);
    }

    printf("OpenCL program built successfully!");

    // Free allocated memory after successful program build
    for (int i = 0; i < NUM_FILES; ++i)
    {
        free_ptr(program_buffer[i]);
    }

    // Release OpenCL program and context
    clReleaseProgram(program);
    clReleaseContext(context);
    return EXIT_SUCCESS;
}
