#include <stdio.h>

int sgn(double x) {	   // определение функции
	asm("shl rax,1;"   // двигаемся влево на 1 бит
		"jz sgn_end;"  // если рез. последней операции равен 0, то переходим к
					   // метке "sgn_end"
		"sbb rax,rax;"	// вычитаем число из самого себя, затем из результата
						// вычитаем CARRY FLAG
		"or al,1;"		// побитовое ИЛИ

		"sgn_end:");  // МЕТКА завершения программы
}

void main() {
	double a;
	scanf("%lf", &a);
	printf("%d\n", sgn(a));
}