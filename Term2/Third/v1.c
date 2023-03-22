#include <stdio.h>

unsigned long f(unsigned char n)
{
asm(
"mov ecx,eax; \n"
"f_loop:; \n"
"dec ecx; \n"
"jle f_end; \n"
"mul ecx; \n"
"jnc f_loop; \n"
"mov eax,0; \n"
"f_end:; \n"
);
}

void main(){
    unsigned char c;
    scanf("%d", &c);
    unsigned long fact = f(c);
    printf("%lu", fact);
}