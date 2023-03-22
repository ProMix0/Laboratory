#include <stdio.h>
#include <string.h>

int CountChar(char *s, char c, long n)
{
asm(
"mov ecx,edx; \n"
"xor edx,edx; \n"
"cc_loop:; \n"
"inc edx; \n"
"repne scasb; \n"
"jz cc_loop; \n"
"lea eax,[edx-1]; \n"
);
}

void main(){
    char str[64], c;
    scanf("%s %c", str, &c);
    int count = CountChar(str, c, strlen(str));
    printf("%d", count);
}