#include <stdio.h>

void Swap(void* mem1, void* mem2, int len) {
	asm("mov rcx,rdx\n"
		"shr rcx,3\n"
		"and rdx,7\n"

		"test rcx,rcx\n"
		"jz skip_8\n"

		"loop_8:"
		"mov rax,[rsi]\n"
		"mov rbx,[rdi]\n"

		"mov [rdi],rax\n"
		"mov [rsi],rbx\n"

		"add rsi,8\n"
		"add rdi,8\n"
		"loop loop_8\n"

		"skip_8:"
		"mov rcx,rdx\n"
		"test rcx,rcx\n"
		"jz return\n"

		"loop_1:"
		"mov al,[rsi]\n"
		"mov bl,[rdi]\n"

		"mov [rdi],al\n"
		"mov [rsi],bl\n"

		"inc rsi\n"
		"inc rdi\n"
		"loop loop_1\n"
		"return:");
}

void main() {
	float a[] = {-1, 2.4, 3.0}, b[] = {4.25, -5.15, 6.666666};
	for (int i = 0; i < 3; i++) printf("%f ", a[i]);
	printf("\n");
	for (int i = 0; i < 3; i++) printf("%f ", b[i]);
	printf("\nSwap\n");
	Swap(a, b, 3 * sizeof(float));
	for (int i = 0; i < 3; i++) printf("%f ", a[i]);
	printf("\n");
	for (int i = 0; i < 3; i++) printf("%f ", b[i]);
	printf("\n\n\n");

	float c[] = {-1, 2.4, 3.0, 15.345, 09.90},
		  d[] = {123.456, 654.321, 4.25, -5.15, 6.666666};
	for (int i = 0; i < 3; i++) printf("%f ", c[i]);
	printf("\n");
	for (int i = 0; i < 3; i++) printf("%f ", d[i]);
	printf("\nSwap\n");
	Swap(c, d, 5 * sizeof(float));
	for (int i = 0; i < 3; i++) printf("%f ", c[i]);
	printf("\n");
	for (int i = 0; i < 3; i++) printf("%f ", d[i]);
	printf("\n\n\n");

	char e[] = "hello", f[] = "world";
	printf("%s\n%s\nSwap\n", e, f);
	Swap(e, f, 5);
	printf("%s\n%s\n\n\n", e, f);

	char g[] = "goodbye", h[] = "my hell";
	printf("%s\n%s\nSwap\n", g, h);
	Swap(g, h, 7);
	printf("%s\n%s\n\n\n", g, h);
}