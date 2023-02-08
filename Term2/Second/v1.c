#include <stdio.h>

int sgn(float x) {
    asm(
        "shl eax,1;"
        "jz sgn_end;"
        "sbb eax,eax;"
        "or al,1;"

        "sgn_end:");
}

void main() {
    float a;
    scanf("%f", &a);
    printf("%d\n", sgn(a));
}