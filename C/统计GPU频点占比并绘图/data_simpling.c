#include<string.h>
#include<stdio.h>
#include<error.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>  


#define ERROR	    			-22
#define BUFFER_SIZE				256
#define MAX_AVALLABLE_FREQ		20
#define DEFAULT_INTERVAL		100
#define DEFAULT_TIME			60

#define FILE_PATH	"/sdcard/gpu_path.txt"
#define FILE_DIR	"/sdcard/gpu_proportion"

static int get_argv(char *time_char)  
{  
    char *ch = time_char;  
    int ret = 0;  
      
    if( *ch == '0'){  
        fprintf(stderr, "interval should not be zero\n");  
        return ERROR;  
    }  
  
    for(ch=time_char; *ch; ch++){  
        if( *ch == '-' || !(*ch >= '0' || *ch <= '9')){  
            fprintf(stderr, "please input the correct argv time(no negative)\n");  
            return ERROR;  
        }  
        ret = ret*10+(*ch-'0');  
    }  
    return ret;  
}  

static int get_file_path(char *cur_path, char *avalible_freq)
{
	FILE *fp  = NULL;
	fp = fopen(FILE_PATH, "r");
	if(fp==NULL){
		fprintf(stderr, "failed open file %s\n", FILE_PATH);
		return ERROR;
	}
	char buffer[BUFFER_SIZE];
	int i = 0;
	while(fgets(buffer, BUFFER_SIZE, fp)){
		if(i==0){
			strncpy(cur_path,buffer,strlen(buffer)-2);
			//printf("%s\t",cur_path);
		}
		if(i==1){
			strncpy(avalible_freq,buffer,strlen(buffer));
			//printf("%s",avalible_freq);
		}
		i++;
	}
	fclose(fp);
	return 0;
}


static int EmptyDir(char *destDir)  
{  
  
    DIR *dp;  
    struct dirent *entry;  
    struct stat statbuf;  
    if ((dp = opendir(destDir)) == NULL){  
        fprintf(stderr, "cannot open directory: %s\n", destDir);  
        return -1;  
    }  
    chdir (destDir);  
    while ((entry = readdir(dp)) != NULL){  
        lstat(entry->d_name, &statbuf);  
        if (S_ISREG(statbuf.st_mode))  
        {  
            remove(entry->d_name);  
        }  
    }  
    return 0;  
}  


static void get_time(char *result_file)
{
	if(!access(FILE_DIR,F_OK)){
		EmptyDir(FILE_DIR);
		rmdir(FILE_DIR);
	}
	mkdir(FILE_DIR,S_IRWXG);
	time_t t;
    struct tm * lt;
    time (&t);//获取Unix时间戳。
    lt = localtime (&t);//转为时间结构。
	sprintf(result_file, "%s/%d-%d-%d_%d-%d-%d%s\n", FILE_DIR, lt->tm_year+1900, lt->tm_mon, \
	lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec,".txt");
}

static int get_store_data(const char *file_path,  char *result_file)
{
	FILE    *fp = NULL;
	long int data = 0;
	fp = fopen(file_path, "r");
	char buffer[BUFFER_SIZE];
	if(fp==NULL){
		fprintf(stderr, "failed opend file:%s\n", file_path);
		return ERROR;
	}
	fgets(buffer, BUFFER_SIZE, fp);
	//printf("%s",buffer);
	fclose(fp);
	
	// store data
	FILE *fp_result = NULL;
	fp_result = fopen(result_file, "a+");
	if(fp_result==NULL){
		fprintf(stderr, "failed opend file:%s\n", result_file);
		return ERROR;
	}
	fputs(buffer, fp_result);
	fclose(fp_result);
	return 0;
}

static int check_argv(int *interval, char *tag, int *accummulate_time, char *value)
{
	int argv_interval=*interval, argv_time = *accummulate_time;
	//printf("%s\n",tag);
	//printf("%d\n",strcmp(tag,"-i"));
	if(strcmp(tag,"-i")==0){
		argv_interval = get_argv(value);  
		if(argv_interval<0)
			return ERROR;  
	}else if(strcmp(tag,"-t")==0){
		argv_time = get_argv(value); 
		if(argv_time<0)
			return ERROR;    			 
	}else{
		fprintf(stderr, "you must input correct argvs\n");   
		return ERROR; 
	}
    if(argv_interval==0)
		argv_interval  = DEFAULT_INTERVAL;
	if(argv_time==0)
		argv_time = DEFAULT_TIME;
	*interval = argv_interval;
	*accummulate_time = argv_time;
	return 0;
}

static int data_simpling(int interval, long int counts)
{
	char cur_path[BUFFER_SIZE], avalible_freq[BUFFER_SIZE];
	char result[BUFFER_SIZE];
	get_time(result);
	printf("%s", result);
	get_file_path(cur_path, avalible_freq);
	get_store_data(avalible_freq, result);
	long int count = 0;
	struct timeval start, end;
	gettimeofday(&start, NULL);
	while(count<counts){
		get_store_data(cur_path, result);
		usleep(interval*1000*0.9);
		count++;
	}
	gettimeofday(&end, NULL);
	float timeuse = (1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec)/1000.0;
	printf("time_use is %.2fms\n", timeuse);
}

int main(int argc, char *argv[])
{
	int interval = DEFAULT_INTERVAL;
	int accummulate_time = DEFAULT_TIME;
	// input argc less than 1  
    if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        return ERROR;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
        return ERROR;  
	}
	
	// input argc 1  
    if(argc==1){  
        interval = DEFAULT_INTERVAL;  
        accummulate_time = DEFAULT_TIME;  		
    // input argc 3  
    }else if(argc==3){
		if(check_argv(&interval, argv[1], &accummulate_time, argv[2])<0)
			return ERROR;
    // input argc 3  or more
    }else{
		if(check_argv(&interval, argv[1], &accummulate_time, argv[2])<0)
			return ERROR;
		if(check_argv(&interval, argv[3], &accummulate_time, argv[4])<0)
			return ERROR;
	}

	long int counts = accummulate_time*1000/interval;
	printf("accummulate_time: %d\tinterval: %d\tcounts: %d\n",accummulate_time,interval, counts);
	data_simpling(interval, counts);
	return 0;
}
