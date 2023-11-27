#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

size_t size;
const char* source =
	R"(
kernel void factor(global float *matrix, global int *rows, global int *index, global float *factor, const int size)
{
	int row = get_global_id(0);
	factor[row] = -matrix[size * rows[row] + *index] / matrix[size * rows[*index] + *index];
}
kernel void pass(global float *matrix, global int *rows, global int *index, global float *factor, const int size)
{
	int row = get_global_id(0);
	int col = get_global_id(1);
	matrix[size * rows[row] + col] += factor[row] * matrix[size * rows[*index] + col];
}
kernel void swap(global float *matrix, global int *rows, global int *index, const int size)
{
	(*index)++;
	int row = *index;
	if (matrix[size * rows[row] + row] != matrix[size * rows[row] + row] ||
		fabs(matrix[size * rows[row] + row]) < 1.0e-6) {
		int pivot_row = row;
		do {
			pivot_row++;
			if (pivot_row >= size) return;
		} while (matrix[size * rows[pivot_row] + row] != matrix[size * rows[pivot_row] + row] ||
			 fabs(matrix[size * rows[pivot_row] + row]) < 1.0e-6);
		int swap = rows[row];
		rows[row] = rows[pivot_row];
		rows[pivot_row] = swap;

		matrix[size * rows[*index - 1] + *index - 1] *= -1;
	}
}
kernel void sum(global float *matrix, global int *rows, global float *result, const int size)
{
	float res = 1;
	for (int i = 0; i < size; i++)
		res *= matrix[size * rows[i] + i];
	*result = res;
}
kernel void fill_rows(global int *rows)
{
	int id = get_global_id(0);
	rows[id] = id;
}
)";

cl_command_queue queue;
cl_kernel pass_kernel, swap_kernel, sum_kernel, factor_kernel, rows_kernel;
cl_mem rows_buffer, array_buffer, index_buffer, res_buffer, factor_buffer;

cl_int zero = 0;
size_t one = 1;

cl_float calculate_determinant(cl_float* matrix, int size) {
	size_t offset[2] = {1, 1}, worksize[2] = {size, size};
	for (int i = 0; i < size - 1; i++) {
		clEnqueueNDRangeKernel(queue, factor_kernel, 1, offset, worksize, NULL,
							   0, NULL, NULL);
		clEnqueueNDRangeKernel(queue, pass_kernel, 2, offset, worksize, NULL, 0,
							   NULL, NULL);
		if (i - 1 < size)
			clEnqueueNDRangeKernel(queue, swap_kernel, 1, NULL, &one, NULL, 0,
								   NULL, NULL);
		offset[0]++;
		offset[1]++;
	}
	clEnqueueNDRangeKernel(queue, sum_kernel, 1, NULL, &one, NULL, 0, NULL,
						   NULL);
	cl_float res;
	clEnqueueReadBuffer(queue, res_buffer, CL_TRUE, 0, sizeof(cl_float), &res,
						0, 0, 0);
	return res;
}

int main(int argc, char* argv[]) {
	if (argc < 2) return -1;
	size = atoi(argv[1]);
	if (size < 1) return -1;

	int size_bytes = size * size * sizeof(cl_float);
	cl_float* matrix = malloc(size_bytes);
	arc4random_buf(matrix, size_bytes);

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
					   size_bytes, matrix, NULL);
	rows_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE,
								 sizeof(cl_int) * size, 0, NULL);
	index_buffer =
		clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
					   sizeof(cl_int), &zero, NULL);
	res_buffer =
		clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float), 0, NULL);
	factor_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE,
								   sizeof(cl_float) * size, 0, NULL);

	factor_kernel = clCreateKernel(program, "factor", NULL);
	clSetKernelArg(factor_kernel, 0, sizeof(array_buffer), &array_buffer);
	clSetKernelArg(factor_kernel, 1, sizeof(rows_buffer), &rows_buffer);
	clSetKernelArg(factor_kernel, 2, sizeof(index_buffer), &index_buffer);
	clSetKernelArg(factor_kernel, 3, sizeof(factor_buffer), &factor_buffer);
	clSetKernelArg(factor_kernel, 4, sizeof(cl_int), &size);
	pass_kernel = clCreateKernel(program, "pass", NULL);
	clSetKernelArg(pass_kernel, 0, sizeof(array_buffer), &array_buffer);
	clSetKernelArg(pass_kernel, 1, sizeof(rows_buffer), &rows_buffer);
	clSetKernelArg(pass_kernel, 2, sizeof(index_buffer), &index_buffer);
	clSetKernelArg(pass_kernel, 3, sizeof(factor_buffer), &factor_buffer);
	clSetKernelArg(pass_kernel, 4, sizeof(cl_int), &size);
	swap_kernel = clCreateKernel(program, "swap", NULL);
	clSetKernelArg(swap_kernel, 0, sizeof(array_buffer), &array_buffer);
	clSetKernelArg(swap_kernel, 1, sizeof(rows_buffer), &rows_buffer);
	clSetKernelArg(swap_kernel, 2, sizeof(index_buffer), &index_buffer);
	clSetKernelArg(swap_kernel, 3, sizeof(cl_int), &size);
	sum_kernel = clCreateKernel(program, "sum", NULL);
	clSetKernelArg(sum_kernel, 0, sizeof(array_buffer), &array_buffer);
	clSetKernelArg(sum_kernel, 1, sizeof(rows_buffer), &rows_buffer);
	clSetKernelArg(sum_kernel, 2, sizeof(res_buffer), &res_buffer);
	clSetKernelArg(sum_kernel, 3, sizeof(cl_int), &size);
	rows_kernel = clCreateKernel(program, "fill_rows", 0);
	clSetKernelArg(rows_kernel, 0, sizeof(rows_buffer), &rows_buffer);

	clEnqueueNDRangeKernel(queue, rows_kernel, 1, NULL, &size, NULL, 0, NULL,
						   0);
	clFinish(queue);

	double determ = calculate_determinant(matrix, size);

	printf("Result: %f\n", time, determ);
}
