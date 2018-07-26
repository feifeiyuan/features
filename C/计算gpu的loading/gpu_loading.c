#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>  
#include <unistd.h>  
#include <sys/time.h>  
#include<unistd.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <dirent.h>

#include "gpu_loading.h"

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
	//printf("%s\n",tag);
	//printf("%d\n",strcmp(tag,"-i"));
	if(strcmp(tag,"-t")==0){
		argv_time = get_argv(value); 
		if(argv_time<0)
			return INVALID_ARGV;    			 
	}else{
		fprintf(stderr, "you must input correct argvs\n");   
		fprintf(stderr, "expl: ./data/local/tmp/cpu_sgm -i 100(ms) -t 60(s)\n");  
		return INVALID_ARGV; 
	}
	if(argv_time==0)
		argv_time = DEFAULT_TIME;
	*accummulate_time = argv_time;
	return 0;
}

static s8 get_init_freq_count(char *file, u8 *size, ulli *gpu_freq)  
{  
    FILE    *fp = NULL; 
	char name[] = "/available_frequencies";
	char *path = get_certain_path(file, name);
    fp = fopen(path, "r");  
    u8 i = 0;  
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:", path);  
        return INVALID_ARGV;  
    }  
    // even freq     
    // odd count  
    for(i=0; i<MAX_AVALLABLE_FREQ; i=i+2){  
        gpu_freq[i+1] = 0;  
        if(fscanf(fp,"%lld", gpu_freq+i)!=1)  
            break;  
    }  
    *size = i;  
    fclose(fp);  
    return 0;  
} 

static void debug_cpufreq_count(u8 gpu_size,  ulli *gpu_freq)  
{  
    u8 i = 0;  
    for(i=0;i<gpu_size; i=i+2){  
        printf("%lld\t%lld\n", gpu_freq[i], gpu_freq[i+1]);  
    }  
}  

static s8 get_data(const char *file_path, uli *ret_data)  
{  
    FILE    *fp = NULL;  
    uli data = 0;  
    fp = fopen(file_path, "r");  
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:\n", file_path);  
        return INVALID_ARGV;  
    }  
    fscanf(fp, "%ld", &data);  
    fclose(fp);  
	*ret_data = data;
    return 0;  
}


static int get_utilisation(uli *cur_utilisation)
{
	FILE *fp = NULL;
	uli data = 0;  
	fp = popen("cat /d/pvr/status | grep \"GPU Utilisation\" | grep -o -E '[0-9]*%' | sed 's/%//g'", "r");
	if(fp==NULL){
		fprintf(stderr, "failed execute shell\n");  
        return INVALID_ARGV;  
	}
	fscanf(fp, "%ld", &data);  
	*cur_utilisation = data;
	pclose(fp);
	return 0;
}

static s8 data_simpling(ulli *gpu_available_freq, ulli *gpu_freq_utilisation, uli counts, u8 gpu_size, uli polling_interval, char *cur_path)
{
	uli n = 0, cur_freq =0, cur_utilisation;
	s8  ret = 0, i = 0 ;
	struct timeval start, end;  
    float timeuse = 0.0;
	//gettimeofday(&start, NULL); 
	for(n=0; n<counts; n++){
		if(get_data(cur_path, &cur_freq)<0){
			return ERROR;
		}
		for(i=0; i<gpu_size; i = i+2){
			if(cur_freq==gpu_available_freq[i]){
				gpu_available_freq[i+1]++;
				if(get_utilisation(&cur_utilisation)<0){
					return ERROR;
				}
				gpu_freq_utilisation[i+1]+=cur_utilisation;
				break;
			}
		}
		usleep((polling_interval*0.01)*1000);
	}
	//gettimeofday(&end, NULL);  
	//timeuse = (1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec)/1000.0;  
	//printf("time_use is %.2fms\n", timeuse); 
	return ret;
}

int main(int argc, char *argv[])  
{ 
	si   accummulate_time = 0;
	u8   gpu_size=0, i=0, utl_size =0;  
	uli polling_interval = 0, counts = 0;
	ulli gpu_utilisation_total = 0;
    ulli   gpu_available_freq[MAX_AVALLABLE_FREQ], gpu_freq_utilisation[MAX_AVALLABLE_FREQ];  
	char *file_path;
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        fprintf(stderr, "expl: ./data/local/tmp/gpu_loading -t 60(s)\n");  
        return INVALID_ARGV;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
		fprintf(stderr, "expl: ./data/local/tmp/gpu_loading -t 60(s)\n");  
        return INVALID_ARGV;  
	} 
	// input argc 1  
    if(argc==1){  
        accummulate_time = DEFAULT_TIME;  		
    // input argc 3  or more
    }else if(argc==3){
		if(check_argv(argv[1], &accummulate_time, argv[2])<0)
			return INVALID_ARGV;
    }
	printf("accummulate_time is %d(s)\n", accummulate_time); 
	file_path = access_dir_exist();
	if(file_path==NULL){
		return ERROR;
	}
	if(get_init_freq_count(file_path, &gpu_size, gpu_available_freq)<0)
		return ERROR;
	if(get_init_freq_count(file_path, &utl_size, gpu_freq_utilisation)<0)
		return ERROR;
	
	// debug
	//debug_cpufreq_count(gpu_size, gpu_available_freq);
	//debug_cpufreq_count(utl_size, gpu_freq_utilisation);
	//printf("gpu_size is %d\n", gpu_size);
	
	if(get_data(get_certain_path(file_path, "/polling_interval"), &polling_interval))
		return ERROR;
	printf("polling interval is %d(ms)\n", polling_interval);
	counts = accummulate_time*1000/polling_interval;
	printf("counts is %d\n", counts);
	
	
	data_simpling(gpu_available_freq, gpu_freq_utilisation, counts, gpu_size, polling_interval, get_certain_path(file_path, "/cur_freq"));
	// debug
	//debug_cpufreq_count(gpu_size, gpu_available_freq);
	//debug_cpufreq_count(gpu_size, gpu_freq_utilisation);
	
	for(i=0; i<gpu_size; i=i+2){
		printf("%.f: %lld\t\t", gpu_available_freq[i]*1.0/1000000, gpu_available_freq[i+1]);
		gpu_utilisation_total += gpu_freq_utilisation[i+1];
	}
	printf("\n\n");
	
	for(i=0; i<gpu_size; i=i+2){
		if(gpu_available_freq[i+1]!=0){
			printf("%.f: %.2f%\t utilisation:%.2f%\n", gpu_available_freq[i]*1.0/1000000, 
			gpu_available_freq[i+1]*1.0/counts*100, gpu_freq_utilisation[i+1]*1.0/gpu_available_freq[i+1]);
		}
	}
	printf("Averag GPU Utilisation : %.2f%\n", gpu_utilisation_total*1.0/counts);
	return 0;
}