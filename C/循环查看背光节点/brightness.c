#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>  
#include <unistd.h>  
#include <sys/time.h>  
#include<unistd.h>

typedef signed      int    si;  
typedef signed      char   s8;  
typedef unsigned    char   u8;  
typedef unsigned    int    ui;  
typedef unsigned long int  uli;  
typedef unsigned long long ulli;


#define FAILED_OPEN_FILE	-1
#define NOT_ROOT			-2
#define ERROR				-22
#define INVALID_ARGV		-3
#define DEFAULT_COUNT		0
#define FILE_PATH			"/sys/class/backlight/sprd_backlight/brightness"

static si get_argv(char *time_char)  
{  
    char *ch = time_char;  
    si ret = 0;  
      
    if( *ch == '0'){  
        fprintf(stderr, "interval should not be zero\n");  
        return INVALID_ARGV;  
    }  
  
    for(ch=time_char; *ch; ch++){  
        if( *ch == '-' || !(*ch >= '0' || *ch <= '9')){  
            fprintf(stderr, "please input the correct argv time(no negative)\n");  
            return INVALID_ARGV;  
        }  
        ret = ret*10+(*ch-'0');  
    }  
    return ret;  
}  

static si check_argv(char *tag, si *count, char *value)
{
	si argv_count = *count;
	//printf("%s\n",tag);
	//printf("%d\n",strcmp(tag,"-i"));
	if(strcmp(tag,"-n")==0){
		argv_count = get_argv(value); 
		if(argv_count<0)
			return INVALID_ARGV;    			 
	}else{
		fprintf(stderr, "you must input correct argvs\n");   
		return INVALID_ARGV; 
	}
	if(argv_count==0)
		argv_count = DEFAULT_COUNT;
	*count = argv_count;
	return 0;
}

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

int main(int argc, char *argv[])
{
	int value = -1;
	int count = 0;
	if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        return INVALID_ARGV;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
        return INVALID_ARGV;  
	} 
	// input argc 1  
    if(argc==1){  
        count = DEFAULT_COUNT;  		
    // input argc 3  or more
    }else if(argc==3){
		if(check_argv(argv[1], &count, argv[2])<0)
			return INVALID_ARGV;
    }
	
	// 只有userdebug版本才有权限
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	if(count==0){
		
	}
	while(1){
		value = get_brightness();
		if(value<0)
			return ERROR;
		printf("brightness: %d\n", value);
		if(count==1){
			break;
		}
		sleep (1);
	}
	return 0;
}
