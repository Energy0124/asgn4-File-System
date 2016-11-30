#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define ADDERROR  "Add error!\n"
#define RETRIEVEERROR  "Retrieve error!\n"
#define DIFFERROR  "Diff error!\n"
#define CALERROR  "Calculate error!\n"
#define ADD_SUC "Add success!\n"
#define RETRIEVE_SUC "Retrieve success!\n"
#define VFS_FOLDER "./.vfsdata/"
#define INIT_MALLOC_SIZE 12000
int versionCount = 0; //number of version
char readBuffer[BUFSIZ + 1];
char readBuffer2[BUFSIZ + 1];
char writeBuffer[BUFSIZ + 1];
char writeBuffer2[BUFSIZ + 1];
int *baseMatrix = NULL;
int *matrix1 = NULL;
int *matrix2 = NULL;
int matrixSize = 0;


void readMeta(int *versionCount, int *matrixSize);


void writeMeta(int count, int size);

int *readBinary(char *path, int *diffsize);

void readMeta(int *version, int *size) {
    FILE *metaFile = NULL;
    metaFile = fopen(VFS_FOLDER"meta", "r");
    if (metaFile == NULL) {
        *version = 0;
        *size = 0;
        return;
    }
    fscanf(metaFile, "%d %d", version, size);
    fclose(metaFile);
}

void writeMeta(int count, int size) {
    FILE *metaFile = NULL;
    metaFile = fopen(VFS_FOLDER"meta", "w");
    if (metaFile == NULL) {
        printf(ADDERROR);
        return;
    }
    fprintf(metaFile, "%d %d", count, size);
    fflush(metaFile);
    fclose(metaFile);
}

int *readText(char *filename, int *number) {
    FILE *file = fopen(filename, "r");
    int size = INIT_MALLOC_SIZE;
    int *array = (int *) malloc(size * sizeof(int));
    int count = 0;
    while (count < size) {
        if (count > size - 9) {
            size *= 2;
            array = realloc(array, (size_t) size);
        }
        int n;
        if (fscanf(file, "%d", &n) != 1) {
            break;
        }
        array[count] = n;
        count++;
    }
    *number = count;
    fclose(file);
    return array;
}

int *readBinary(char *path, int *diffSize) {
    FILE *file = fopen(path, "r");
    int size = INIT_MALLOC_SIZE;
    struct stat st;
    stat(path, &st);
    long fileSize = st.st_size;
    printf("binary file size: %ld\n", fileSize);

    int *array = (int *) malloc(fileSize + 10 * sizeof(int));
    // int *array = (int *) malloc(size * sizeof(int));
    fread(array, (size_t) fileSize, 1, file);
    *diffSize = fileSize / sizeof(int);
    fclose(file);
    return array;
}

int add(char *path) {
    readMeta(&versionCount, &matrixSize);
    FILE *input = fopen(path, "r");
    if (input == NULL) {
        printf(ADDERROR);
        return -1;
    }

    setbuf(input, readBuffer);
    int size;

    if (versionCount == 0) { //first input
        baseMatrix = readText(path, &size);
        matrixSize = size;
        versionCount++;//increment version
        char outName[100];
        sprintf(outName, VFS_FOLDER"%d", versionCount);
        FILE *output = fopen(outName, "wb");
        setbuf(output, writeBuffer);
        if (output == NULL) {
            printf(ADDERROR);
            return -1;
        }
        int i;
        for (i = 0; i < size; ++i) {
            fwrite(&baseMatrix[i], sizeof baseMatrix[i], 1, output);
        }
        fflush(output);
        fclose(output);
    } else {
        int diffSize;
        baseMatrix = readBinary(VFS_FOLDER"1", &diffSize);
        matrix1 = readText(path, &size);
        // size=matrixSize;
        versionCount++;//increment version
        char outName[100];
        sprintf(outName, VFS_FOLDER"%d", versionCount);
        FILE *output = fopen(outName, "wb");
        if (output == NULL) {
            printf(ADDERROR);
            return -1;
        }
        int i;
        for (i = 0; i < size; ++i) {
            if (baseMatrix[i] != matrix1[i]) {
                fwrite(&i, sizeof i, 1, output);
                fwrite(&matrix1[i], sizeof matrix1[i], 1, output);
            }
        }
        fflush(output);
        fclose(output);
    }
    writeMeta(versionCount, matrixSize);

    return 0;
}


int retrieve(char *path, int version) {

    return 0;
}

int diff(char *path, int version1, int version2, int row, int column) {

    return 0;
}

int calculate(char *path, int version1, int version2, char *par, int row, int column) {

    return 0;
}

