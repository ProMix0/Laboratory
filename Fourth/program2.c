#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Wrong arguments count\n");
        return 1;
    }

    char *fileName = argv[1];
    int replacesCount = atoi(argv[2]);

    FILE *inputFile = fopen(fileName, "rt");
    if (!inputFile) {
        printf("Fail to open file\n");
        return 1;
    }

    char *destinationFileName = malloc(strlen(fileName) + 4);
    strcpy(destinationFileName, fileName);
    FILE *outputFile = fopen(strcat(destinationFileName, ".out"), "w+");
    if (!outputFile) {
        printf("Fail to write to %s\n", destinationFileName);
        return 1;
    }

    int c;
    while ((c = fgetc(inputFile)) != EOF) {
        if (replacesCount > 0 && c >= '0' && c <= '9') {
            fputc(' ', outputFile);
            replacesCount--;
        } else {
            fputc(c, outputFile);
        }
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
