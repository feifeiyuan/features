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

#include "cpu_loading.h"

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

static si get_processor_num()
{
	s8 cpu_num = 0;
	static char file_path[BUFFER_SIZE] = "/sys/devices/system/cpu";
	char *pattern = "cpu?";
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	si ret = 0;
	dir = opendir(file_path);
	if(dir !=NULL){
		while((entry = readdir(dir)) !=NULL){
			ret = fnmatch(pattern, entry->d_name, FNM_PATHNAME|FNM_PERIOD);
			if(ret==0){
				cpu_num++;
			}else if(ret == FNM_NOMATCH){
				continue;
			}else{
				printf("error file=%s\n", entry->d_name);
			}
		}
		closedir(dir);
	}
	return cpu_num;
}

static s8 get_init_freq_count(const char *cpu_cur_path, u8 *size, ui *cpu_available_freq)  
{  
    FILE    *fp = NULL;  
	memset(cpu_available_freq, 0, MAX_AVALLABLE_FREQ);
    fp = fopen(cpu_cur_path, "r");  
    u8 i = 0;  
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:", cpu_cur_path);  
        return INVALID_ARGV;  
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

static int cal_state_time(u8 cpu0_size, u8 cpu4_size, ui *cpu0_available_freq_start, \
ui *cpu4_available_freq_start, ui *cpu0_available_freq_end, ui *cpu4_available_freq_end, si processor_num, uli *lit_total_time, uli *big_total_time)
{
	int i=0;
	uli  lit_time = 0, big_time = 0;
	for(i=0; i<cpu0_size; i=i+2){
		cpu0_available_freq_start[i+1] = cpu0_available_freq_end[i+1]-cpu0_available_freq_start[i+1];
		lit_time += cpu0_available_freq_start[i+1];
		//printf("%d: %d\n",cpu0_available_freq_start[i] ,cpu0_available_freq_start[i+1]  );
	}
	if(processor_num>LIT_CLUSTER_END){
		for(i=0; i<cpu4_size; i=i+2){
			cpu4_available_freq_start[i+1] = cpu4_available_freq_end[i+1]-cpu4_available_freq_start[i+1];
			big_time += cpu4_available_freq_start[i+1];
			//printf("%d: %d\n",cpu4_available_freq_start[i] ,cpu4_available_freq_start[i+1]);
		}
	}
	*lit_total_time = lit_time;
	*big_total_time = big_time;
	
	return 0;
}

static int cal_idle_time(long long int *cpu_idle_state0_start, long long int *cpu_idle_state1_start, \
long long int *cpu_idle_state0_end, long long int *cpu_idle_state1_end, int processor_num)
{
	int i =0;
	long long int state0 = 0, state1 = 0;
	for(i=0; i<LIT_CLUSTER_END; i++){
		state0 = cpu_idle_state0_end[i] - cpu_idle_state0_start[i];
		//printf("state0:%lld\n", state0);
		state1 = cpu_idle_state1_end[i] - cpu_idle_state1_start[i];
		//printf("state1:%lld\n", state1);
		cpu_idle_state0_start[i] = state1 + state0;
		//printf("idle:%lld\n", cpu_idle_state0_start[i]);
	}
	printf("\n");
	if(processor_num>LIT_CLUSTER_END){
		for(i=LIT_CLUSTER_END; i<processor_num; i++){
			state0 = cpu_idle_state0_end[i] - cpu_idle_state0_start[i];
			//printf("state0:%lld\n", state0);
			state1 = cpu_idle_state1_end[i] - cpu_idle_state1_start[i];
			//printf("state1:%lld\n", state1);
			cpu_idle_state0_start[i] = state1 + state0;
			//printf("idle:%lld\n", cpu_idle_state0_start[i]);
		}
	}
	return 0;
}

static char * IntToChar(int cpu)
{
	static char num[SIZE] ;
	sprintf(num, "%d", cpu);
	return num;
}

static long long int get_idle_time(int cpu, char *state)
{
	char path[BUFFER_SIZE] = "/sys/devices/system/cpu/cpu";
	strcat(path, IntToChar(cpu));
	strcat(path, "/cpuidle/");
	strcat(path, state);
	strcat(path, "/time");
	//printf("%s\n", path);
	
	FILE *fp = NULL;
	long long int value = 0;
	fp = fopen(path, "r");
	if(fp==NULL){
		fprintf(stderr, "faile open the file %s\n", path);
		return ERROR;
	}
	fscanf(fp,"%lld", &value);
	fclose(fp);
	return value;
}


static double dis_result(u8 cpu_size, ui *cpu_available_freq_start, int start, int end, uli total_time,  long long int *cpu_idle_state0_start)
{
	int i = 0, j = 0;
	printf("\t");
	for(i=0; i<cpu_size; i=i+2){
		printf("%d\t", cpu_available_freq_start[i]);
	}
	printf("online\toffline\n");
	float sum_arr[cpu_size];
	for(i=0; i<cpu_size; i++){
		sum_arr[i] = 0.0;
	}
	for(j =start; j<end; j++){
		printf("cpu%d\t", j);
		if(total_time*10000<cpu_idle_state0_start[j]){
			total_time = cpu_idle_state0_start[j];
		}
		float active_per = (total_time*10000-cpu_idle_state0_start[j])*1.0/(total_time*10000);
		for(i=0; i<cpu_size; i=i+2){
			float freq_perc = cpu_available_freq_start[i+1]*1.0/total_time;
			float online_perc = active_per*freq_perc;
			printf("%.2f%\t", online_perc*100);
			sum_arr[i+1]+=online_perc;
		}
		printf("%.2f%\t%.2f%\n", active_per*100, (1-active_per)*100);
	}
	printf("\t");
	for(i=0; i<cpu_size; i=i+2){
		float freq_perc = cpu_available_freq_start[i+1]*1.0/total_time;
		printf("%.2f%\t", freq_perc*100);
	}
	double sum = 0;
	for(i=0; i<cpu_size; i=i+2){
		sum += (sum_arr[i+1]*cpu_available_freq_start[i]);
	}
	printf("\nsum:%.2f\n\n", sum);
	return sum;
}

int main(int argc, char *argv[])
{
	u8   cpu0_size, cpu4_size;
	ui   cpu0_available_freq_start[MAX_AVALLABLE_FREQ], cpu4_available_freq_start[MAX_AVALLABLE_FREQ];  
	ui   cpu0_available_freq_end[MAX_AVALLABLE_FREQ], cpu4_available_freq_end[MAX_AVALLABLE_FREQ];  
	si   accummulate_time = 0;
	si 	 processor_num = 0;
	int i =0;
	
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        fprintf(stderr, "expl: ./data/local/tmp/cpu_loading -t 60(s)\n");  
        return INVALID_ARGV;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
		fprintf(stderr, "expl: ./data/local/tmp/cpu_loading -t 60(s)\n");  
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
	
	processor_num = get_processor_num();
	if(processor_num<=0){
		return ERROR;
	}
	
	if(get_init_freq_count(POLICY0_TIME_STATS, &cpu0_size, cpu0_available_freq_start)<0){
		return ERROR;
	}
	long long int cpu_idle_state0_start[processor_num];
	long long int cpu_idle_state1_start[processor_num];
	for(i=0; i<LIT_CLUSTER_END; i++){
		cpu_idle_state0_start[i] = get_idle_time(i, "state0");
		cpu_idle_state1_start[i] = get_idle_time(i, "state1");
		//printf("%ld\t%ld\n", cpu_idle_state0_start[i], cpu_idle_state1_start[i]);
	}
	
	if(processor_num>LIT_CLUSTER_END){
		if(get_init_freq_count(POLICY4_TIME_STATS, &cpu4_size, cpu4_available_freq_start)<0)
			return ERROR;
		for(i=LIT_CLUSTER_END; i<processor_num; i++){
			cpu_idle_state0_start[i] = get_idle_time(i, "state0");
			cpu_idle_state1_start[i] = get_idle_time(i, "state1");
			//printf("%ld\t%ld\n", cpu_idle_state0_start[i], cpu_idle_state1_start[i]);
		}
	}
	sleep(accummulate_time);
	if(get_init_freq_count(POLICY0_TIME_STATS, &cpu0_size, cpu0_available_freq_end)<0){
		return ERROR;
	}
	long long int cpu_idle_state0_end[processor_num];
	long long int cpu_idle_state1_end[processor_num];
	for(i=0; i<LIT_CLUSTER_END; i++){
		cpu_idle_state0_end[i] = get_idle_time(i, "state0");
		cpu_idle_state1_end[i] = get_idle_time(i, "state1");
		//printf("%ld\t%ld\n", cpu_idle_state0_end[i], cpu_idle_state1_end[i]);
	}
	
	if(processor_num>LIT_CLUSTER_END){
		if(get_init_freq_count(POLICY4_TIME_STATS, &cpu4_size, cpu4_available_freq_end)<0)
			return ERROR;
		for(i=LIT_CLUSTER_END; i<processor_num; i++){
			cpu_idle_state0_end[i] = get_idle_time(i, "state0");
			cpu_idle_state1_end[i] = get_idle_time(i, "state1");
			//printf("%ld\t%ld\n", cpu_idle_state0_end[i], cpu_idle_state1_end[i]);
		}
	}
	uli lit_total_time = 0, big_total_time = 0;
	cal_state_time(cpu0_size, cpu4_size, cpu0_available_freq_start, \
	cpu4_available_freq_start, cpu0_available_freq_end, cpu4_available_freq_end, processor_num, &lit_total_time, &big_total_time);
	
	//printf("lit_total_time:%ld\n", lit_total_time);
	//printf("big_total_time:%ld\n", big_total_time);
	cal_idle_time(cpu_idle_state0_start, cpu_idle_state1_start, cpu_idle_state0_end, cpu_idle_state1_end, processor_num);
	
	double sum_lit = dis_result(cpu0_size, cpu0_available_freq_start, 0, LIT_CLUSTER_END, lit_total_time, cpu_idle_state0_start);

	if(processor_num>LIT_CLUSTER_END){
		double sum_big = dis_result(cpu4_size, cpu4_available_freq_start, LIT_CLUSTER_END, processor_num, big_total_time, cpu_idle_state0_start);
		printf("SUM:%.2f\n", sum_lit+sum_big);
	}
	
}