#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

int32_t *get_array(int count);

const char *source =
    "kernel void swap(global int *array)\n"
    "{\n"
    "        int index = get_global_id(0)*4;\n"
    // First pair
    "	int diff = max(array[index] - array[index+1], (int)0);\n"
    "	array[index]+=-diff;\n"
    "	array[index+1]+=diff;\n"

    "	diff = max(array[index+2] - array[index+3], (int)0);\n"
    "	array[index+2]+=-diff;\n"
    "	array[index+3]+=diff;\n"
    // Central pair
    "	diff = max(array[index+1] - array[index+2], (int)0);\n"
    "	array[index+1]+=-diff;\n"
    "	array[index+2]+=diff;\n"
    // Second pair
    "	diff = max(array[index] - array[index+1], (int)0);\n"
    "	array[index]+=-diff;\n"
    "	array[index+1]+=diff;\n"

    "	diff = max(array[index+2] - array[index+3], (int)0);\n"
    "	array[index+2]+=-diff;\n"
    "	array[index+3]+=diff;\n"
    "}\n"
    "kernel void swap_shifted(global int *array)\n"
    "{\n"
    "        int index = get_global_id(0)*4+2;\n"
    // First pair
    "	int diff = max(array[index] - array[index+1], (int)0);\n"
    "	array[index]+=-diff;\n"
    "	array[index+1]+=diff;\n"

    "	diff = max(array[index+2] - array[index+3], (int)0);\n"
    "	array[index+2]+=-diff;\n"
    "	array[index+3]+=diff;\n"
    // Central pair
    "	diff = max(array[index+1] - array[index+2], (int)0);\n"
    "	array[index+1]+=-diff;\n"
    "	array[index+2]+=diff;\n"
    // Second pair
    "	diff = max(array[index] - array[index+1], (int)0);\n"
    "	array[index]+=-diff;\n"
    "	array[index+1]+=diff;\n"

    "	diff = max(array[index+2] - array[index+3], (int)0);\n"
    "	array[index+2]+=-diff;\n"
    "	array[index+3]+=diff;\n"
    "}\n";

const char *errCodeToString(int err) {
  switch (err) {
  case CL_SUCCESS:
    return "Success!";
  case CL_DEVICE_NOT_FOUND:
    return "Device not found.";
  case CL_DEVICE_NOT_AVAILABLE:
    return "Device not available";
  case CL_COMPILER_NOT_AVAILABLE:
    return "Compiler not available";
  case CL_MEM_OBJECT_ALLOCATION_FAILURE:
    return "Memory object allocation failure";
  case CL_OUT_OF_RESOURCES:
    return "Out of resources";
  case CL_OUT_OF_HOST_MEMORY:
    return "Out of host memory";
  case CL_PROFILING_INFO_NOT_AVAILABLE:
    return "Profiling information not available";
  case CL_MEM_COPY_OVERLAP:
    return "Memory copy overlap";
  case CL_IMAGE_FORMAT_MISMATCH:
    return "Image format mismatch";
  case CL_IMAGE_FORMAT_NOT_SUPPORTED:
    return "Image format not supported";
  case CL_BUILD_PROGRAM_FAILURE:
    return "Program build failure";
  case CL_MAP_FAILURE:
    return "Map failure";
  case CL_INVALID_VALUE:
    return "Invalid value";
  case CL_INVALID_DEVICE_TYPE:
    return "Invalid device type";
  case CL_INVALID_PLATFORM:
    return "Invalid platform";
  case CL_INVALID_DEVICE:
    return "Invalid device";
  case CL_INVALID_CONTEXT:
    return "Invalid context";
  case CL_INVALID_QUEUE_PROPERTIES:
    return "Invalid queue properties";
  case CL_INVALID_COMMAND_QUEUE:
    return "Invalid command queue";
  case CL_INVALID_HOST_PTR:
    return "Invalid host pointer";
  case CL_INVALID_MEM_OBJECT:
    return "Invalid memory object";
  case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
    return "Invalid image format descriptor";
  case CL_INVALID_IMAGE_SIZE:
    return "Invalid image size";
  case CL_INVALID_SAMPLER:
    return "Invalid sampler";
  case CL_INVALID_BINARY:
    return "Invalid binary";
  case CL_INVALID_BUILD_OPTIONS:
    return "Invalid build options";
  case CL_INVALID_PROGRAM:
    return "Invalid program";
  case CL_INVALID_PROGRAM_EXECUTABLE:
    return "Invalid program executable";
  case CL_INVALID_KERNEL_NAME:
    return "Invalid kernel name";
  case CL_INVALID_KERNEL_DEFINITION:
    return "Invalid kernel definition";
  case CL_INVALID_KERNEL:
    return "Invalid kernel";
  case CL_INVALID_ARG_INDEX:
    return "Invalid argument index";
  case CL_INVALID_ARG_VALUE:
    return "Invalid argument value";
  case CL_INVALID_ARG_SIZE:
    return "Invalid argument size";
  case CL_INVALID_KERNEL_ARGS:
    return "Invalid kernel arguments";
  case CL_INVALID_WORK_DIMENSION:
    return "Invalid work dimension";
  case CL_INVALID_WORK_GROUP_SIZE:
    return "Invalid work group size";
  case CL_INVALID_WORK_ITEM_SIZE:
    return "Invalid work item size";
  case CL_INVALID_GLOBAL_OFFSET:
    return "Invalid global offset";
  case CL_INVALID_EVENT_WAIT_LIST:
    return "Invalid event wait list";
  case CL_INVALID_EVENT:
    return "Invalid event";
  case CL_INVALID_OPERATION:
    return "Invalid operation";
  case CL_INVALID_GL_OBJECT:
    return "Invalid OpenGL object";
  case CL_INVALID_BUFFER_SIZE:
    return "Invalid buffer size";
  case CL_INVALID_MIP_LEVEL:
    return "Invalid mip-map level";
  default:
    return "Unknown";
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: ./parallel <N>\n");
    return -1;
  }

  int count = atoi(argv[1]);
  if (count < 2) {
    printf("Must be at least two elements\n");
    return -1;
  }

  count = (count + 3) & ~3;

  int32_t *array = get_array(count);

  cl_int ret;

  cl_uint qty_platforms = 0;
  ret = clGetPlatformIDs(0, NULL, &qty_platforms);
  if (ret != CL_SUCCESS) {
    fprintf(stderr, "Error, code = %d.\n", ret);
    return 1;
  }
  cl_platform_id *platforms =
      (cl_platform_id *)malloc(sizeof(cl_platform_id) * qty_platforms);
  cl_device_id **devices =
      (cl_device_id **)malloc(sizeof(cl_device_id *) * qty_platforms);
  cl_uint *qty_devices = (cl_uint *)malloc(sizeof(cl_uint) * qty_platforms);
  ret = clGetPlatformIDs(qty_platforms, platforms, NULL);
  for (int i = 0; i < qty_platforms; i++) {
    ret =
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU,
                       0, NULL, &qty_devices[i]);
    if (qty_devices[i]) {
      devices[i] =
          (cl_device_id *)malloc(qty_devices[i] * sizeof(cl_device_id));
      ret = clGetDeviceIDs(platforms[i],
                           CL_DEVICE_TYPE_GPU,
                           qty_devices[i], devices[i], NULL);
    }
  }
  cl_context context =
      clCreateContext(0, qty_devices[0], devices[0], NULL, NULL, &ret);

  //  cl_context context =
  //      clCreateContextFromType(NULL, CL_DEVICE_TYPE_GPU, NULL, NULL, &ret);

  cl_device_id device;
  clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device), &device, 0);

  cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);
  size_t zero = 0;
  cl_program program =
      clCreateProgramWithSource(context, 1, &source, &zero, &ret);

  ret = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
  if (ret != CL_SUCCESS) {
    printf("clBuildProgram failed: %d, %s\n", ret, errCodeToString(ret));

    char buf[0x10000];

    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0x10000, buf,
                          NULL);
    printf("\n%s\n", buf);
    return -1;
  }

  cl_mem array_buffer =
      clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                     sizeof(*array) * count, array, NULL);

  cl_kernel swap_kernel = clCreateKernel(program, "swap", NULL);
  clSetKernelArg(swap_kernel, 0, sizeof(cl_mem), &array_buffer);
  cl_kernel swap_shifted_kernel = clCreateKernel(program, "swap_shifted", NULL);
  clSetKernelArg(swap_shifted_kernel, 0, sizeof(cl_mem), &array_buffer);

  clFinish(queue);

  struct timeval start, end;
  gettimeofday(&start, 0);

  size_t offset = 0, size = count / 4, size_shifted = size - 1;
  for (int i = 0; i < count / 4; i++) {
    clEnqueueNDRangeKernel(queue, swap_shifted_kernel, 1, &offset,
                           &size_shifted, NULL, 0, NULL, NULL);
    clEnqueueNDRangeKernel(queue, swap_kernel, 1, &offset, &size, NULL, 0, NULL,
                           NULL);
  }
  clFinish(queue);

  gettimeofday(&end, 0);
  double time_result =
      (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
  printf("Time: %lf\n", time_result);

  clEnqueueReadBuffer(queue, array_buffer, CL_TRUE, 0, sizeof(*array) * count,
                      array, 0, 0, 0);

  FILE *file = fopen("result_parallel", "w+");
  if (file) {
    for (int i = 0; i < count; i++)
      fprintf(file, "%d\n", array[i]);
    fclose(file);
  }
  free(array);
  return 0;
}

int32_t *get_array(int count) {
  if (getenv("SEED"))
    srand(atoi(getenv("SEED")));
  else
    srand((0));
  int32_t *result = (int32_t *)malloc(sizeof(int32_t) * count);
  for (int i = 0; i < count; i++)
    result[i] = rand();
  return result;
}
