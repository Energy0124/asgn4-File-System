#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

struct timeval start;
struct timeval end;
double my_time;
int  add(char *path);
int retrieve(char *path, int version);
int diff(char *path, int version1, int version2, int row, int column );
int calculate(char *path, int version1, int version2, char *par, int row, int column);


int recordTime( ){
	FILE *fp;
	fp = fopen("./testcases/time.txt", "a+");
	if(fp == NULL)
		return -1;
	fprintf(fp, "%.3f", my_time);
	fputc('\n',fp);
	fclose(fp);
	return 0;
} 

int main(int argc, char *agrv[]){
	if(strcmp(agrv[1],"add")==0){
		if(argc != 3){
			printf("Usage: add [filepath]\n");
			return 1;
		}
		gettimeofday(&start,NULL);
		add(agrv[2]);
		gettimeofday(&end,NULL);
		my_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)*0.000001;
		recordTime();
		return 0;
	}else if(strcmp(agrv[1], "retrieve")==0){
		if(argc != 4){
			printf("Usage: retrieve [filepath] [version]\n");
			return 2;
		}
		gettimeofday(&start,NULL);
		retrieve(agrv[2], atoi(agrv[3]));
		gettimeofday(&end,NULL);
		my_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)*0.000001;
		recordTime();
		return 0;
	}else if(strcmp(agrv[1], "diff")==0){
		if(argc != 7){
			printf("Usage: diff [filepath] [version1] [version2] [row] [column]\n");
			return 1;
		}
		gettimeofday(&start,NULL);
		diff(agrv[2],atoi(agrv[3]),atoi(agrv[4]),atoi(agrv[5]),atoi(agrv[6]));
		gettimeofday(&end,NULL);
		my_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)*0.000001;
		recordTime();
		return 0;	
	}else if(strcmp(agrv[1], "calculate")==0){
		if(argc != 8){
			printf("Usage: calculate [filepath] [version1] [version2] [type] [row] [column]\n");
			return 1;
		}
		gettimeofday(&start,NULL);
		calculate(agrv[2],atoi(agrv[3]),atoi(agrv[4]),agrv[5],atoi(agrv[6]),atoi(agrv[7]));
		gettimeofday(&end,NULL);
		my_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)*0.000001;
		recordTime();
		return 0;	
	}else{
		printf("Can not find command %s\n",agrv[1]);
		}
	return 0;
}
