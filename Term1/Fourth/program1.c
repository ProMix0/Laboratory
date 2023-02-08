#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Wrong arguments count\n");
        return 1;
    }

    char *fileName = argv[1];
    char *startWord = argv[2];
    int wordLength = strlen(startWord);

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

    char *line;
    size_t length;
    while (getline(&line, &length, inputFile) != -1) {
        char startWith = length >= wordLength;
        int index = 0;
        while (startWith) {
            if (startWord[index] != line[index]) {
                startWith = 0;
                break;
            }
            index++;
            if (startWord[index] == 0)
                break;
        }

        if (!startWith)
            fprintf(outputFile, "%s", line);
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}