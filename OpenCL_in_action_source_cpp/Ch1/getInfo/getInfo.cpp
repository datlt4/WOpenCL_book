// #define __NO_STD_STRING
#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#ifdef __APPLE__
#include <OpenCL/cl.hpp> // OpenCL 1.2
#else
#include "CL/cl.hpp"
#endif

int main()
{
    std::ostringstream os;
    std::vector<cl::Platform> platforms;
    try
    {
        cl::Platform::get(&platforms);
        os << "NUMBER OF OPENCL PLATFORMS : " << platforms.size() << std::endl;
        int count_platform = 0;
        for (cl::Platform &platform : platforms)
        {
            os << "+ Platform " << count_platform << ":" << std::endl;
            os << "\t- CL_PLATFORM_NAME       : " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
            os << "\t- CL_PLATFORM_VENDOR     : " << platform.getInfo<CL_PLATFORM_VENDOR>() << std::endl;
            os << "\t- CL_PLATFORM_VERSION    : " << platform.getInfo<CL_PLATFORM_VERSION>() << std::endl;
            os << "\t- CL_PLATFORM_PROFILE    : " << platform.getInfo<CL_PLATFORM_PROFILE>() << std::endl;
            os << "\t- CL_PLATFORM_EXTENSIONS : " << platform.getInfo<CL_PLATFORM_EXTENSIONS>() << std::endl;
            std::vector<cl::Device> devices;
            platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
            os << "\t- NUMBER OF DEVICES      : " << devices.size() << std::endl;

            int count_device = 0;
            for (cl::Device &device : devices)
            {
                os << "\n\tDevice : " << count_device << std::endl;
                os << "\t\t. CL_DEVICE_NAME                          : " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
                os << "\t\t. CL_DEVICE_VENDOR                        : " << device.getInfo<CL_DEVICE_VENDOR>() << std::endl;
                os << "\t\t. CL_DRIVER_VERSION                       : " << device.getInfo<CL_DRIVER_VERSION>() << std::endl;
                os << "\t\t. CL_DEVICE_PROFILE                       : " << device.getInfo<CL_DEVICE_PROFILE>() << std::endl;
                os << "\t\t. CL_DEVICE_VERSION                       : " << device.getInfo<CL_DEVICE_VERSION>() << std::endl;
                os << "\t\t. CL_DEVICE_EXTENSIONS                    : " << device.getInfo<CL_DEVICE_EXTENSIONS>() << std::endl;
                os << "\t\t. CL_DEVICE_PLATFORM                      : " << device.getInfo<CL_DEVICE_PLATFORM>() << std::endl;
                std::string deviceType;
                switch (device.getInfo<CL_DEVICE_TYPE>())
                {
                case 1 << 0:
                    deviceType = std::string("CL_DEFAULT");
                    break;
                case 1 << 1:
                    deviceType = std::string("CL_CPU");
                    break;
                case 1 << 2:
                    deviceType = std::string("CL_GPU");
                    break;
                case 1 << 3:
                    deviceType = std::string("CL_ACCELERATOR");
                    break;
                default:
                    break;
                }
                os << "\t\t. CL_DEVICE_TYPE                          : " << deviceType << std::endl;
                os << "\t\t. CL_DEVICE_VENDOR_ID                     : " << device.getInfo<CL_DEVICE_VENDOR_ID>() << std::endl;
                os << "\t\t. CL_DEVICE_MAX_COMPUTE_UNITS             : " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
                os << "\t\t. CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS      : " << device.getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>() << std::endl;
                os << "\t\t. CL_DEVICE_MAX_WORK_GROUP_SIZE           : " << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl;
                std::vector<std::size_t> maxWorkItems = device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
                // os << "\t\t. CL_DEVICE_MAX_WORK_ITEM_SIZES           : " << device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>() << std::endl;
                for (int i = 0; i < maxWorkItems.size(); i++)
                {
                    if (i == 0)
                    {
                        os << "\t\t. CL_DEVICE_MAX_WORK_ITEM_SIZES           : " << maxWorkItems[i];
                    }
                    else
                    {
                        os << "x" << maxWorkItems[i];
                    }
                }
                os << std::endl;
                os << "\t\t. CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR   : " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR>() << std::endl;
                os << "\t\t. CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT  : " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT>() << std::endl;
                os << "\t\t. CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT    : " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT>() << std::endl;
                os << "\t\t. CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG   : " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG>() << std::endl;
                os << "\t\t. CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT  : " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT>() << std::endl;
                os << "\t\t. CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE : " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE>() << std::endl;
                os << "\t\t. CL_DEVICE_MAX_CLOCK_FREQUENCY           : " << device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << std::endl;
                os << "\t\t. CL_DEVICE_ADDRESS_BITS                  : " << device.getInfo<CL_DEVICE_ADDRESS_BITS>() << std::endl;
                os << "\t\t. CL_DEVICE_MAX_READ_IMAGE_ARGS           : " << device.getInfo<CL_DEVICE_MAX_READ_IMAGE_ARGS>() << std::endl;
                os << "\t\t. CL_DEVICE_MAX_WRITE_IMAGE_ARGS          : " << device.getInfo<CL_DEVICE_MAX_WRITE_IMAGE_ARGS>() << std::endl;
                os << "\t\t. CL_DEVICE_MAX_MEM_ALLOC_SIZE            : " << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << std::endl;
                os << "\t\t. CL_DEVICE_IMAGE2D_MAX_WIDTH             : " << device.getInfo<CL_DEVICE_IMAGE2D_MAX_WIDTH>() << std::endl;
                os << "\t\t. CL_DEVICE_IMAGE2D_MAX_HEIGHT            : " << device.getInfo<CL_DEVICE_IMAGE2D_MAX_HEIGHT>() << std::endl;
                os << "\t\t. CL_DEVICE_IMAGE3D_MAX_WIDTH             : " << device.getInfo<CL_DEVICE_IMAGE3D_MAX_WIDTH>() << std::endl;
                os << "\t\t. CL_DEVICE_IMAGE3D_MAX_HEIGHT            : " << device.getInfo<CL_DEVICE_IMAGE3D_MAX_HEIGHT>() << std::endl;
                os << "\t\t. CL_DEVICE_IMAGE3D_MAX_DEPTH             : " << device.getInfo<CL_DEVICE_IMAGE3D_MAX_DEPTH>() << std::endl;
                os << "\t\t. CL_DEVICE_IMAGE_SUPPORT                 : " << device.getInfo<CL_DEVICE_IMAGE_SUPPORT>() << std::endl;
                os << "\t\t. CL_DEVICE_MAX_PARAMETER_SIZE            : " << device.getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>() << std::endl;
                os << "\t\t. CL_DEVICE_MAX_SAMPLERS                  : " << device.getInfo<CL_DEVICE_MAX_SAMPLERS>() << std::endl;
                os << "\t\t. CL_DEVICE_MEM_BASE_ADDR_ALIGN           : " << device.getInfo<CL_DEVICE_MEM_BASE_ADDR_ALIGN>() << std::endl;
                os << "\t\t. CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE      : " << device.getInfo<CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE>() << std::endl;
                os << "\t\t. CL_DEVICE_SINGLE_FP_CONFIG              : " << device.getInfo<CL_DEVICE_SINGLE_FP_CONFIG>() << std::endl;
                os << "\t\t. CL_DEVICE_GLOBAL_MEM_CACHE_TYPE         : " << device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_TYPE>() << std::endl;
                os << "\t\t. CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE     : " << device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE>() << std::endl;
                os << "\t\t. CL_DEVICE_GLOBAL_MEM_CACHE_SIZE         : " << device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_SIZE>() << std::endl;
                os << "\t\t. CL_DEVICE_GLOBAL_MEM_SIZE               : " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << std::endl;
                os << "\t\t. CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE      : " << device.getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>() << std::endl;
                os << "\t\t. CL_DEVICE_MAX_CONSTANT_ARGS             : " << device.getInfo<CL_DEVICE_MAX_CONSTANT_ARGS>() << std::endl;
                os << "\t\t. CL_DEVICE_LOCAL_MEM_TYPE                : " << device.getInfo<CL_DEVICE_LOCAL_MEM_TYPE>() << std::endl;
                os << "\t\t. CL_DEVICE_LOCAL_MEM_SIZE                : " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << std::endl;
                os << "\t\t. CL_DEVICE_ERROR_CORRECTION_SUPPORT      : " << device.getInfo<CL_DEVICE_ERROR_CORRECTION_SUPPORT>() << std::endl;
                os << "\t\t. CL_DEVICE_PROFILING_TIMER_RESOLUTION    : " << device.getInfo<CL_DEVICE_PROFILING_TIMER_RESOLUTION>() << std::endl;
                os << "\t\t. CL_DEVICE_ENDIAN_LITTLE                 : " << device.getInfo<CL_DEVICE_ENDIAN_LITTLE>() << std::endl;
                os << "\t\t. CL_DEVICE_AVAILABLE                     : " << device.getInfo<CL_DEVICE_AVAILABLE>() << std::endl;
                os << "\t\t. CL_DEVICE_COMPILER_AVAILABLE            : " << device.getInfo<CL_DEVICE_COMPILER_AVAILABLE>() << std::endl;
                os << "\t\t. CL_DEVICE_EXECUTION_CAPABILITIES        : " << device.getInfo<CL_DEVICE_EXECUTION_CAPABILITIES>() << std::endl;
                os << "\t\t. CL_DEVICE_QUEUE_PROPERTIES              : " << device.getInfo<CL_DEVICE_QUEUE_PROPERTIES>() << std::endl;
                count_device++;
            }
            count_platform++;
        }
    }
    catch (cl::Error e)
    {
        os << e.what() << ": Error code " << e.err() << std::endl;
    }
    std::ofstream outFile("OpenCL_info.txt");
    outFile << os.str() << std::endl;
    outFile.close();
    return 0;
}
