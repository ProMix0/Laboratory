#include <stdio.h>

unsigned long f(unsigned char n) {
	asm("mov ecx,eax; \n"  // Дублирование числа в ecx
		"f_loop:; \n"
		"dec ecx; \n"	  // -1
		"jle f_end; \n"	  // Выход из цикла если ecx <= 0
		"mul ecx; \n"	  // Умножает eax на значение ecx
		"jnc f_loop; \n"  // Повторяем цикл если факториал помещается в регистре
		"xor eax,eax; \n"  // Обнуление eax
		"f_end:; \n");
}

void main() {
	unsigned char c;
	scanf("%d", &c);
	unsigned long fact = f(c);
	printf("%lu", fact);
}