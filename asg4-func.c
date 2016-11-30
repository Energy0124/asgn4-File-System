#include <stdio.h>
#include <stdlib.h>

#define ADDERROR  "Add error!"
#define RETRIEVEERROR  "Retrieve error!"
#define DIFFERROR  "Diff error!"
#define CALERROR  "Calculate error!"
#define ADD_SUC "Add success!"
#define RETRIEVE_SUC "Retrieve success!"

int versionCount=0; //number of version
char readBuffer[BUFSIZ+1];
int  add(char *path){
    FILE *input = fopen(path, "r");
	setbuf(input,readBuffer);

	return 0;
}


int retrieve(char *path, int version){
	
	return 0;
}

int diff(char *path, int version1, int version2, int row, int column ){
	
	return 0;
}

int calculate(char *path, int version1, int version2, char *par, int row ,int column){
	
	return 0;
}

