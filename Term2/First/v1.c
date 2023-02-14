#include <stdio.h>

int sgn(float x) { // определение функции
    asm(
        "shl eax,1;" // двигаемся влево на 1 бит
        "jz sgn_end;" // если рез. последней операции равен 0, то переходим к метке "sgn_end"
        "sbb eax,eax;" // вычитаем число из самого себя, затем из результата вычитаем CARRY FLAG
        "or al,1;" // побитовое ИЛИ

        "sgn_end:"); // МЕТКА завершения программы 
}

void main() {
    float a;
    scanf("%f", &a);
    printf("%d\n", sgn(a));
}