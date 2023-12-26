#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

size_t size;
const char* source =
	R"(
kernel void square(global unsigned int *array, const int size)
{
	int id = get_global_id(0);
	array[id] *= 2;
}
)";

cl_command_queue queue;
cl_kernel square_kernel;
cl_mem array_buffer, size_buffer;

int main(int argc, char* argv[]) {
	if (argc < 2) return -1;
	size = atoi(argv[1]);
	if (size < 1) return -1;

	int size_bytes = size * sizeof(cl_int);
	cl_int* array = malloc(size_bytes);
	arc4random_buf(array, size_bytes);

	cl_context context =
		clCreateContextFromType(NULL, CL_DEVICE_TYPE_GPU, NULL, NULL, NULL);

	cl_device_id device;
	clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device), &device, 0);

	queue = clCreateCommandQueueWithProperties(context, device, 0, NULL);

	cl_program program =
		clCreateProgramWithSource(context, 1, &source, NULL, NULL);

	cl_int ret = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	if (ret != CL_SUCCESS) {
		printf("clBuildProgram failed: %d\n", ret);

		char buf[0x10000];

		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0x10000,
							  buf, NULL);
		printf("\n%s\n", buf);
		return (-1);
	}

	array_buffer =
		clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
					   size_bytes, array, NULL);
	size_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
								 sizeof(cl_int), &size, NULL);

	square_kernel = clCreateKernel(program, "square", NULL);
	clSetKernelArg(square_kernel, 0, sizeof(array_buffer), &array_buffer);
	clSetKernelArg(square_kernel, 1, sizeof(size_buffer), &size_buffer);

	clEnqueueNDRangeKernel(queue, square_kernel, 1, NULL, &size, NULL, 0, NULL,
						   0);
	clEnqueueReadBuffer(queue, array_buffer, 1, 0, size_bytes, array, 0, 0, 0);
	clFinish(queue);
}
