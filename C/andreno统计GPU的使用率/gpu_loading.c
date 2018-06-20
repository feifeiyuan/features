#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>  
#include <unistd.h>  
#include <sys/time.h>  
#include<unistd.h>

#include "gpu_loading.h"

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

static s8 get_init_freq_count(const char *gpu_cur_path, u8 *size, ulli *gpu_freq)  
{  
    FILE    *fp = NULL;  
    fp = fopen(gpu_cur_path, "r");  
    u8 i = 0;  
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:", gpu_cur_path);  
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

static void debug_cpufreq_count(u8 gpu_size,  uli *gpu_freq)  
{  
    u8 i = 0;  
    for(i=0;i<gpu_size; i=i+2){  
        printf("%ld\t%ld\n", gpu_freq[i], gpu_freq[i+1]);  
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

static s8 data_simpling(ulli *gpu_available_freq, ulli *gpu_freq_utilisation, uli counts, u8 gpu_size, uli polling_interval)
{
	uli n = 0, cur_freq =0, cur_utilisation;
	s8  ret = 0, i = 0 ;
	for(n=0; n<counts; n++){
		if(get_data(GPU_CUR_FREQ, &cur_freq)){
			return ERROR;
		}
		for(i=0; i<gpu_size; i = i+2){
			if(cur_freq==gpu_available_freq[i]){
				gpu_available_freq[i+1]++;
				if(get_data(GPU_CUR_UTILISATION, &cur_utilisation)){
					return ERROR;
				}
				gpu_freq_utilisation[i+1]+=cur_utilisation;
				break;
			}
		}
		usleep(polling_interval*1000);
	}
	return ret;
}

int main(int argc, char *argv[])  
{ 
	si   accummulate_time = 0;
	u8   gpu_size=0, i=0;  
	uli polling_interval = 0, counts = 0;
	ulli gpu_utilisation_total = 0;
    ulli   gpu_available_freq[MAX_AVALLABLE_FREQ], gpu_freq_utilisation[MAX_AVALLABLE_FREQ];  
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
	if(get_init_freq_count(GPU_AVAILABLE_FREQ_PATH, &gpu_size, gpu_available_freq)<0)
		return ERROR;
	if(get_init_freq_count(GPU_AVAILABLE_FREQ_PATH, &gpu_size, gpu_freq_utilisation)<0)
		return ERROR;
	
	// debug
	//debug_cpufreq_count(gpu_size, gpu_available_freq);
	//debug_cpufreq_count(gpu_size, gpu_freq_utilisation);
	//printf("gpu_size is %d\n", gpu_size);
	if(get_data(GPU_POLLING_INTERVAL, &polling_interval))
		return ERROR;
	printf("polling interval is %d(ms)\n", polling_interval);
	counts = accummulate_time*1000/polling_interval;
	//printf("counts is %d\n", counts);
	
	data_simpling(gpu_available_freq, gpu_freq_utilisation, counts, gpu_size, polling_interval);
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
}