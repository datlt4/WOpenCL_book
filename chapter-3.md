# Chapter 3: Host programming : data transfer and partitioning

## 3.1 Setting kernel arguments

- Tại chương 2, ta đã biết cách tạo ra các `kernels` từ các `functions`, nhưng chưa đề cập tới việc set `arguments` cho các hàm. Hàm `clSetKernelArg` cho phép thực hiện điều đó.

```cpp
clSetKernelArg (cl_kernel kernel, cl_uint index, size_t size, const void *value);
```
where:
> `index`: xác định thứ tự của kernel argument trong danh sách các tham số của kernel function.<br>
> Nếu `index = 0` - first argument, `index = 1` - second argument.<br>
> `value`: con trỏ trỏ tới `data` sẽ được truyền vào kernel function. Con trỏ có thể trỏ tới các dạng dữ liệu sau:

|Pointer to|Description|
|---|---|
|Pointer to primitive data|Transfers simple primitives to the device|
|Pointer to a memory object|Transfers significant or complex data|
|Pointer to a sampler object|Transfers an object that defines how images are read|
|`NULL`|Transfers no data from the host; the device will just reserve memory in its local address space for the kernel argument|

- Example

```cpp
clSetKernelArg(proc, 0, sizeof(num), &num);
clSetKernelArg(proc, 1, sizeof(mem_obj), &mem_obj);
```

- Trong OpenCL, `memory objects` được biểu diễn bằng structure `cl_mem`, và chúng được chia thành 2 loại: `buffer objects` và `image objects`. Nếu `Memory objects` để lưu pixel data -> sử dụng `image objects`. Còn trong các trường hợp khác, ta nên lưu data trong `buffer objects`.

## 3.2 Buffer objects

- `Buffer objects` có thể gói bất kỳ loại dữ liệu nào mà không liên quan tới images. Khởi tạo bằng hàm `clCreateBuffer`:

```cpp
clCreateBuffer(cl_context context, cl_mem_flags options, size_t size, void *host_ptr, cl_int *error)
```
where:
> `@return`: trả về một `cl_mem`, cái mà chứ data được trỏ bở `host_ptr *`

<a name="table_memory_object_properties"></a>
|`cl_mem_flags`|Meaning|
|:---:|---|
|`CL_MEM_READ_WRITE`|The memory object can be read from and written to.|
|`CL_MEM_WRITE_ONLY`|The memory object can only be written to.|
|`CL_MEM_READ_ONLY`|The memory object can only be read from.|
|`CL_MEM_USE_HOST_PTR`|The memory object will access the memory region specified by the host pointer.|
|`CL_MEM_COPY_HOST_PTR`|The memory object will set the memory region specified by the host pointer.|
|`CL_MEM_ALLOC_HOST_PTR`|A region in host-accessible memory will be allocated for use in data transfer.|

- Ba properties đầu tiên xác định khả năng truy cập của `buffer object` - rằng buộc khả năng `devices` truy cập vào `buffer object`, không phải `host`.

- Ba properties tiếp theo, xác định cách mà một `buffer object` được allocated trong `host memory`.

- Khi set tham sô thứ 2 của hàm `clCreateBuffer`, ta thường tổ hợp của 2 flag. Một là một trong bộ 3 đầu - giúp cài đặt khả năng truy cập của `buffer object`, còn flag thứ 2 là một trong bộ 3 thứ 2 - xác định nơi `buffer object` được allocated.

### 3.2.1 Allocating buffer objects

- Với ví dụ dưới đây, một `buffer object` tên là `vec_buff` được tạo as `read-only`. `Buffer` này sẽ gói data được tham chiếu bởi `vec` và data bản đầu được allocated ở `host`, nên `vec` được gọi là `host pointer`.

```cpp
vec_buff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float)*32, vec, &error);
```

- "Tạo một `buffer object` để nắm dữ liệu đầu ra của `kernel`":
  - Sử dụng cờ `CL_MEM_WRITE_ONLY` để tạo một `buffer` chỉ `device` có thể viết vào nó.
  - Vì `device` allocate memory chứ không phải `host`, có nên ta set tham số `host pointer` là `NULL`.

- "Truyền data từ `host` sang `device`":
  - `host pointer` khác `NULL`.
  - Trong trường hợp này, cần xác định nơi mà `buffer object` cần được allocated.
    - Nếu `buffer object` truy cập vào vùng nhớ được tham chiếu bởi `host pointer`, dùng cờ `CL_MEM_USE_HOST_PTR`. Cách này tối ưu bộ nhớ, nhưng có một nhược điểm là việc truyền nhận dữ liệu giữa `host` và `devices` không thể dự đoán được.
    - Bên cạnh đó, ta có thể allocate memory ở một nơi khác và copy data từ `host pointer` sang nơi mới - sử dụng cờ `CL_MEM_COPY_HOST_PTR`. Các này không tối ưu bộ nhớ, nhưng nó cho phép ta có thể thay đổi `host pointer memory` ngay cả khi `host` đang trao đổi dữ liệu với `devices`.
    - Cờ `CL_MEM_ALLOC_HOST_PTR` có thể được sử dụng tổ hợp với `CL_MEM_COPY_HOST_PTR`, giúp rằng buộc new memory region là `host-accessible`.

- Example code:

```cpp
input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(input_vector), input_vector, &error);
output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(input_vector), NULL, &error);

clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer);
clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_buffer);
```

### 3.2.2 Creating subbuffer objects

- Giống như ta có thể tạo ra một `substring` từ một `string`, thì ta có thể tạo ra một `subbuffer object` từ một `buffer object`.

```cpp
clCreateSubBuffer(cl_mem buffer, cl_mem_flags flags, cl_buffer_create_type type, const void *info, cl_int *error)
```
where:
> `flags`: giống như [bảng](#table_memory_object_properties).<br>
> `type`: luôn luôn là `CL_BUFFER_CREATE_TYPE_REGION`.<br>
> `info`: con trỏ tới một `_cl_buffer_region` structure.

- Structure `_cl_buffer_region` được định nghĩa như sau :

```cpp
typedef struct _cl_buffer_region {
  size_t origin;  // the start of the subbuffer's data inside the buffer.
  size_t size;    // the size of the subbuffer.
} cl_buffer_region;
```

- Example code:

```cpp
cl_buffer_region region;
region.size = 40*sizeof(float);
region.origin = 50*sizeof(float);

sub_buffer = clCreateSubBuffer(main_buffer, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, CL_BUFFER_CREATE_TYPE_REGION, &region, &err);
```

## 3.3 Image objects

- `OpenCL` cung cấp một loại `memory objects` xác định để lưu `pixel data`.

- `Image objects` cũng được biểu diễn bằng một structure `cl_mem`.

### 3.3.1 Creating image objects

- `Image objects` bao gồm 2 loại: `2-d` và `3-d`.
  - `Two dimensional image objects`: được tạo bởi hàm `clCreateImage2D`.
  - `Three dimensional image objects`: được tạo bởi hàm `clCreateImage3D`.
  - Cả 2 hàm đều trả về một `cl_mem`.

```cpp
clCreateImage2D (cl_context context, cl_mem_flags opts, const cl_image_format *format, size_t width, size_t height, size_t row_pitch, void *data, cl_int *error)
clCreateImage3D (cl_context context, cl_mem_flags opts, const cl_image_format *format, size_t width, size_t height, size_t depth, size_t row_pitch, size_t slice_pitch, void *data, cl_int *error)
```
where:
> `context`, `opts`: được sử dụng để tạo `buffer objects`.<br>
> `format` (`const` [`cl_image_format`](#image_object_format)): xác định format của image data.<br>
> Các tham số còn lại (ngoại từ `error`): xác định `dimensions` và `pitches` của pixels trong ảnh.

#### IMAGE OBJECT FORMATS

- `format`: xác định cách mà các pixel của ảnh được lưu trữ trong memory.
<a name="image_object_format"></a>
```cpp
typedef struct _cl_image_format {
  cl_channel_order image_channel_order;
  cl_channel_type image_channel_data_type;
} cl_image_format;
```
where:
> `cl_channel_order`: định nghĩa cách biểu diễn các kênh theo từng pixel:
red, green, blue, and alpha channels: `CL_RGB`, `CL_RGBA`, `CL_ARGB`, `CL_BGRA`, `CL_RG`, `CL_RA`, `CL_R`, `CL_A`. Add bit padding: `CL_RGBx`, `CL_RGx`, and `CL_Rx`. `CL_INTENSITY` measures alpha independent of color. `CL_LUMINANCE` is used for grayscale images.<br>
> `cl_channel_type`: xác định cách các image's channel biểu diễn từng bit. Bao gồm: định dạng số từng kênh, số bit mỗi kênh.

|Image channel types|Meaning|
|:---:|---|
|`CL_HALF_FLOAT`|Each component is floating-point (16 bits).|
|`CL_FLOAT`|Each component is floating-point (32 bits).|
|`CL_UNSIGNED_INT8`|Each component is an unsigned integer (8 bits).|
|`CL_UNSIGNED_INT16`|Each component is an unsigned integer (16 bits).|
|`CL_UNSIGNED_INT32`|Each component is an unsigned integer (32 bits).|
|`CL_SIGNED_INT8`|Each component is a signed integer (8 bits).|
|`CL_SIGNED_INT16`|Each component is a signed integer (16 bits).|
|`CL_SIGNED_INT32`|Each component is a signed integer (32 bits).|
|`CL_UNORM_INT8`|Each component is a normalized unsigned integer (8 bits).|
|`CL_UNORM_INT16`|Each component is a normalized unsigned integer (16 bits).|
|`CL_SNORM_INT8`|Each component is a normalized signed integer (8 bits).|
|`CL_SNORM_INT16`|Each component is a normalized signed integer (16 bits).|
|`CL_UNORM_SHORT_565`|The RGB components are combined into a normalized 16-bit format (5-6-5).|
|`CL_UNORM_SHORT_555`|The xRGB components are combined into a normalized 16-bit format (x-5-5-5).|
|`CL_UNORM_INT_101010`|The xRGB components are combined into a normalized 32-bit format (x-10-10-10).|

- Thông thường 24-bit RGB color model được biểu diễn bởi `CL_UNSIGNED_INT8`, với mỗi 8-bit để lưu từng kênh. High-Color formats sử dụng `CL_UNORM_SHORT_565` thêm 1 extra bit ở kênh green. Định dạng ảnh 30-bit Deeo Color sử dụng `CL_UNORM_INT_101010` flag.

- Example code: Khởi tạo một `cl_image_format` structure chứa các pixels được định dạng theo 24-bit RGB.

```cpp
cl_image_format rgb_format;
rgb_format.image_channel_order = CL_RGB;
rgb_format.image_channel_data_type = CL_UNSIGNED_INT8;
```

#### IMAGE OBJECT DIMENSIONS AND PITCH

- Các tham số còn lại trong hàm `clCreateImage2D` và `clCreateImage3D` quy định chiều của `image object` và số bytes mỗi chiều (`pitch`). Mỗi chiều được tính theo `pixel`.

- `row_pitch`: xác định có bao nhiêu bytes trên từng hàng.

- `slice_pitch`: số bytes trên mỗi ảnh 2-d (mỗi `slice`).

- "If row_pitch is set to 0, OpenCL will assume its value equals width * (pixel size). If slice_pitch is set to 0, its value will be set to row_pitch * height. In this book's example code, row_pitch and slice_pitch will always be set to 0." (_cited: [OpenCL in Action How to Accelerate Graphics and Computations](https://www.manning.com/books/opencl-in-action) - Chapter 3 Host programming: data transfer and partitioning._
)

[<img src= "images/F3_2.png" width="522">]()

- Example code: tạo một ảnh 3-d chứa 4 slices, mỗi slice có kích thước `64x80` pixels, định dạng màu `rgb`, đây là một `read-only image object`. Trong trường hợp set cờ `CL_MEM_WRITE_ONLY`, `object data` bằng `NULL`.

```cpp
#define NUM_ROWS 64
#define NUM_COLS 80
#define NUM_SLICES 4

unsigned char image_data[NUM_SLICES][NUM_ROWS][NUM_COLS];
cl_image_format rgb_format;
rgb_format.image_channel_order = CL_RGB;
rgb_format.image_channel_data_type = CL_UNSIGNED_INT8;

cl_mem image_object = clCreateImage3D (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, rgb_format, NUM_COLS, NUM_ROWS, NUM_SLICES, NULL, NULL, image_data, &error);
```

### 3.3.2 Obtaining information about image objects

- Hàm `getImageInfo` cung cấp thông tin của `image object` về chiều và định dạng pixel.

```cpp
clGetImageInfo (cl_mem image, cl_image_info param_name, size_t param_value_size, void *param_value, size_t *param_value_size_ret)
```

|`cl_image_info`|Parameter value|Purpose|
|:---:|:---:|---|f
|`CL_IMAGE_ELEMENT_SIZE`|`size_t`|Returns the bit size of the elements (pixels) that make up the image|
|`CL_IMAGE_WIDTH`|`size_t`|Returns the pixel width|
|`CL_IMAGE_HEIGHT`|`size_t`|Returns the pixel height|
|`CL_IMAGE_DEPTH`|`size_t`|Returns the depth of a 3-D image (the number of 2-D components)|
|`CL_IMAGE_ROW_PITCH`|`size_t`|Returns the row pitch (the number of bytes per row)|
|`CL_IMAGE_SLICE_PITCH`|`size_t`|Returns the slice pitch of a 3-D image (the number of bytes per 2-D component)|
|`CL_IMAGE_FORMAT`|`cl_image_format`|Returns the data structure that sets the image’s channel/pixel format|
|`CL_IMAGE_D3D10_SUBRESOURCE_KHR`|`ID3D10 Resource*`|Returns a pointer to the Direct3D subresource used to create the image object|

## 3.4 Obtaining information about buffer objects

- Trong khi `clGetImageInfo` chỉ cung cấp thông tin về `image objects`, ta có thể lấy thông tin của cả `image objects` và `buffer objects` bằng `clGetMemObjectInfo`:

```cpp
clGetMemObjectInfo(cl_mem object, cl_mem_info param_name, size_t param_value_size, void *param_value, size_t *param_value_size_ret);
```
where:
> 3 tham số đầu tiên: lần lượt là `memory objects`, tên xác định loại thông tin cần tìm, số lượng data yêu cầu.<br>
> 2 tham số cuối là tham số đầu ra

|`cl_mem_info`|`Parameter value`|Purpose|
|:---:|:---:|---|
|`CL_MEM_TYPE`|`cl_mem_object_type`|Returns the type of the memory object (`CL_MEM_OBJECT_BUFFER`, `CL_MEM_OBJECT_IMAGE2D`, or `CL_MEM_OBJECT_IMAGE3D`)|
|`CL_MEM_FLAGS`|`cl_mem_flags`|Returns the flags used to configure the memory object’s accessibility and allocation|
|`CL_MEM_HOST_PTR`|`void*`|Returns the host pointer that references the memory object’s data|
|`CL_MEM_SIZE`|`size_t`|Returns the size of the memory object’s data|
|`CL_MEM_CONTEXT`|`cl_context`|Returns the context associated with the memory object|
|`CL_MEM_ASSOCIATED_MEMOBJECT`|`cl_mem`|Returns the memory object from which this memory object was created (only valid for subbuffer objects)|
|`CL_MEM_OFFSET`|`size_t`|Returns the offset used to create the subbuffer object (only valid for subbuffer objects)|
|`CL_MEM_REFERENCE_COUNT`|`cl_uint`|Returns the memory object’s reference count (the number of times the object has been accessed)|
|`CL_MEM_D3D10_RESOURCE_KHR`|`ID3D10Resource*`|Returns a pointer to the OpenCL-Direct3D interface|

- Hàm này đặt biệt hữu ích khi ta muốn kiểm tra kích thước và địa chỉ của `memory object's data`.

<details>
  <summary>EXAMPLE CODE!</summary>

`buffer_check.c`

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device() {

   cl_platform_id platform;
   cl_device_id dev;
   int err;

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   } 

   /* Access a device */
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

   return dev;
}

int main() {

   /* Host/device data structures */
   cl_device_id device;
   cl_context context;
   cl_int err;

   /* Data and buffers */
   float main_data[100];
   cl_mem main_buffer, sub_buffer;
   void *main_buffer_mem = NULL, *sub_buffer_mem = NULL;
   size_t main_buffer_size, sub_buffer_size;
   cl_buffer_region region;
   
   /* Create device and context */
   device = create_device();
   context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
   if(err < 0) {
      perror("Couldn't create a context");
      exit(1);   
   }

   /* Create a buffer to hold 100 floating-point values */
   main_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | 
      CL_MEM_COPY_HOST_PTR, sizeof(main_data), main_data, &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   }

   /* Create a sub-buffer */
   /* Modified on 2/12/2014 to account for unaligned memory error */
   region.origin = 0x100;
   region.size = 20*sizeof(float);
   sub_buffer = clCreateSubBuffer(main_buffer, CL_MEM_READ_ONLY |
      CL_MEM_COPY_HOST_PTR, CL_BUFFER_CREATE_TYPE_REGION, &region, &err);
   if(err < 0) {
      perror("Couldn't create a sub-buffer");
      exit(1);   
   }

   /* Obtain size information about the buffers */
   clGetMemObjectInfo(main_buffer, CL_MEM_SIZE, 
         sizeof(main_buffer_size), &main_buffer_size, NULL);
   clGetMemObjectInfo(sub_buffer, CL_MEM_SIZE, 
         sizeof(sub_buffer_size), &sub_buffer_size, NULL);
   printf("Main buffer size: %lu\n", main_buffer_size);
   printf("Sub-buffer size:  %lu\n", sub_buffer_size);
   
   /* Obtain the host pointers */
   clGetMemObjectInfo(main_buffer, CL_MEM_HOST_PTR, sizeof(main_buffer_mem), 
  	      &main_buffer_mem, NULL);
   clGetMemObjectInfo(sub_buffer, CL_MEM_HOST_PTR, sizeof(sub_buffer_mem), 
  	      &sub_buffer_mem, NULL);
   printf("Main buffer memory address: %p\n", main_buffer_mem);
   printf("Sub-buffer memory address:  %p\n", sub_buffer_mem);

   /* Print the address of the main data */
   printf("Main array address: %p\n", main_data);

   /* Deallocate resources */
   clReleaseMemObject(main_buffer);
   clReleaseMemObject(sub_buffer);
   clReleaseContext(context);

   return 0;
}
```

`Makefile`

```cpp
PROJ=buffer_check

CC=gcc

CFLAGS=-std=c99 -Wall -DUNIX -g -DDEBUG

# Check for 32-bit vs 64-bit
PROC_TYPE = $(strip $(shell uname -m | grep 64))
 
# Check for Mac OS
OS = $(shell uname -s 2>/dev/null | tr [:lower:] [:upper:])
DARWIN = $(strip $(findstring DARWIN, $(OS)))

# MacOS System
ifneq ($(DARWIN),)
	CFLAGS += -DMAC
	LIBS=-framework OpenCL

	ifeq ($(PROC_TYPE),)
		CFLAGS+=-arch i386
	else
		CFLAGS+=-arch x86_64
	endif
else

# Linux OS
LIBS=-lOpenCL
ifeq ($(PROC_TYPE),)
	CFLAGS+=-m32
else
	CFLAGS+=-m64
endif

# Check for Linux-AMD
ifdef AMDAPPSDKROOT
	INC_DIRS=. $(AMDAPPSDKROOT)/include
	ifeq ($(PROC_TYPE),)
		LIB_DIRS=$(AMDAPPSDKROOT)/lib/x86
	else
		LIB_DIRS=$(AMDAPPSDKROOT)/lib/x86_64
	endif
else

# Check for Linux-Nvidia
ifdef NVSDKCOMPUTE_ROOT
	INC_DIRS=. $(NVSDKCOMPUTE_ROOT)/OpenCL/common/inc
endif

endif
endif

$(PROJ): $(PROJ).c
	$(CC) $(CFLAGS) -o $@ $^ $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(LIBS)

.PHONY: clean

clean:
	rm $(PROJ)
```

</details>

## 3.5 Memory object transfer commands

### 3.5.1 Read/write data transfer

### 3.5.2 Mapping memory objects

### 3.5.3 Copying data between memory objects

## 3.6 Data partitioning

### 3.6.2 Work sizes and offsets

### 3.6.3 A simple one-dimensional example

### 3.6.4 Work-groups and compute units

## 3.7 Summary