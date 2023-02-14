#include <stdio.h>

void abs_arr(long s[], unsigned long d[], int count){
asm(
    "mov ecx, DWORD PTR 20[bp];\n"
    "jecxz abs_exit;\n"
    "mov esi, QWORD PTR 16[bp];\n"
    "mov edi, QWORD PTR 8[bp];\n"
    "abs_loop:\n"
    "lodsd;\n"
    "cdq;\n"
    "xor eax, edx;\n"
    "sub eax, edx;\n"
    "stosd;\n"
    "loop abs_loop;\n"
    "abs_exit:\n"
);
}

void main(){
    long from[16]={
        1,-2,3,-4,5,-6,7,-8,9,-10,11,-12,13,-14,15,-16
    },to[16];
    abs_arr(from, to, 16);
    for(int i=0;i<16;i++){
        printf("%ld\t%ld\n",from[i],to[i]);
    }
}