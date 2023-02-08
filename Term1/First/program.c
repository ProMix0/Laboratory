#include <stdio.h>
#include <string.h>

void readLine(char *str);

void main() {
    printf("Enter first string\n");
    char str1[128];
    readLine(str1);

    printf("Enter second string\n");
    char str2[128];
    readLine(str2);

    printf("First string length: %ld\nSecond string length: %ld\n", strlen(str1),
           strlen(str2));

    char resultstr[256];
    int index = 0;
    char ch;
    if (str1[0] != 0) {
        int index1 = 0;
        do {
            ch = str1[index1];
            if (ch != ' ')
                resultstr[index++] = ch;

            index1++;
        } while (str1[index1] != 0);
    }
    if (str2[0] != 0) {
        int index2 = 0;
        do {
            ch = str2[index2];
            if (ch != ' ')
                resultstr[index++] = ch;

            index2++;
        } while (str1[index2] != 0);
    }
    resultstr[index] = 0;

    int changesCount = 0;
    index = 0;
    while ((ch = resultstr[index]) != 0 && changesCount <= 4) {
        if (ch == resultstr[index + 1]) {
            changesCount++;
            resultstr[index] = '+';
        } else {
            changesCount++;

            switch (ch) {
                case 'a':
                    resultstr[index] = 'e';
                    break;
                case 'b':
                    resultstr[index] = 'k';
                    break;
                case 'x':
                    resultstr[index] = 'm';
                    break;
                case 'z':
                    resultstr[index] = 'p';
                    break;

                default:
                    changesCount--;
                    break;
            }
        }
        index++;
    }

    printf("Result string: %s\n", resultstr);
}

void readLine(char *str) {
    int input, index = 0;

    while ((input = getchar()) != '\n') {
        str[index++] = input;
    }

    str[index] = 0;
}
