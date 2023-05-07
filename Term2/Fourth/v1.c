#include <stdio.h>
#include <string.h>

int CountChar(char *s, char c, long n) {
	asm("mov ecx,edx; \n"  // Количество элементов в ecx
		"xor edx,edx; \n"  // Обнуление edx
		"cc_loop:; \n"
		"inc edx; \n"  // +1
		"repne scasb; \n"  // Повторяем пока eax и байт по адресу из edi не
						   // равны
		// При проверке увеличивает edi и уменьшает ecx
		"jz cc_loop; \n"  // Если сравниваемые элементы равны
		"lea eax,[edx-1]; \n"  // edx -1
	);
}

void readLine(char *str) {
	char c;
	while ((c = getchar()) != '\n') {
		*str = c;
		str++;
	}
	*str = 0;
}

void main() {
	char str[128], c;
	readLine(str);
	scanf("%c", &c);
	int count = CountChar(str, c, strlen(str));
	printf("%d", count);
}