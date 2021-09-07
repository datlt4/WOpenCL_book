# WOpenCL_book

# Install OpenCL

## On Ubuntu

1. Install nvidia-driver, 

- After install nvidia-driver, header files located at `/usr/local/cuda/include/CL`, and cd

- And at `/usr/local/cuda/lib64`
```
/usr/local/cuda/lib64/libOpenCL.so -> libOpenCL.so.1*
/usr/local/cuda/lib64/libOpenCL.so.1 -> libOpenCL.so.1.0*
/usr/local/cuda/lib64/libOpenCL.so.1.0 -> libOpenCL.so.1.0.0*
/usr/local/cuda/lib64/libOpenCL.so.1.0.0*
```

<details>
  <summary>Makefile</summary>

```Makefile
PROJ=getInfo
CXX = g++
CXXFLAGS=-std=c++11
INC_DIRS=-I /usr/local/cuda/include
LIB_DIRS=-L /usr/local/cuda/lib64/libOpenCL.so

LIBS=-lOpenCL

$(PROJ): $(PROJ).cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LIBS) $(INC_DIRS) $(LIB_DIRS)

.PHONY: clean

clean:
	rm -f $(PROJ) OpenCL_info.txt
```

</details>

2. Install OpenCL for Intel onboard GPU

```bash
sudo add-apt-repository ppa:intel-opencl/intel-opencl # for Ubuntu 18.04
sudo apt update
sudo apt install --assume-yes intel-opencl-icd
# sudo apt install --assume-yes libze-loader libze-intel-gpu
```

3. Apt

```
sudo apt update
sudo apt install ocl-icd-opencl-dev
```
- Now libOpenCL.so should be located at /usr/lib/x86_64-linux-gnu/libOpenCL.so


## On Jetson

- By default, Jetson boards don't support OpenCL.
- Start by downloading and extracting Khronos Group's OpenCL Headers from [github](https://github.com/KhronosGroup/OpenCL-Headers).
- Once extracted move the `CL/` directory into `/usr/include`:

```bash
git clone https://github.com/KhronosGroup/OpenCL-Headers.git
sudo mv OpenCL-Headers/CL /usr/include
sudo wget https://www.khronos.org/registry/OpenCL/api/2.1/cl.hpp -P /usr/include/CL
```

- Now if you go into `/usr/lib/aarch64-linux-gnu` you can find the OpenCL library as `libOpenCL.so.1`. We need to add a symbolic link from `libOpenCL.so.1` to `libOpenCL.so`:

```bash
cd /usr/lib/aarch64-linux-gnu
sudo ln -s libOpenCL.so.1 libOpenCL.so
```

<details>
  <summary>Makefile</summary>

```Makefile
PROJ=getInfo
CXX = g++
CXXFLAGS=-std=c++11
INC_DIRS=-I /usr/include
LIB_DIRS=-L /usr/lib/aarch64-linux-gnu/libOpenCL.so

LIBS=-lOpenCL

$(PROJ): $(PROJ).cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LIBS) $(INC_DIRS) $(LIB_DIRS)

.PHONY: clean

clean:
	rm -f $(PROJ) OpenCL_info.txt
```

# Reference:

- [__OpenCL in action How to accelerate graphics and computations - Matthew Scarpino - November 2011__](https://www.manning.com/books/opencl-in-action)

# Part 1: Foundations of OpenCL programming

## [Chapter 1: Introducing OpenCL](OpenCL_in_action/chapter-1.md)

- 1.1 The dawn of OpenCL
- [1.2 Why OpenCL?](OpenCL_in_action/chapter-1.md/#Introducing-OpenCL)
    - 1.2.1 Portability
    - 1.2.2 Standardized vector processing
    - 1.2.3 Parallel programming
- 1.3 Analogy: OpenCL processing and a game of cards
- 1.4 A first look at an OpenCL application
- 1.5 The OpenCL standard and extensions
- 1.6 Frameworks and software development kits (SDKs)
- 1.7 Summary


## [Chapter 2: Host programming: fundamental data structures](OpenCL_in_action/chapter-2.md#chapter-2-host-programming--fundamental-data-structures)
- [2.1 Primitive data types](OpenCL_in_action/chapter-2.md#21-primitive-data-types)
- [2.2 Accessing platforms](OpenCL_in_action/chapter-2.md#22-accessing-platforms)
    - [2.2.1 Creating platform structures](OpenCL_in_action/chapter-2.md#221-creating-platform-structures)
    - [2.2.2 Obtaining platform information](OpenCL_in_action/chapter-2.md#222-obtaining-platform-information)
    - [2.2.3 Code example: testing platform extensions](OpenCL_in_action/chapter-2.md#223-code-example-testing-platform-extensions)
- [2.3 Accessing installed devices](OpenCL_in_action/chapter-2.md#23-accessing-installed-devices)
    - [2.3.1 Creating device structures](OpenCL_in_action/chapter-2.md#231-creating-device-structures)
    - [2.3.2 Obtaining device information](OpenCL_in_action/chapter-2.md#232-obtaining-device-information)
    - [2.3.3 Code example: testing device extensions](OpenCL_in_action/chapter-2.md#233-code-example-testing-device-extensions)
- [2.4 Managing devices with contexts](OpenCL_in_action/chapter-2.md#24-managing-devices-with-contexts)
    - [2.4.1 Creating contexts](OpenCL_in_action/chapter-2.md#241-creating-contexts)
    - [2.4.2 Obtaining context information](OpenCL_in_action/chapter-2.md#242-obtaining-context-information)
    - [2.4.3 Contexts and the reference count](OpenCL_in_action/chapter-2.md#243-contexts-and-the-reference-count)
    - [2.4.4 Code example: checking a context’s reference count](OpenCL_in_action/chapter-2.md#244-code-example-checking-a-contexts-reference-count)
- [2.5 Storing device code in programs](OpenCL_in_action/chapter-2.md#25-storing-device-code-in-programs)
    - [2.5.1 Creating programs](OpenCL_in_action/chapter-2.md#251-creating-programs)
    - [2.5.2 Building programs](OpenCL_in_action/chapter-2.md#252-building-programs)
    - [2.5.3 Obtaining program information](OpenCL_in_action/chapter-2.md#253-obtaining-program-information)
    - [2.5.4 Code example: building a program from multiple source files](OpenCL_in_action/chapter-2.md#254-code-example-building-a-program-from-multiple-source-files)
- [2.6 Packaging functions in kernels](OpenCL_in_action/chapter-2.md#26-packaging-functions-in-kernels)
    - [2.6.1 Creating kernels](OpenCL_in_action/chapter-2.md#261-creating-kernels)
    - [2.6.2 Obtaining kernel information](OpenCL_in_action/chapter-2.md#262-obtaining-kernel-information)
    - [2.6.3 Code example: obtaining kernel information](OpenCL_in_action/chapter-2.md#263-code-example-obtaining-kernel-information)
- [2.7 Collecting kernels in a command queue](OpenCL_in_action/chapter-2.md#27-collecting-kernels-in-a-command-queue)
    - [2.7.1 Creating command queues](OpenCL_in_action/chapter-2.md#271-creating-command-queues)
    - [2.7.2 Enqueuing kernel execution commands](OpenCL_in_action/chapter-2.md#272-enqueuing-kernel-execution-commands)
    - [2.7.3 Example code](OpenCL_in_action/chapter-2.md#273-example-code)
- [2.8 Summary](OpenCL_in_action/chapter-2.md#28-summary)

## [Chapter 3: Host programming: data transfer and partitioning](OpenCL_in_action/chapter-3.md#chapter-3-host-programming--data-transfer-and-partitioning)
- [3.1 Setting kernel arguments](OpenCL_in_action/chapter-3.md#31-setting-kernel-arguments)
- [3.2 Buffer objects](OpenCL_in_action/chapter-3.md#32-buffer-objects)
    - [3.2.1 Allocating buffer objects](OpenCL_in_action/chapter-3.md#321-allocating-buffer-objects)
    - [3.2.2 Creating subbuffer objects](OpenCL_in_action/chapter-3.md#322-creating-subbuffer-objects)
- [3.3 Image objects](OpenCL_in_action/chapter-3.md#33-image-objects)
    - [3.3.1 Creating image objects](OpenCL_in_action/chapter-3.md#331-creating-image-objects)
    - [3.3.2 Obtaining information about image objects](OpenCL_in_action/chapter-3.md#332-obtaining-information-about-image-objects)
- [3.4 Obtaining information about buffer objects](OpenCL_in_action/chapter-3.md#34-obtaining-information-about-buffer-objects)
- [3.5 Memory object transfer commands](OpenCL_in_action/chapter-3.md#35-memory-object-transfer-commands)
    - [3.5.1 Read/write data transfer](OpenCL_in_action/chapter-3.md#351-readwrite-data-transfer)
    - [3.5.2 Mapping memory objects](OpenCL_in_action/chapter-3.md#352-mapping-memory-objects)
    - [3.5.3 Copying data between memory objects](OpenCL_in_action/chapter-3.md#353-copying-data-between-memory-objects)
- [3.6 Data partitioning](OpenCL_in_action/chapter-3.md#36-data-partitioning)
    - [3.6.1 Loops and work-items](OpenCL_in_action/chapter-3.md#361-loops-and-work-items)
    - [3.6.2 Work sizes and offsets](OpenCL_in_action/chapter-3.md#362-work-sizes-and-offsets)
    - [3.6.3 A simple one-dimensional example](OpenCL_in_action/chapter-3.md#363-a-simple-one-dimensional-example)
    - [3.6.4 Work-groups and compute units](OpenCL_in_action/chapter-3.md#364-work-groups-and-compute-units)
- [3.7 Summary](OpenCL_in_action/chapter-3.md#37-summary)

## Chapter 4: Kernel programming: data types and device memory
- [4.1 Introducing kernel coding](OpenCL_in_action/chapter-4.md#41-Introducing-kernel-coding)
- [4.2 Scalar data types](OpenCL_in_action/chapter-4.md#42-Scalar-data-types)
    - [4.2.1 Accessing the double data type](OpenCL_in_action/chapter-4.md#421-Accessing-the-double-data-type)
    - [4.2.2 Byte order](OpenCL_in_action/chapter-4.md#422-Byte-order)
- [4.3 Floating-point computing](OpenCL_in_action/chapter-4.md#43-Floating-point-computing)
    - [4.3.1 The float data type](OpenCL_in_action/chapter-4.md#431-The-float-data-type)
    - [4.3.2 The double data type](OpenCL_in_action/chapter-4.md#432-The-double-data-type)
    - [4.3.3 The half data type](OpenCL_in_action/chapter-4.md#433-The-half-data-type)
    - [4.3.4 Checking IEEE-754 compliance](OpenCL_in_action/chapter-4.md#434-Checking-IEEE-754-compliance)
- [4.4 Vector data types](OpenCL_in_action/chapter-4.md#44-Vector-data-types)
    - [4.4.1 Preferred vector widths](OpenCL_in_action/chapter-4.md#441-Preferred-vector-widths)
    - [4.4.2 Initializing vectors](OpenCL_in_action/chapter-4.md#442-Initializing-vectors)
    - [4.4.3 Reading and modifying vector components](OpenCL_in_action/chapter-4.md#443-Reading-and-modifying-vector-components)
    - [4.4.4 Endianness and memory access](OpenCL_in_action/chapter-4.md#444-Endianness-and-memory-access)
- [4.5 The OpenCL device model](OpenCL_in_action/chapter-4.md#45-The-OpenCL-device-model)
    - [4.5.1 Device model analogy part 1: math students in school](OpenCL_in_action/chapter-4.md#451-Device-model-analogy-part-1-math-students-in-school)
    - [4.5.2 Device model analogy part 2: work-items in a device](OpenCL_in_action/chapter-4.md#452-Device-model-analogy-part-2-work-items-in-a-device)
    - [4.5.3 Address spaces in code](OpenCL_in_action/chapter-4.md#453-Address-spaces-in-code)
    - [4.5.4 Memory alignment](OpenCL_in_action/chapter-4.md#454-Memory-alignment)
- [4.6 Local and private kernel arguments](OpenCL_in_action/chapter-4.md#46-Local-and-private-kernel-arguments)
    - [4.6.1 Local arguments](OpenCL_in_action/chapter-4.md#461-Local-arguments)
    - [4.6.2 Private arguments](OpenCL_in_action/chapter-4.md#462-Private-arguments)
- [4.7 Summary](OpenCL_in_action/chapter-4.md#47-Summary)

## Chapter 5: Kernel programming: operators and functions
- 5.1 Operators
- 5.2 Work-item and work-group functions
    - 5.2.1 Dimensions and work-items
    - 5.2.2 Work-groups
    - 5.2.3 An example application
- 5.3 Data transfer operations
    - 5.3.1 Loading and storing data of the same type
    - 5.3.2 Loading vectors from a scalar array
    - 5.3.3 Storing vectors to a scalar array
- 5.4 Floating-point functions
    - 5.4.1 Arithmetic and rounding functions
    - 5.4.2 Comparison functions
    - 5.4.3 Exponential and logarithmic functions
    - 5.4.4 Trigonometric functions
    - 5.4.5 Miscellaneous floating-point functions
- 5.5 Integer functions
    - 5.5.1 Adding and subtracting integers
    - 5.5.2 Multiplication
    - 5.5.3 Miscellaneous integer functions
- 5.6 Shuffle and select functions
    - 5.6.1 Shuffle functions
    - 5.6.2 Select functions
- 5.7 Vector test functions
- 5.8 Geometric functions
- 5.9 Summary

## Chapter 6: Image processing
- 6.1 Image objects and samplers
    - 6.1.1 Image objects on the host: cl_mem
    - 6.1.2 Samplers on the host: cl_sampler
    - 6.1.3 Image objects on the device: image2d_t and image3d_t
    - 6.1.4 Samplers on the device: sampler_t
- 6.2 Image processing functions
    - 6.2.1 Image read functions
    - 6.2.2 Image write functions
    - 6.2.3 Image information functions
    - 6.2.4 A simple example
- 6.3 Image scaling and interpolation
    - 6.3.1 Nearest-neighbor interpolation
    - 6.3.2 Bilinear interpolation
    - 6.3.3 Image enlargement in OpenCL
- 6.4 Summary

## Chapter 7: Events, profiling, and synchronization
- 7.1 Host notification events
    - 7.1.1 Associating an event with a command
    - 7.1.2 Associating an event with a callback function
    - 7.1.3 A host notification example
- 7.2 Command synchronization events
    - 7.2.1 Wait lists and command events
    - 7.2.2 Wait lists and user events
    - 7.2.3 Additional command synchronization functions
    - 7.2.4 Obtaining data associated with events
- 7.3 Profiling events
    - 7.3.1 Configuring command profiling
    - 7.3.2 Profiling data transfer
    - 7.3.3 Profiling data partitioning
- 7.4 Work-item synchronization
    - 7.4.1 Barriers and fences
    - 7.4.2 Atomic operations
    - 7.4.3 Atomic commands and mutexes
    - 7.4.4 Asynchronous data transfer
- 7.5 Summary

## Chapter 8: Development with C++
- 8.1 Preliminary concerns
    - 8.1.1 Vectors and strings
    - 8.1.2 Exceptions
- 8.2 Creating kernels
    - 8.2.1 Platforms, devices, and contexts
    - 8.2.2 Programs and kernels
- 8.3 Kernel arguments and memory objects
    - 8.3.1 Memory objects
    - 8.3.2 General data arguments
    - 8.3.3 Local space arguments
- 8.4 Command queues
    - 8.4.1 Creating CommandQueue objects
    - 8.4.2 Enqueuing kernel-execution commands
    - 8.4.3 Read/write commands
    - 8.4.4 Memory mapping and copy commands
- 8.5 Event processing
    - 8.5.1 Host notification
    - 8.5.2 Command synchronization
    - 8.5.3 Profiling events
    - 8.5.4 Additional event functions
- 8.6 Summary

## Chapter 9: Development with Java and Python
- 9.1 Aparapi
    - 9.1.1 Aparapi installation
    - 9.1.2 The Kernel class
    - 9.1.3 Work-items and work-groups
- 9.2 JavaCL
    - 9.2.1 JavaCL installation
    - 9.2.2 Overview of JavaCL development
    - 9.2.3 Creating kernels with JavaCL
    - 9.2.4 Setting arguments and enqueuing commands
- 9.3 PyOpenCL
    - 9.3.1 PyOpenCL installation and licensing
    - 9.3.2 Overview of PyOpenCL development
    - 9.3.3 Creating kernels with PyOpenCL
    - 9.3.4 Setting arguments and executing kernels
- 9.4 Summary

## Chapter 10: General coding principles
- 10.1 Global size and local size
    - 10.1.1 Finding the maximum work-group size
    - 10.1.2 Testing kernels and devices
- 10.2 Numerical reduction
    - 10.2.1 OpenCL reduction
    - 10.2.2 Improving reduction speed with vectors
- 10.3 Synchronizing work-groups
- 10.4 Ten tips for high-performance kernels
- 10.5 Summary

# Part 2: Coding practical algorithms in OpenCL

## Chapter 11: Reduction and sorting
- 11.1 MapReduce
    - 11.1.1 Introduction to MapReduce
    - 11.1.2 MapReduce and OpenCL
    - 11.1.3 MapReduce example: searching for text
- 11.2 The bitonic sort
    - 11.2.1 Understanding the bitonic sort
    - 11.2.2 Implementing the bitonic sort in OpenCL
- 11.3 The radix sort
    - 11.3.1 Understanding the radix sort
    - 11.3.2 Implementing the radix sort with vectors
- 11.4 Summary

## Chapter 12: Matrices and QR decomposition
- 12.1 Matrix transposition
    - 12.1.1 Introduction to matrices
    - 12.1.2 Theory and implementation of matrix transposition
- 12.2 Matrix multiplication
    - 12.2.1 The theory of matrix multiplication
    - 12.2.2 Implementing matrix multiplication in OpenCL
- 12.3 The Householder transformation
    - 12.3.1 Vector projection
    - 12.3.2 Vector reflection
    - 12.3.3 Outer products and Householder matrices
    - 12.3.4 Vector reflection in OpenCL
- 12.4 The QR decomposition
    - 12.4.1 Finding the Householder vectors and R
    - 12.4.2 Finding the Householder matrices and Q
    - 12.4.3 Implementing QR decomposition in OpenCL
- 12.5 Summary

## Chapter 13: Sparse matrices
- 13.1 Differential equations and sparse matrices
- 13.2 Sparse matrix storage and the Harwell-Boeing collection
    - 13.2.1 Introducing the Harwell-Boeing collection
    - 13.2.2 Accessing data in Matrix Market files
- 13.3 The method of steepest descent
    - 13.3.1 Positive-definite matrices
    - 13.3.2 Theory of the method of steepest descent
    - 13.3.3 Implementing SD in OpenCL
- 13.4 The conjugate gradient method
    - 13.4.1 Orthogonalization and conjugacy
    - 13.4.2 The conjugate gradient method
- 13.5 Summary

## Chapter 14: Signal processing and the fast Fourier transform
- 14.1 Introducing frequency analysis
- 14.2 The discrete Fourier transform
    - 14.2.1 Theory behind the DFT
    - 14.2.2 OpenCL and the DFT
- 14.3 The fast Fourier transform
    - 14.3.1 Three properties of the DFT
    - 14.3.2 Constructing the fast Fourier transform
    - 14.3.3 Implementing the FFT with OpenCL
- 14.4 Summary

# Part 3 : Accelerating OpenGL with OpenCL

## Chapter 15: Combining OpenCL and OpenGL
- 15.1 Sharing data between OpenGL and OpenCL
    - 15.1.1 Creating the OpenCL context
    - 15.1.2 Sharing data between OpenGL and OpenCL
    - 15.1.3 Synchronizing access to shared data
- 15.2 Obtaining information
    - 15.2.1 Obtaining OpenGL object and texture information
    - 15.2.2 Obtaining information about the OpenGL context
- 15.3 Basic interoperability example
    - 15.3.1 Initializing OpenGL operation
    - 15.3.2 Initializing OpenCL operation
    - 15.3.3 Creating data objects
    - 15.3.4 Executing the kernel
    - 15.3.5 Rendering graphics
- 15.4 Interoperability and animation
    - 15.4.1 Specifying vertex data
    - 15.4.2 Animation and display
    - 15.4.3 Executing the kernel
- 15.5 Summary

## Chapter 16: Textures and renderbuffers
- 16.1 Image filtering
    - 16.1.1 The Gaussian blur
    - 16.1.2 Image sharpening
    - 16.1.3 Image embossing
- 16.2 Filtering textures with OpenCL
    - 16.2.1 The init_gl function
    - 16.2.2 The init_cl function
    - 16.2.3 The configure_shared_data function
    - 16.2.4 The execute_kernel function
    - 16.2.5 The display function
- 16.3 Summary

# Appendix

## appendix A: Installing and using a software development kit
- A.1 Understanding OpenCL SDKs
    - A.1.1 Checking device compliance
    - A.1.2 OpenCL header files and libraries
- A.2 OpenCL on Windows
    - A.2.1 Windows installation with an AMD graphics card
    - A.2.2 Building Windows applications with an AMD graphics card
    - A.2.3 Windows installation with an Nvidia graphics card
    - A.2.4 Building Windows applications with an Nvidia graphics card
- A.3 OpenCL on Linux
    - A.3.1 Linux installation with an AMD graphics card
    - A.3.2 Linux installation with an Nvidia graphics card
    - A.3.3 Building OpenCL applications for Linux
- A.4 OpenCL on Mac OS
- A.5 Summary

## appendix B: Real-time rendering with OpenGL
- B.1 Installing OpenGL
    - B.1.1 OpenGL installation on Windows
    - B.1.2 OpenGL installation on Linux
    - B.1.3 OpenGL installation on Mac OS
- B.2 OpenGL development on the host
    - B.2.1 Placing data in vertex buffer objects (VBOs)
    - B.2.2 Configuring vertex attributes
    - B.2.3 Compiling and deploying shaders
    - B.2.4 Launching the rendering process
- B.3 Shader development
    - B.3.1 Introduction to shader coding
    - B.3.2 Vertex shaders
    - B.3.3 Fragment shaders
- B.4 Creating the OpenGL window with GLUT
    - B.4.1 Configuring and creating a window
    - B.4.2 Event handling
    - B.4.3 Displaying a window
- B.5 Combining OpenGL and GLUT
    - B.5.1 GLUT/OpenGL initialization
    - B.5.2 Setting the viewport
    - B.5.3 Rendering the model
- B.6 Adding texture
    - B.6.1 Creating textures in the host application
    - B.6.2 Texture mapping in the vertex shader
    - B.6.3 Applying textures in the fragment shader
- B.7 Summary

## appendix C: The minimalist GNU for Windows and OpenCL
- C.1 Installing MinGW on Windows
    - C.1.1 Obtaining and running the graphical installer
    - C.1.2 Installing new tools in MinGW
- C.2 Building MinGW executables
    - C.2.1 Building Hello World! with MinGW
    - C.2.2 The GNU compiler
- C.3 Makefiles
    - C.3.1 Structure of a GNU makefile
    - C.3.2 Targets and phony targets
    - C.3.3 Simple example makefile
- C.4 Building OpenCL applications
- C.5 Summary

## appendix D: OpenCL on mobile devices
- D.1 Numerical processing
- D.2 Image processing
- D.3 Summary
