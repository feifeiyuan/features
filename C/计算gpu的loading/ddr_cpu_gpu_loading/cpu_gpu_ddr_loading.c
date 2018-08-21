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

#include "cpu_gpu_ddr_loading.h"

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

static si check_argv(char *tag, si *time, char *value)
{
	si argv_time = *time;
	//printf("%d\n",strcmp(tag,"-i"));
	if(strcmp(tag,"-ct")==0){
		argv_time = get_argv(value); 
		if(argv_time<0)
			return INVALID_ARGV;    			 
	}else if(strcmp(tag,"-gt")==0){
		argv_time = get_argv(value); 
		if(argv_time<0)
			return INVALID_ARGV; 
	}else if(strcmp(tag,"-dt")==0){
		argv_time = get_argv(value); 
		if(argv_time<0)
			return INVALID_ARGV; 
	}else{
		fprintf(stderr, "you must input correct argvs\n");   
		fprintf(stderr, "expl: ./data/local/tmp/cpu_sgm -i 100(ms) -t 60(s)\n");  
		return INVALID_ARGV; 
	}
	if(argv_time==0&&strcmp(tag,"-ct")==0)
		argv_time = DEFAULT_CPU_TIME;
	if(argv_time==0&&strcmp(tag,"-gt")==0)
		argv_time = DEFAULT_GPU_TIME;
	if(argv_time==0&&strcmp(tag,"-dt")==0)
		argv_time = DEFAULT_DDR_TIME;
	*time = argv_time;
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

static int get_vdd_lit()
{
	char *file_path = NULL;
	if(!access("/sys/kernel/debug/fan53555-reg/voltage",F_OK)){
		file_path = "/sys/kernel/debug/fan53555-reg/voltage";
	}else if(!access("/sys/kernel/debug/sprd-regulator/vddarm0/voltage",F_OK)){
		file_path = "/sys/kernel/debug/sprd-regulator/vddarm0/voltage";
	}else if(!access("/sys/kernel/debug/sprd-regulator/vddcpu/voltage",F_OK)){
		file_path = "/sys/kernel/debug/sprd-regulator/vddcpu/voltage";
	}else if(!access("/sys/kernel/debug/sprd-regulator/vddcore/voltage",F_OK)){
		file_path = "/sys/kernel/debug/sprd-regulator/vddcore/voltage";
	}else{
		printf("we do not find correct path for GPU info\n");
		return ERROR;
	}
	
	int current_freq = 0;
	FILE *fp = NULL;
	fp = fopen(file_path, "r");
	if(fp==NULL){
		fprintf(stderr, "failed opend file:%s", file_path);  
        return FAILED_OPEN_FILE;  
	}
	fscanf(fp,"%d", &current_freq);
	printf("%d\n", current_freq);
	fclose(fp);
	return current_freq;
}

static int get_vdd_big()
{
	char *file_path = NULL;
	if(!access("/sys/kernel/debug/sprd-regulator/vddarm1/voltage",F_OK)){
		file_path = "/sys/kernel/debug/sprd-regulator/vddarm1/voltage";
	}else if(!access("/sys/kernel/debug/sprd-regulator/vddcpu/voltage",F_OK)){
		file_path = "/sys/kernel/debug/sprd-regulator/vddcpu/voltage";
	}else{
		printf("we do not find correct path for GPU info\n");
		return ERROR;
	}
	
	//printf("%s\n", file_path);
	int current_freq = 0;
	FILE *fp = NULL;
	fp = fopen(file_path, "r");
	if(fp==NULL){
		fprintf(stderr, "failed opend file:%s", file_path);  
        return FAILED_OPEN_FILE;  
	}
	fscanf(fp,"%d", &current_freq);
	printf("%d\n", current_freq);
	fclose(fp);
	return current_freq;
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
	uli  temp_total_time = total_time;
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
		float freq_perc = cpu_available_freq_start[i+1]*1.0/temp_total_time;
		printf("%.2f%\t", freq_perc*100);
	}
	double sum = 0;
	for(i=0; i<cpu_size; i=i+2){
		sum += (sum_arr[i+1]*cpu_available_freq_start[i]);
	}
	printf("\nsum:%.2f\n\n", sum);
	return sum;
}


static int get_cpu_loading(si accummulate_time)
{
	u8   cpu0_size, cpu4_size;
	int i = 0;
	ui   cpu0_available_freq_start[MAX_AVALLABLE_FREQ], cpu4_available_freq_start[MAX_AVALLABLE_FREQ];  
	ui   cpu0_available_freq_end[MAX_AVALLABLE_FREQ], cpu4_available_freq_end[MAX_AVALLABLE_FREQ];  
	si processor_num = get_processor_num();
	if(processor_num<=0){
		return ERROR;
	}
	printf("cpu_time is %d\n", accummulate_time);
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


// GPU
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

static s8 get_init_gpu_freq_count(char *file, u8 *size, ulli *gpu_freq)  
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

static char * read_node_str(char *path)
{
	FILE *fp  = NULL;
	char buffer[BUFFER_SIZE], *str;
	fp = fopen(path, "r");
	if(fp==NULL){
		fprintf(stderr, "faile open the file %s\n", path);
		return NULL;
	}
	fgets(buffer, BUFFER_SIZE, fp);
	str = (char *)malloc(sizeof(buffer));
	if(str==NULL){
		fprintf(stderr, "%s there is no space\n", __func__);
		return NULL;
	}
	memset(str, 0, sizeof(buffer));
	strcat(str,buffer);
	fclose(fp);
	return str;
}

static void print_other_info(u8 ddr_size, ui *ddr_available_freq)
{
	printf("%s\n", "unit:10ms");
	int i = 0;
	printf("theory_bw = ");
	for(i=0; i<ddr_size; i=i+2){
		printf("%d\t", ddr_available_freq[i]*8000);
	}
	printf("\n");
	printf("overflow  = %s", read_node_str(DDR_OVERFLOW));
	printf("underflow = %s", read_node_str(DDR_UNDERFLOW));
}

static int read_node(char *path)
{
	FILE *fp  = NULL;
	int freq = 0;
	fp = fopen(path, "r");
	if(fp==NULL){
		fprintf(stderr, "faile open the file %s\n", path);
		return FAILED_OPEN_FILE;
	}
	fscanf(fp, "%d", &freq);
	fclose(fp);
	return freq;
}

static int data_ddr_simpling(si accummulate_time, u8 ddr_size, ui *ddr_available_freq)
{
	struct timeval start, end;  
    float timeuse = 0.0;
	uli total_count = 0, count = 0;
	int freq = 0, i = 0;
	int interval = 2;
	total_count = accummulate_time*1000/interval;
	gettimeofday(&start, NULL);  
	while(count<total_count){
		freq = read_node(DDR_CUR_FREQ);
		if(freq<0){
			return ERROR;
		}
		for(i=0; i<ddr_size; i=i+2){
			if(freq == ddr_available_freq[i]){
				ddr_available_freq[i+1]++;
				break;
			}
		}
		count++;
		usleep((interval-1)*1000);
	}
	gettimeofday(&end, NULL);  
	timeuse = (1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec)/1000.0;  
	//printf("time_use is %.2fms\n", timeuse); 
	
	for(i=0; i<ddr_size; i=i+2){
		printf("%d: %20d\t\t%.2f%\n", ddr_available_freq[i], ddr_available_freq[i+1], ddr_available_freq[i+1]*1.0*100/total_count);
	}
	return 0;
}

static int get_ddr_loading(si accummulate_time)
{
	u8   ddr_size;
	ui   ddr_available_freq[MAX_AVALLABLE_FREQ];  
	printf("\n ddr_time is %d\n", accummulate_time);
	if(get_init_freq_count(DDR_AVAILABLE_FREQ_PATH, &ddr_size, ddr_available_freq)<0)
		return ERROR;
	
	if(data_ddr_simpling(accummulate_time, ddr_size, ddr_available_freq)){
		return ERROR;
	}
	print_other_info(ddr_size, ddr_available_freq);
	return 0;
}

static int get_gpu_loading(si gpu_time)
{
	u8   gpu_size=0, i=0, utl_size =0;  
	uli polling_interval = 0, counts = 0;
	ulli gpu_utilisation_total = 0;
    ulli   gpu_available_freq[MAX_AVALLABLE_FREQ], gpu_freq_utilisation[MAX_AVALLABLE_FREQ];  
	char *file_path;
	printf("\ngpu_time is %d(s)\n", gpu_time); 
	file_path = access_dir_exist();
	if(file_path==NULL){
		return ERROR;
	}
	if(get_init_gpu_freq_count(file_path, &gpu_size, gpu_available_freq)<0)
		return ERROR;
	if(get_init_gpu_freq_count(file_path, &utl_size, gpu_freq_utilisation)<0)
		return ERROR;
	
	// debug
	//debug_cpufreq_count(gpu_size, gpu_available_freq);
	//debug_cpufreq_count(utl_size, gpu_freq_utilisation);
	//printf("gpu_size is %d\n", gpu_size);
	
	if(get_data(get_certain_path(file_path, "/polling_interval"), &polling_interval))
		return ERROR;
	//printf("polling interval is %d(ms)\t", polling_interval);
	counts = gpu_time*1000/polling_interval;
	//printf("counts is %d\n", counts);
	
	
	data_simpling(gpu_available_freq, gpu_freq_utilisation, counts, gpu_size, polling_interval, get_certain_path(file_path, "/cur_freq"));
	// debug
	//debug_cpufreq_count(gpu_size, gpu_available_freq);
	//debug_cpufreq_count(gpu_size, gpu_freq_utilisation);
	
	for(i=0; i<gpu_size; i=i+2){
		//printf("%.f: %lld\t\t", gpu_available_freq[i]*1.0/1000000, gpu_available_freq[i+1]);
		gpu_utilisation_total += gpu_freq_utilisation[i+1];
	}
	
	for(i=0; i<gpu_size; i=i+2){
		if(gpu_available_freq[i+1]!=0){
			printf("%.f: %.2f%\t utilisation:%.2f%\n", gpu_available_freq[i]*1.0/1000000, 
			gpu_available_freq[i+1]*1.0/counts*100, gpu_freq_utilisation[i+1]*1.0/gpu_available_freq[i+1]);
		}
	}
	printf("Averag GPU Utilisation : %.2f%\n", gpu_utilisation_total*1.0/counts);
}

static int write_node(char *value, char *path)
{
	FILE *fp  = NULL;
	fp = fopen(path, "w");
	if(fp==NULL){
		fprintf(stderr, "faile open the file %s\n", path);
		return ERROR;
	}
	fprintf(fp, "%s", value);
	fclose(fp);
	return 0;
}

static int CharToInt(char *str)
{
	int freq = 0;
	char *ptr = str;
	while(*ptr){
		freq = freq*10+(*ptr-'0');
		ptr++;
	}
	return freq;
} 


static int write_node_int(int value, char *path)
{
	FILE *fp  = NULL;
	fp = fopen(path, "w");
	if(fp==NULL){
		fprintf(stderr, "faile open the file %s\n", path);
		return ERROR;
	}
	fprintf(fp, "%d", value);
	printf("%d\t", value);
	fclose(fp);
	return 0;
}

static int fix_freq_get_vdd(int cpu_size, int *cpu_available_freq, char *path, char *governor_path)
{
	int i = 0;
	char *default_governor;
	default_governor = read_node_str(governor_path);
	write_node("userspace", governor_path);
	for(i=0; i<cpu_size; i++){
		if(write_node_int(cpu_available_freq[i], path)){
			return ERROR;
		}
		
		if(strcmp(governor_path, LLT_GOVERNOR)==0){
			if(get_vdd_lit()<0){
				return ERROR;
			}
		}else{
			if(get_vdd_big()<0){
				return ERROR;
			}
		}
	}
	write_node(default_governor, governor_path);
	return 0;
}


static int get_vdd()
{
	u8   cpu0_size, cpu4_size;
	ui   cpu0_available_freq[MAX_AVALLABLE_FREQ], cpu4_available_freq[MAX_AVALLABLE_FREQ];  
	
	si 	 processor_num = 0;
	processor_num = get_processor_num();
	if(processor_num<=0){
		return ERROR;
	}
	if(get_init_freq_count(CPU0_AVAILABLE_FREQ_PATH, &cpu0_size, cpu0_available_freq)<0)
		return ERROR;
	printf("\ndvfs table\nlit core:\n");
	printf("/* kHz	uV */\n");
	fix_freq_get_vdd(cpu0_size, cpu0_available_freq, CPU0_SET_FREQ_PATH, LLT_GOVERNOR);
	
	if(processor_num>LIT_CLUSTER_END){
		if(get_init_freq_count(CPU4_AVAILABLE_FREQ_PATH, &cpu4_size, cpu4_available_freq)<0)
			return ERROR;
		printf("\nbig core:\n");
		printf("/* kHz	uV */\n");
		fix_freq_get_vdd(cpu4_size, cpu4_available_freq, CPU4_SET_FREQ_PATH, BIG_GOVERNOR);
	}	
}


int main(int argc, char *argv[])
{
	si   cpu_time = 0;
	si   gpu_time = 0;
	si   ddr_time = 0;
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
        cpu_time = DEFAULT_CPU_TIME; 
		gpu_time = DEFAULT_GPU_TIME;
		ddr_time = DEFAULT_DDR_TIME;
    // input argc 3  or more
    }else if(argc <= 7){
		if(check_argv(argv[1], &cpu_time, argv[2])<0)
			return INVALID_ARGV;
		if(check_argv(argv[1], &gpu_time, argv[2])<0)
			return INVALID_ARGV;
		if(check_argv(argv[1], &ddr_time, argv[2])<0)
			return INVALID_ARGV;
		if(argc >= 5){
			if(check_argv(argv[3], &cpu_time, argv[4])<0)
				return INVALID_ARGV;
			if(check_argv(argv[3], &gpu_time, argv[4])<0)
				return INVALID_ARGV;
			if(check_argv(argv[3], &ddr_time, argv[4])<0)
				return INVALID_ARGV;
		}
		if(argc >= 7){
			if(check_argv(argv[5], &cpu_time, argv[6])<0)
				return INVALID_ARGV;
			if(check_argv(argv[5], &gpu_time, argv[6])<0)
				return INVALID_ARGV;
			if(check_argv(argv[5], &ddr_time, argv[6])<0)
				return INVALID_ARGV;
		}
	}
	
	if(cpu_time==0)
		cpu_time = DEFAULT_CPU_TIME;
	if(gpu_time==0)
		gpu_time = DEFAULT_GPU_TIME;
	if(ddr_time==0)
		ddr_time = DEFAULT_DDR_TIME;
	
	get_cpu_loading(cpu_time);
	get_vdd();
	
	get_gpu_loading(gpu_time);
	
	get_ddr_loading(ddr_time);
}