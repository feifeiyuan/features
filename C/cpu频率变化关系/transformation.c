#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>  
#include <unistd.h>  
#include <sys/time.h>  
#include<unistd.h>

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
	//printf("%s\n",tag);
	//printf("%d\n",strcmp(tag,"-i"));
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

static si get_processor_num()
{
	FILE *file_process = NULL;
	char buffer[BUFFER_SIZE];
	s8 cpu_num = 0;
	file_process = fopen("/proc/cpuinfo","r");
	if(file_process==NULL){
		fprintf(stderr, "%s:failed run commond\n", __func__, "/proc/cpuinfo");  
        return FAILED_OPEN_FILE; 
	}
	while(fgets(buffer, BUFFER_SIZE, file_process)!=NULL) {
		if(strstr(buffer,"processor")!=NULL){
			cpu_num++;
		}
	}
	fclose(file_process);
	return cpu_num;
}

static s8 get_init_freq(char *cpu_cur_path, u8 *size, ui *cpu_available_freq)  
{  
	u8 i = 0, j = 0;
	FILE *fp = NULL;  
	
    fp = fopen(cpu_cur_path, "r");   
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:%s", cpu_cur_path);  
        return FAILED_OPEN_FILE;  
    }  
    // even freq     
    // odd count  
    for(i=0; i<MAX_AVALLABLE_FREQ; i++){  
        if(fscanf(fp,"%d", cpu_available_freq+i)!=1)  
            break;  
    }  
    *size = i;  
    fclose(fp); 
    return 0;  
}  

static void print_debug(u8 size, struct transformation *trans_union)
{	
	u8 i = 0, j = 0;
	for(i=0;i<size;i++){
		if(i==0){
			printf("\t");
		}
		printf("%lld\t", trans_union[i].current);
	}
	for(i=0;i<size;i++){
		printf("\n%lld\t", trans_union[i*size].previous);
		for(j=0; j<size; j++){
			printf("%lld\t", trans_union[j+i*size].count);
		}
	}
	printf("\n\n");
}

static si get_init_transformation(u8 size, ui *cpu_available_freq, struct transformation *trans_union)
{
	u8 i = 0, j = 0;
	for(i=0; i<size; i++){
		for(j=0; j<size; j++){
			trans_union[j+i*size].previous = cpu_available_freq[i];
			trans_union[j+i*size].current = cpu_available_freq[j];
			trans_union[j+i*size].count = 0;
		}
	}
	// debug
	#if 0
	for(i=0;i<size*size;i++){
		printf("%lld\t%lld\t", trans_union[i].previous, trans_union[i].current);
		if((i+1)%size==0){
			printf("\n");
		}
	}
	#endif
	return 0;
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

static si data_lit(struct transformation *trans_union, u8 size, ulli counts)
{
	ulli current_freq = 0, times = 1, previous_freq = 0;
	previous_freq =  get_cur_freq(CPU0_CUR_FREQ_PATH);
	if(previous_freq<=0){
		return ERROR;
	}
	u8 i = 0, match = 0, j=0;
	while(times<counts){
		current_freq = get_cur_freq(CPU0_CUR_FREQ_PATH);
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

static si store_transition(u8 size, struct transformation *trans_union, ulli previous_freq, ulli current_freq)
{
	u8 i = 0, j = 0;
	for(i=0; i<size; i++){
		for(j=0;j<size;j++){
			if(trans_union[j+i*size].previous==previous_freq){
				if(trans_union[j+i*size].current == current_freq){
					trans_union[j+i*size].count++;
					return 0;
				}
			}else{
				break;
			}
		}
	}
	return 0;
}

static si data_lit_big(struct transformation *trans_union_lit, u8 size_lit,
struct transformation *trans_union_big, u8 size_big, ulli counts)
{
	ulli lit_current_freq = 0, times = 1, lit_previous_freq = 0;
	ulli big_current_freq = 0, big_previous_freq = 0;
	lit_previous_freq =  get_cur_freq(CPU0_CUR_FREQ_PATH);
	big_previous_freq =  get_cur_freq(CPU4_CUR_FREQ_PATH);
	if(lit_previous_freq<=0||big_previous_freq<0)
		return ERROR;
	while(times<counts){
		lit_current_freq = get_cur_freq(CPU0_CUR_FREQ_PATH);
		big_current_freq = get_cur_freq(CPU4_CUR_FREQ_PATH);
		if(lit_current_freq<=0||big_current_freq<=0){
			return ERROR;
		}
		store_transition(size_lit, trans_union_lit, lit_previous_freq, lit_current_freq);
		store_transition(size_big, trans_union_big, big_previous_freq, big_current_freq);
		lit_previous_freq = lit_current_freq;
		big_previous_freq = big_current_freq;
		times++;
		usleep(20*1000);
	}
	print_debug(size_lit, trans_union_lit);
	print_debug(size_big, trans_union_big);
	return 0;
}

int main(int argc, char *argv[])  
{
	si accumulate_time = 0, processor_num = 0; 
	u8   cpu0_size, cpu4_size;  
    ui   cpu0_available_freq[MAX_AVALLABLE_FREQ], cpu4_available_freq[MAX_AVALLABLE_FREQ];  
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
	
	processor_num = get_processor_num();
	if(processor_num<=0){
		printf("find cpu core num is not correct\n");
		return ERROR;
	}

	if(get_init_freq(CPU0_AVAILABLE_FREQ_PATH, &cpu0_size, cpu0_available_freq)<0)
			return ERROR;
	struct transformation trans_union_cpu0[cpu0_size*cpu0_size];
	get_init_transformation(cpu0_size, cpu0_available_freq, trans_union_cpu0);
		
	if(processor_num==LIT_CLUSTER_END){
		if(data_lit(trans_union_cpu0, cpu0_size, counts)<0){
			return ERROR;
		}
	}else if(processor_num>LIT_CLUSTER_END){
		if(get_init_freq(CPU4_AVAILABLE_FREQ_PATH, &cpu4_size, cpu4_available_freq)<0)
			return ERROR;
		struct transformation trans_union_cpu4[cpu4_size*cpu4_size];
		get_init_transformation(cpu4_size, cpu4_available_freq, trans_union_cpu4);
		
		if(data_lit_big(trans_union_cpu0, cpu0_size, trans_union_cpu4, cpu4_size, counts)<0){
			return ERROR;
		}
	} 
}