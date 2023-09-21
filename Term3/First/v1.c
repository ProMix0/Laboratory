#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Wrong arguments count\n");
        return -1;
    }

    char *fileName = argv[1];
    char toDelete = *argv[2];

    int inputFile = open(fileName, O_RDONLY);
    if (inputFile < 0) {
        printf("Fail to open file\n");
        return -1;
    }

    char *destinationFileName = malloc(strlen(fileName) + 4);
    strcpy(destinationFileName, fileName);
    int outputFile = creat(strcat(destinationFileName, ".out"), O_WRONLY);
    if (outputFile < 0) {
        printf("Fail to write to %s\n", destinationFileName);
        return -1;
    }

    int replace_count = 0, deletions = 0;
    int c;
    while (read(inputFile, &c, 1)) {
        if (c != toDelete) {
            write(outputFile, &c, 1);
        } else {
            deletions++;
        }
    }

    close(inputFile);
    close(outputFile);

    return deletions;
}