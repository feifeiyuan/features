#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>  
#include <unistd.h>  
#include <sys/time.h>  
#include<unistd.h>

#define FAILED_OPEN_FILE	-1
#define NOT_ROOT			-2
#define ERROR				-22
#define FILE_PATH			"/sys/class/backlight/sprd_backlight/brightness"

static int get_brightness()
{
	int value = 0;
	FILE *fp = NULL;
	fp = fopen(FILE_PATH, "r");
	if(fp == NULL){
		fprintf(stderr, "Failed open file %s\n", FILE_PATH);
		return FAILED_OPEN_FILE;
	}
	fscanf(fp, "%d", &value);
	if(value<0||value>255){
		fprintf(stderr, "The value should x>=0&&x<=255\n");
		return ERROR;
	}
	fclose(fp);
	return value;
}

int main()
{
	int value = -1;
	// 只有userdebug版本才有权限
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	while(1){
		value = get_brightness();
		if(value<0)
			return ERROR;
		printf("brightness: %d\n", value);
		sleep (1);
	}
	return 0;
}
