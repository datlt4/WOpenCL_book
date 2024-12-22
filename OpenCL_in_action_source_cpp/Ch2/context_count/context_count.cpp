// Enable OpenCL exceptions
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 300

#include <vector>   // For using std::vector
#include <iostream> // For standard input/output
#include <string>
#include <thread>
#include <chrono>
#ifdef __APPLE__
#include <OpenCL/cl.hpp> // OpenCL 1.2 for macOS
#else
#include "CL/cl.hpp" // OpenCL for other platforms
#endif

void useContext(const cl::Context &context)
{
    // Variable to store the reference count
    cl_uint ref_cnt;
    cl_uint err;

    err = context.getInfo(CL_CONTEXT_REFERENCE_COUNT, &ref_cnt);
    std::cout << "[THREAD] Reference count (" << __LINE__ << "): " << ref_cnt << std::endl;

    // Increment reference count to ensure the context is not deallocated
    clRetainContext(context());

    // Perform operations using the context

    // Get the reference count of the context and store it in ref_cnt
    err = context.getInfo(CL_CONTEXT_REFERENCE_COUNT, &ref_cnt);
    // Print the reference count
    std::cout << "[THREAD] Reference count (" << __LINE__ << "): " << ref_cnt << std::endl;

    // Simulate using the context by printing a message and sleeping for 1 second
    std::cout << "Using the context..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Get the reference count again after using the context
    err = context.getInfo(CL_CONTEXT_REFERENCE_COUNT, &ref_cnt);
    // Print the reference count again
    std::cout << "[THREAD] Reference count (" << __LINE__ << "): " << ref_cnt << std::endl;

    // Decrement reference count to release the context
    clReleaseContext(context());
    err = context.getInfo(CL_CONTEXT_REFERENCE_COUNT, &ref_cnt);
    std::cout << "[THREAD] Reference count (" << __LINE__ << "): " << ref_cnt << std::endl;
}

int main(int argc, char **argv)
{
    /* Identify a platform */
    std::vector<cl::Platform> platforms;
    cl_int err = cl::Platform::get(&platforms);
    if (err != CL_SUCCESS)
    {
        std::cerr << "No platform found!" << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Access a device, preferably a GPU */
    std::vector<cl::Device> devices;
    err = platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if (err != CL_SUCCESS)
    {
        err = platforms[0].getDevices(CL_DEVICE_TYPE_CPU, &devices);
        if (err != CL_SUCCESS)
        {
            std::cerr << "No device found on platform!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    /* Create the context */
    cl::Context context(devices, nullptr, nullptr, nullptr, &err);
    if (err != CL_SUCCESS)
    {
        std::cout << "Couldn't create a context" << std::endl;
    }

    /* Determine the reference count */
    cl_uint ref_cnt;
    err = context.getInfo(CL_CONTEXT_REFERENCE_COUNT, &ref_cnt);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Couldn't read the reference count." << std::endl;
    }
    std::cout << "Reference count [" << __LINE__ << "]: " << ref_cnt << std::endl;

    /* Update and display the reference count */
    clRetainContext(context());
    err = context.getInfo(CL_CONTEXT_REFERENCE_COUNT, &ref_cnt);
    std::cout << "Reference count [" << __LINE__ << "]: " << ref_cnt << std::endl;

    clReleaseContext(context());
    err = context.getInfo(CL_CONTEXT_REFERENCE_COUNT, &ref_cnt);
    std::cout << "Reference count [" << __LINE__ << "]: " << ref_cnt << std::endl;

    /* This thread */
    useContext(context);
    err = context.getInfo(CL_CONTEXT_REFERENCE_COUNT, &ref_cnt);
    std::cout << "Reference count [" << __LINE__ << "]: " << ref_cnt << std::endl;

    /* Multi-thread */
    /* The context object is not copied but referenced directly within the lambda function.
     * As a result, the reference count of the context does not increase when the thread is created.
     * The reference count remains the same before and after the thread is created and joined.
     */
    std::thread t1([&]()
                   { useContext(context); });
    err = context.getInfo(CL_CONTEXT_REFERENCE_COUNT, &ref_cnt);
    std::cout << "Reference count [" << __LINE__ << "]: " << ref_cnt << std::endl;
    t1.join();

    /* Multi-thread */
    /* When the cl::Context object is passed to the new thread, the copy constructor of cl::Context is called.
     * This constructor increments the reference count of the underlying OpenCL context to ensure that
     * it is not deallocated while the new copy is still in use.
     */
    std::thread t2(useContext, context);
    err = context.getInfo(CL_CONTEXT_REFERENCE_COUNT, &ref_cnt);
    std::cout << "Reference count [" << __LINE__ << "]: " << ref_cnt << std::endl;
    t2.join();
    err = context.getInfo(CL_CONTEXT_REFERENCE_COUNT, &ref_cnt);
    std::cout << "Reference count [" << __LINE__ << "]: " << ref_cnt << std::endl;

    /* Release context*/
    // Use the context for OpenCL operations...
    // Context will be automatically released when it goes out of scope
    // clReleaseContext(context());

    return EXIT_SUCCESS;
}