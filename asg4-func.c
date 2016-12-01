#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>


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


int getIndexByRowColumn(int row, int column, int dimension) {
    if (row < 1) row = 1;
    if (row > dimension) row = dimension;
    if (column < 1) column = 1;
    if (column > dimension)column = dimension;
    row -= 1;
    column -= 1;
    return (row * dimension + column);
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
            array = realloc(array, size * sizeof(int));
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
    FILE *file = fopen(path, "rb");
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
        return 0;
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
    printf(ADD_SUC);
    writeMeta(path, versionCount, matrixElementCount);

    return 0;
}


int retrieve(char *path, int version) {
    readMeta(path, &versionCount, &matrixElementCount);
    //printf("version:%d, versionCount:%d\n", version, versionCount);
    if (version > versionCount || !fileExist(path)) {

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
//        j == 0 ? fprintf(output, "%d", matrix1[j]) : fprintf(output, " %d", matrix1[j]);
        fprintf(output, "%d ", matrix1[j]);
    }
    fflush(output);
    fclose(output);
    writeMeta(path, versionCount, matrixElementCount);
    printf(RETRIEVE_SUC);
    return 0;
}


int diff(char *path, int version1, int version2, int row, int column) {
    readMeta(path, &versionCount, &matrixElementCount);

    int dimension = (int) (sqrt(matrixElementCount) + 1e-9);

    if (version1 > versionCount || version2 > versionCount || !fileExist(path)) {
        printf(DIFFERROR);
        return -1;
    }
    if (row > dimension || column > dimension || row < 1 || column < 1) {
        printf(DIFFERROR);
        return -1;
    }

    int index = getIndexByRowColumn(row, column, dimension);
    FILE *baseFile = fopen(getVFSName(path, 1), "rb");
    setbuf(baseFile, readBuffer);
    int eb, e1 = -1, e2 = -1;
    int d1s, d2s;
    fseek(baseFile, index * sizeof(int), SEEK_SET);
    fread(&eb, sizeof(int), 1, baseFile);
    fclose(baseFile);
    //baseMatrix = readBinary(getVFSName(path,1), &diffSize);
    if (version1 != 1 && version2 != 1) {

        diff1 = readBinary(getVFSName(path, version1), &d1s);
        int i;
        for (i = 0; i < d1s; i += 2) {
            if (diff1[i] == index) {
                e1 = diff1[i + 1];
            }
        }
        if (e1 == -1) e1 = eb;
        diff2 = readBinary(getVFSName(path, version2), &d2s);
        for (i = 0; i < d2s; i += 2) {
            if (diff2[i] == index) {
                e2 = diff2[i + 1];
            }
        }
        if (e2 == -1) e2 = eb;
        if (e1 == e2) {
            printf("0\n");
        } else {
            printf("1\n");
        }

    } else if (version1 == 1 && version2 == 1) {
        printf("0\n");
    } else if (version1 == 1) {
        e1 = eb;
        diff2 = readBinary(getVFSName(path, version2), &d2s);
        int i;
        for (i = 0; i < d2s; i += 2) {
            if (diff2[i] == index) {
                e2 = diff2[i + 1];
            }
        }
        if (e2 == -1) e2 = eb;
        if (e1 == e2) {
            printf("0\n");
        } else {
            printf("1\n");
        }

    } else if (version2 == 1) {
        e2 = eb;
        diff1 = readBinary(getVFSName(path, version1), &d1s);
        int i;
        for (i = 0; i < d1s; i += 2) {
            if (diff1[i] == index) {
                e1 = diff1[i + 1];
            }
        }
        if (e1 == -1) e1 = eb;
        if (e1 == e2) {
            printf("0\n");
        } else {
            printf("1\n");
        }
    }
    // writeMeta(path,versionCount, matrixElementCount);

    return 0;
}


int calculate(char *path, int version1, int version2, char *par, int row, int column) {
    readMeta(path, &versionCount, &matrixElementCount);
    int dimension = (int) (sqrt(matrixElementCount) + 1e-9);
    if (version1 > versionCount || version2 > versionCount || !fileExist(path)) {
        printf(CALERROR);
        return -1;
    }
    if (row > dimension || column > dimension || row < 1 || column < 1) {
        printf(CALERROR);
        return -1;
    }
    if (version1 == version2) {
        int sumDiff = 0;
        printf("%d\n", sumDiff);
        return 0;
    }

    if (strcmp(par, "-r") == 0) {
        int rowStart = getIndexByRowColumn(row, 1, dimension);
        int rowEnd = getIndexByRowColumn(row, dimension, dimension);
        int *baseRowElement = malloc(sizeof(int) * dimension + 10);
        int *rowElement1 = malloc(sizeof(int) * dimension + 10);
        int *rowElement2 = malloc(sizeof(int) * dimension + 10);
        FILE *baseFile = fopen(getVFSName(path, 1), "rb");
        setbuf(baseFile, readBuffer);
        int d1s, d2s;
        fseek(baseFile, rowStart * sizeof(int), SEEK_SET);
        fread((void *) baseRowElement, dimension * sizeof(int), 1, baseFile);
        memcpy(rowElement1, baseRowElement, sizeof(int) * dimension);
        memcpy(rowElement2, baseRowElement, sizeof(int) * dimension);
        fclose(baseFile);
        int i;
        if (version1 != 1) {
            diff1 = readBinary(getVFSName(path, version1), &d1s);
            for (i = 0; i < d1s; i += 2) {
                if (diff1[i] >= rowStart && diff1[i] <= rowEnd) {
                    rowElement1[diff1[i] - rowStart] = diff1[i + 1];
                }
            }
        }
        if (version2 != 1) {
            diff2 = readBinary(getVFSName(path, version2), &d2s);
            for (i = 0; i < d2s; i += 2) {
                if (diff2[i] >= rowStart && diff2[i] <= rowEnd) {
                    rowElement2[diff2[i] - rowStart] = diff2[i + 1];
                }
            }
        }
        int sumDiff = 0;
        for (i = 0; i < dimension; ++i) {
            sumDiff += rowElement1[i] - rowElement2[i];
        }
        printf("%d\n", sumDiff);

    } else if (strcmp(par, "-c") == 0) {
        int columnStart = getIndexByRowColumn(1, column, dimension);
        int columnEnd = getIndexByRowColumn(dimension, column, dimension);
        int *baseColumnElement = malloc(sizeof(int) * dimension + 10);
        int *columnElement1 = malloc(sizeof(int) * dimension + 10);
        int *columnElement2 = malloc(sizeof(int) * dimension + 10);
        FILE *baseFile = fopen(getVFSName(path, 1), "rb");
        setbuf(baseFile, readBuffer);
        int d1s, d2s;
        fseek(baseFile, columnStart * sizeof(int), SEEK_SET);
        int i;
        for (i = 0; i < dimension; i += 1) {
            fread((void *) &baseColumnElement[i], sizeof(int), 1, baseFile);
            fseek(baseFile, (dimension - 1) * sizeof(int), SEEK_CUR);
        }
        memcpy(columnElement1, baseColumnElement, sizeof(int) * dimension);
        memcpy(columnElement2, baseColumnElement, sizeof(int) * dimension);
        fclose(baseFile);
        if (version1 != 1) {
            diff1 = readBinary(getVFSName(path, version1), &d1s);
            for (i = 0; i < d1s; i += 2) {
                if (diff1[i] % dimension == columnStart) {
                    columnElement1[diff1[i] / dimension] = diff1[i + 1];
                }
            }
        }
        if (version2 != 1) {
            diff2 = readBinary(getVFSName(path, version2), &d2s);
            for (i = 0; i < d2s; i += 2) {
                if (diff2[i] % dimension == columnStart) {
                    columnElement2[diff2[i] / dimension] = diff2[i + 1];
                }
            }
        }
        int sumDiff = 0;
        for (i = 0; i < dimension; ++i) {
            sumDiff += columnElement1[i] - columnElement2[i];
        }
        printf("%d\n", sumDiff);
    } else if (strcmp(par, "-a") == 0) {
        // int index = getIndexByRowColumn(row, column, dimension);
        int sumDiff = 0;
        int d1s, d2s;
        FILE *baseFile = fopen(getVFSName(path, 1), "rb");
        setbuf(baseFile, readBuffer);
        if (version1 != 1) {
            diff1 = readBinary(getVFSName(path, version1), &d1s);
        }
        if (version2 != 1) {
            diff2 = readBinary(getVFSName(path, version2), &d2s);
        }
        int j = 0;
        for (j = -1; j < 2; ++j) {
            if (row + j < 0) continue;
            if (row + j > dimension) continue;

            int rowStart = getIndexByRowColumn(row + j, column - 1, dimension);
            int rowEnd = getIndexByRowColumn(row + j, column + 1, dimension);
            int *baseRowElement = malloc(sizeof(int) * 3);
            int *rowElement1 = malloc(sizeof(int) * 3);
            int *rowElement2 = malloc(sizeof(int) * 3);
            fseek(baseFile, rowStart * sizeof(int), SEEK_SET);
            fread((void *) baseRowElement, (rowEnd - rowStart + 1) * sizeof(int), 1, baseFile);
            memcpy(rowElement1, baseRowElement, sizeof(int) * 3);
            memcpy(rowElement2, baseRowElement, sizeof(int) * 3);
            fclose(baseFile);

            int i;
            if (version1 != 1) {
                for (i = 0; i < d1s; i += 2) {
                    if (diff1[i] >= rowStart && diff1[i] <= rowEnd) {
                        rowElement1[diff1[i] - rowStart] = diff1[i + 1];
                    }
                }
            }

            if (version2 != 1) {
                for (i = 0; i < d2s; i += 2) {
                    if (diff2[i] >= rowStart && diff2[i] <= rowEnd) {
                        rowElement2[diff2[i] - rowStart] = diff2[i + 1];
                    }
                }
            }

            for (i = 0; i < (rowEnd - rowStart + 1); ++i) {
                sumDiff += rowElement1[i] - rowElement2[i];
            }
        }
        printf("%d\n", sumDiff);
    } else {
        printf(CALERROR);
        return -1;
    }


    //baseMatrix = readBinary(getVFSName(path,1), &diffSize);

    //writeMeta(versionCount, matrixElementCount);
    return 0;
}

