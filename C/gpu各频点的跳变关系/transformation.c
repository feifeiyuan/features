#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>  
#include <unistd.h>  
#include <sys/time.h>  
#include <unistd.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <dirent.h>

#include "transformation.h"

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


static si check_argv(char *tag, si *accummulate_time, char *value)
{
	si argv_time = *accummulate_time;
	if(strcmp(tag,"-t")==0){
		argv_time = get_argv(value); 
		if(argv_time<0)
			return INVALID_ARGV;    			 
	}else{
		fprintf(stderr, "you must input correct argvs\n");   
		fprintf(stderr, "expl: ./data/local/tmp/transformation -t 60(s)\n");  
		return INVALID_ARGV; 
	}
	if(argv_time==0)
		argv_time = DEFAULT_TIME;
	*accummulate_time = argv_time;
	return 0;
}

static void print_debug(u8 size, struct transformation *trans_union)
{	
	u8 i = 0, j = 0;
	for(i=0;i<size;i++){
		if(i==0){
			printf("\t");
		}
		printf("%lld\t", trans_union[i].current/1000000);
	}
	for(i=0;i<size;i++){
		printf("\n%lld\t", trans_union[i*size].previous/1000000);
		for(j=0; j<size; j++){
			printf("%lld\t", trans_union[j+i*size].count);
		}
	}
	printf("\n\n");
}

static si get_init_transformation(u8 size, uli *gpu_available_freq, struct transformation *trans_union)
{
	u8 i = 0, j = 0;
	for(i=0; i<size; i++){
		for(j=0; j<size; j++){
			trans_union[j+i*size].previous = gpu_available_freq[i];
			trans_union[j+i*size].current = gpu_available_freq[j];
			trans_union[j+i*size].count = 0;
		}
	}
	return 0;
}

static char *get_certain_path(char *file, char *path)
{	
	static char file_path[BUFFER_SIZE];
	char *pattern = "*.gpu";
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	si ret = 0;
	strcpy(file_path, file );
	dir = opendir(file_path);
	if(dir !=NULL){
		while((entry = readdir(dir)) !=NULL){
			ret = fnmatch(pattern, entry->d_name, FNM_PATHNAME|FNM_PERIOD);
			if(ret==0){
				strcat(file_path, entry->d_name);
				strcat(file_path, "/devfreq/");
				strcat(file_path, entry->d_name);
				strcat(file_path, path);
				break;
			}else if(ret == FNM_NOMATCH){
				continue;
			}else{
				printf("error file=%s\n", entry->d_name);
			}
		}
		closedir(dir);
	}
	return file_path;
}

static ulli get_cur_freq(char *cpu_cur_path)
{
	ulli current_freq = 0;
	FILE *fp = NULL;
	fp = fopen(cpu_cur_path, "r");
	if(fp==NULL){
		fprintf(stderr, "failed opend file:%s", cpu_cur_path);  
        return FAILED_OPEN_FILE;  
	}
	fscanf(fp,"%lld", &current_freq);
	fclose(fp);
	return current_freq;
}

static si data_simpling(struct transformation *trans_union, u8 size, ulli counts, char *path)
{
	ulli current_freq = 0, times = 1, previous_freq = 0;
	char *file_path = get_certain_path(path, "/cur_freq");
	previous_freq =  get_cur_freq(file_path);
	if(previous_freq<=0){
		return ERROR;
	}
	u8 i = 0, match = 0, j=0;
	while(times<counts){
		current_freq = get_cur_freq(file_path);
		if(current_freq<=0){
			return ERROR;
		}
		 match = 0;
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				if(trans_union[j+i*size].previous==previous_freq){
					if(trans_union[j+i*size].current == current_freq){
						trans_union[j+i*size].count++;
						match = 1;
						break;
					}
				}else{
					break;
				}
			}
			if(match==1){
				break;
			}
		}
		previous_freq = current_freq;
		times++;
		usleep(20*1000);
	}
	print_debug(size, trans_union);
	return 0;
}

static char *access_dir_exist()
{
	char *file_path = NULL;
	if(!access("/sys/module/pvrsrvkm",F_OK)){
		file_path = "/sys/module/pvrsrvkm/drivers/platform:pvrsrvkm/";
	}else if(!access("/sys/module/mali",F_OK)){
		file_path = "/sys/module/mali/drivers/platform:mali/";
	}else if(!access("/sys/module/mali_kbase",F_OK)){
		file_path = "/sys/module/mali_kbase/drivers/platform:mali/";
	}else{
		printf("we do not find correct path for GPU info\n");
		return NULL;
	}
	return file_path;
}


static s8 get_init_freq(char *file, u8 *size, uli *gpu_available_freq)  
{  
	u8 i = 0;
	FILE *fp = NULL;  
	char *path = get_certain_path(file, "/available_frequencies");
    fp = fopen(path, "r");   
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:%s", file);  
        return FAILED_OPEN_FILE;  
    }  
    // even freq     
    // odd count  
    for(i=0; i<MAX_AVALLABLE_FREQ; i++){  
        if(fscanf(fp,"%ld", gpu_available_freq+i)!=1)  
            break;  
    }  
    *size = i;  
    fclose(fp); 
    return 0;  
}  

int main(int argc, char *argv[])  
{
	si accumulate_time = 0; 
	u8   gpu_size;  
    uli  gpu_available_freq[MAX_AVALLABLE_FREQ];  
	char *file_path = NULL;
	if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        fprintf(stderr, "expl: ./data/local/tmp/transformation -t 60(s)\n");  
        return INVALID_ARGV;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
		fprintf(stderr, "expl: ./data/local/tmp/transformation -t 60(s)\n");  
        return INVALID_ARGV;  
	} 
	
	// input argc 1  
    if(argc==1){  
		accumulate_time = DEFAULT_TIME;  				
    // input argc 3  or more 
    }else if(argc==3){
		if(check_argv(argv[1], &accumulate_time, argv[2])<0)
			return INVALID_ARGV;
    }
	printf("accumulate_time is %d\n", accumulate_time);
	ulli counts = accumulate_time*1000/20;
	printf("counts is %lld\n", counts);
	file_path = access_dir_exist();
	if(file_path==NULL){
		return ERROR;
	}
	
	if(get_init_freq(file_path, &gpu_size, gpu_available_freq)<0)
		return ERROR;
	struct transformation trans_union[gpu_size*gpu_size];
	get_init_transformation(gpu_size, gpu_available_freq, trans_union);
	if(data_simpling(trans_union, gpu_size, counts, file_path)<0){
		return ERROR;
	}
}