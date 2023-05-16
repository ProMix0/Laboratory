#include <stdio.h>

int DotProduct(short a[], short b[], int n) {
	asm("mov rcx,rdx\n"

		"test rcx,rcx\n"
		"jz return_zero\n"

		"xor rbx,rbx\n"

		"loop:\n"
		"xor rdx,rdx\n"
		"xor rax,rax\n"
		"mov dx,[rsi]\n"
		"add rsi,2\n"
		"mov ax,[rdi]\n"
		"add rdi,2\n"
		"imul ax,dx\n"
		"jc return_error\n"
		"cwde\n"
		"add ebx,eax\n"
		"jo return_error\n"
		"loop loop\n"
		"mov eax,ebx\n"
		"jmp return\n"

		"return_zero:"
		"xor eax,eax\n"
		"jmp return\n"

		"return_error:"
		"xor eax,eax\n"
		"inc eax\n"
		"shl eax, 31\n"

		"return:");
}

void test(short* a, short* b, int n) {
	for (int i = 0; i < n; i++) printf("%d ", a[i]);
	printf("\n");
	for (int i = 0; i < n; i++) printf("%d ", b[i]);
	printf("\n%d\n\n", DotProduct(a, b, n));
}

void main() {
	short a1[] = {1, 2, 3, 4, 5, 6}, b1[] = {1, 2, 3, 4, 5, 6};
	test(a1, b1, 6);

	short a2[] = {1, 2, 3, 4, 5, 6, 10}, b2[] = {1, 2, 3, 4, 5, 6, -10};
	test(a2, b2, 7);

	test(a1, b2, 0);

	short a3[] = {256}, b3[] = {256};
	test(a3, b3, 1);
}
