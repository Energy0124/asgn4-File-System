#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>


#define ADDERROR  "Add error!\n"
#define RETRIEVEERROR  "Retrieve error!\n"
#define DIFFERROR  "Diff error!\n"
#define CALERROR  "Calculate error!\n"
#define ADD_SUC "Add success!\n"
#define RETRIEVE_SUC "Retrieve success!\n"
#define VFS_FOLDER "./.vfsdata/"
#define INIT_MALLOC_SIZE 12000
#define MAX_FILENAME_SIZE 256
#define META_SUBFIX "_meta"
int versionCount = 0; //number of version
char readBuffer[BUFSIZ + 1];
char readBuffer2[BUFSIZ + 1];
char writeBuffer[BUFSIZ + 1];
char writeBuffer2[BUFSIZ + 1];
int *baseMatrix = NULL;
int *matrix1 = NULL;
int *matrix2 = NULL;
int *diff1 = NULL;
int *diff2 = NULL;
int matrixElementCount = 0;


void readMeta(char *path, int *versionCount, int *matrixSize);


void writeMeta(char *path, int count, int size);

int *readBinary(char *path, int *size);

//void writeTextFile(char *path, int *matrix);

char *getVFSName(char *path, int version);

char *getVFSMetaName(char *path);

int getIndexByRowColumn(int row, int column, int dimension);

char *basename(char *path) {
    char *base = strrchr(path, '/');
    return base ? base + 1 : path;
}

// Returns floor of square root of x
int floorSqrt(int x) {
    // Base cases
    if (x == 0 || x == 1)
        return x;

    // Do Binary Search for floor(sqrt(x))
    int start = 1, end = x, ans = -1;
    while (start <= end) {
        int mid = (start + end) / 2;

        // If x is a perfect square
        if (mid * mid == x)
            return mid;

        // Since we need floor, we update answer when mid*mid is
        // smaller than x, and move closer to sqrt(x)
        if (mid * mid < x) {
            start = mid + 1;
            ans = mid;
        } else // If mid*mid is greater than x
            end = mid - 1;
    }
    return ans;
}

int fileExist(char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

void readMeta(char *path, int *version, int *size) {
    FILE *metaFile = NULL;
    char *vfsname = getVFSMetaName(path);
    metaFile = fopen(vfsname, "r");
    if (metaFile == NULL) {
        *version = 0;
        *size = 0;
        return;
    }
    fscanf(metaFile, "%d %d", version, size);
    fclose(metaFile);
}

void writeMeta(char *path, int count, int size) {
    FILE *metaFile = NULL;
    char *vfsname = getVFSMetaName(path);
    metaFile = fopen(vfsname, "w");
    /*if (metaFile == NULL) {
        printf(ADDERROR);
        return;
    }*/
    fprintf(metaFile, "%d %d", count, size);
    fflush(metaFile);
    fclose(metaFile);
}

int *readText(char *filename, int *number) {
    FILE *file = fopen(filename, "r");
    setbuf(file, readBuffer);
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

int *readBinary(char *path, int *size) {
    FILE *file = fopen(path, "r");
    setbuf(file, readBuffer);
    struct stat st;
    stat(path, &st);
    long fileSize = st.st_size;
    //  printf("binary file size: %ld\n", fileSize);
    int *array = (int *) malloc(fileSize + 10 * sizeof(int));
    fread(array, (size_t) fileSize, 1, file);
    *size = fileSize / sizeof(int);
    fclose(file);
    return array;
}

char *getVFSName(char *path, int version) {
    char *base = basename(path);
    char name[MAX_FILENAME_SIZE];
    stpcpy(name, base);
    char *vfsname;
    char vstr[100];
    sprintf(vstr, "_%d", version);
    vfsname = malloc(strlen(VFS_FOLDER) + strlen(name) + strlen(vstr) + 10);
    vfsname[0] = '\0';   // ensures the memory is an empty string
    strcat(vfsname, VFS_FOLDER);
    strcat(vfsname, name);
    strcat(vfsname, vstr);

    return vfsname;
}

char *getVFSMetaName(char *path) {
    char *base = basename(path);
    char name[MAX_FILENAME_SIZE];
    stpcpy(name, base);
    char *vfsname;

    vfsname = malloc(strlen(VFS_FOLDER) + strlen(name) + strlen(META_SUBFIX) + 10);
    vfsname[0] = '\0';   // ensures the memory is an empty string
    strcat(vfsname, VFS_FOLDER);
    strcat(vfsname, name);
    strcat(vfsname, META_SUBFIX);
    return vfsname;
}

int add(char *path) {
    readMeta(path, &versionCount, &matrixElementCount);
    if (!fileExist(path)) {
        printf(ADDERROR);
    }


    /*   FILE *input = fopen(path, "r");
       if (input == NULL) {
           printf(ADDERROR);
           return -1;
       }
   */

    int size;

    if (versionCount == 0) { //first input
        baseMatrix = readText(path, &size);
        matrixElementCount = size;
        versionCount++;//increment version


//        char outName[MAX_FILENAME_SIZE];
//        sprintf(outName, VFS_FOLDER"%d", versionCount);
        //char * vfsname=getVFSName(path,versionCount);
        FILE *output = fopen(getVFSName(path, versionCount), "wb");
        setbuf(output, writeBuffer);
/*        if (output == NULL) {
            printf(ADDERROR);
            return -1;
        }*/
        int i;
        for (i = 0; i < size; ++i) {
            fwrite(&baseMatrix[i], sizeof baseMatrix[i], 1, output);
        }
        fflush(output);
        fclose(output);
    } else {
        int diffSize;
        baseMatrix = readBinary(getVFSName(path, 1), &diffSize);
        matrix1 = readText(path, &size);
        // size=matrixElementCount;
        versionCount++;//increment version
//        char outName[MAX_FILENAME_SIZE];
//        sprintf(outName, VFS_FOLDER"%d", versionCount);
        FILE *output = fopen(getVFSName(path, versionCount), "wb");
        setbuf(output, writeBuffer);
/*        if (output == NULL) {
            printf(ADDERROR);
            return -1;
        }*/
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
    writeMeta(path, versionCount, matrixElementCount);

    return 0;
}


int retrieve(char *path, int version) {
    readMeta(path, &versionCount, &matrixElementCount);
    //printf("version:%d, versionCount:%d\n", version, versionCount);
    if (version > versionCount) {

        printf(RETRIEVEERROR);
        return -1;
    }
    int diffSize;
    baseMatrix = readBinary(getVFSName(path, 1), &diffSize);
//    char filename[MAX_FILENAME_SIZE];
//    sprintf(filename, VFS_FOLDER"%d", version);
    if (version != 1) {
        diff1 = readBinary(getVFSName(path, version), &diffSize);
        matrix1 = baseMatrix;
        int i;
        for (i = 0; i < diffSize; i += 2) {
            matrix1[diff1[i]] = diff1[i + 1];
        }
    } else {
        matrix1 = baseMatrix;
    }
    //writeTextFile(path, matrix1);
    FILE *output = fopen(path, "w");
    setbuf(output, writeBuffer);
    if (output == NULL) {
        printf(ADDERROR);
        return -1;
    }
    int j;
    for (j = 0; j < matrixElementCount; ++j) {
        j == 0 ? fprintf(output, "%d", matrix1[j]) : fprintf(output, " %d", matrix1[j]);
    }
    fflush(output);
    fclose(output);
    writeMeta(path, versionCount, matrixElementCount);
    printf(RETRIEVE_SUC);
    return 0;
}


int diff(char *path, int version1, int version2, int row, int column) {
    readMeta(path, &versionCount, &matrixElementCount);
    if (version1 > versionCount || version2 > versionCount) {
        printf(DIFFERROR);
        return -1;
    }

    int dimension = floorSqrt(matrixElementCount);
    int index = getIndexByRowColumn(row, column, dimension);
    getIndexByRowColumn(row, column, dimension);
    //baseMatrix = readBinary(getVFSName(path,1), &diffSize);
    if (version1 != 1 && version2 != 1) {

    } else if (version1 == 1 && version2 == 1) {

    } else if (version1 == 1) {

    } else if (version2 == 1) {

    }
    // writeMeta(path,versionCount, matrixElementCount);
    return 0;
}

int getIndexByRowColumn(int row, int column, int dimension) {
    row -= 1;
    column -= 1;
    return (row * dimension + column);
}

int calculate(char *path, int version1, int version2, char *par, int row, int column) {
    readMeta(path, &versionCount, &matrixElementCount);
    int diffSize;
    //baseMatrix = readBinary(getVFSName(path,1), &diffSize);

    //writeMeta(versionCount, matrixElementCount);
    return 0;
}

