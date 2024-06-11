// Enable OpenCL exceptions
#define __CL_ENABLE_EXCEPTIONS

#include <vector>   // For using std::vector
#include <iostream> // For standard input/output
#include <string>
#ifdef __APPLE__
#include <OpenCL/cl.hpp> // OpenCL 1.2 for macOS
#else
#include "CL/cl.hpp" // OpenCL for other platforms
#endif

int main(int argc, char **argv)
{
    /* Extension data */
    const std::string icd_ext = "cl_khr_icd";

    /* Access all installed platforms */
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty())
    {
        std::cerr << "Couldn't find any platforms." << std::endl;
    }
    /* Find extensions of all platforms */
    cl_uint platform_index = 0;
    for (cl::Platform &platform : platforms)
    {
        /* Access extension data */
        cl::STRING_CLASS param;
        platform.getInfo(CL_PLATFORM_EXTENSIONS, &param);
        std::cout << "Platform supports extensions: " << param << std::endl;

        /* Look for ICD extension */
        if (param.find(icd_ext) != param.npos)
        {
            /* Display whether ICD extension is supported */
            std::cout << "Platform " << platform_index << " supports the " << icd_ext << " extension." << std::endl;
        }
        else
        {
            std::cout << "No platform support the " << icd_ext << " extension." << std::endl;
        }
        ++platform_index;
    }

    return EXIT_SUCCESS;
}