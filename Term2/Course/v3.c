#include <stdio.h>

long FindMax(long*dest, long count, long*index){
    // rax - array item
    // rsi - array ptr
    // rcx - counter
    // rbx - inverse result
    // rdx - max
    // r8 - array length
    // rdi - index ptr
    asm(
        "mov r8, rsi;\n"
        "mov rcx, rsi;\n"
        "mov rsi, rdi;\n"
        "mov rdi, rdx;\n"

        "xor rdx, rdx;\n"
        //"dec rdx;\n"
        "inc rdx;\n"
        "shl rdx, 63;\n"

        "mov rbx, rcx;\n"
        "inc rbx;\n"

        "cmp rcx, 0;\n"
        "jbe return;\n"

        "loop:"
        "lodsq;\n"
        "cmp rdx, rax;\n"
        "jge do_loop;\n"
        "mov rdx, rax;\n"
        "mov rbx, rcx;\n"
        "do_loop:"
        "loop loop;\n"

        "return:"
        "mov rax, rdx;\n"

        "cmp rdi, 0;\n"
        "jz exit;\n"
        "sub r8, rbx;\n"
        "mov [rdi], r8;\n"
        "exit:"
    );
}

void main(){
    long count;
    scanf("%ld", &count);
    long array[count], index;
    for (int i=0; i<count; i++){
        scanf("%ld", array+i);
    }
    
    printf("Zero length\n");
    long max=FindMax(array, 0, &index);
    printf("Max: %ld, index: %ld\n", max, index);

    printf("Zero index pointer\n");
    max=FindMax(array, count, 0);
    printf("Max: %ld, index: %ld\n", max, index);

    long testArray[]={1,-5,36,-2457,24};

    printf("One max\n");
    max=FindMax(testArray, 5, &index);
    printf("Max: %ld, index: %ld\n", max, index);

    testArray[4]=36;

    printf("Two max\n");
    max=FindMax(testArray, 5,&index);
    printf("Max: %ld, index: %ld\n", max, index);
}