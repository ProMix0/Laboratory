#include <stdio.h>

void abs_arr(unsigned long d[], long s[], int count){
asm(
    "mov ecx, edx;\n"
    "jecxz abs_exit;\n"
    "abs_loop:\n"
    "lodsq;\n"
    "cdq;\n"
    "xor eax, edx;\n"
    "sub eax, edx;\n"
    "stosq;\n"
    "loop abs_loop;\n"
    "abs_exit:\n"
);
}

void main(){
    long from[16]={
        1,-2,3,-4,5,-6,7,-8,9,-10,11,-12,13,-14,15,-16
    };
    unsigned long to[16];
    abs_arr(to, from, 16);
    for(int i=0;i<16;i++){
        printf("%ld\t%lu\n",from[i],to[i]);
    }
}