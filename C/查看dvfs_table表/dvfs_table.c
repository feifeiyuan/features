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

#include "dvfs_table.h"

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

static char * IntToChar(int cpu)
{
	static char num[SIZE] ;
	if(cpu>MAX_CPU||cpu<MIN_CPU){
		return NULL;
	}
	sprintf(num, "%d", cpu);
	return num;
}

static int close_idle(char *cpu, char *state)
{
	char path[BUFFER_SIZE] = "/sys/devices/system/cpu/cpu";
	strcat(path, cpu);
	strcat(path, "/cpuidle/");
	strcat(path, state);
	strcat(path, "/disable");
	//printf("%s\n", path);
	
	FILE *fp = NULL;
	char value[] = "1";
	fp = fopen(path, "w");
	if(fp==NULL){
		fprintf(stderr, "faile open the file %s\n", path);
		return ERROR;
	}
	fprintf(fp, "%s", value);
	fclose(fp);
	return 0;
}

static s8 get_init_freq_count(const char *cpu_cur_path, u8 *size, ui *cpu_available_freq)  
{  
    FILE    *fp = NULL;  
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

static int write_node(int value, char *path)
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

static int switch_governor(char *path, char *governor)
{
	FILE *fp  = NULL;
	fp = fopen(path, "w");
	if(fp==NULL){
		fprintf(stderr, "faile open the file %s\n", path);
		return ERROR;
	}
	fprintf(fp, "%s", governor);
	fclose(fp);
	return 0;
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

static int fix_freq_get_vdd(int cpu_size, int *cpu_available_freq, char *path, char *governor_path)
{
	int i = 0;
	char *default_governor = NULL;
	default_governor = read_node_str(governor_path);
	switch_governor(governor_path, "userspace");
	for(i=0; i<cpu_size; i++){
		if(write_node(cpu_available_freq[i], path)){
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
	switch_governor(governor_path, default_governor);
	return 0;
}

int main()
{
	u8   cpu0_size, cpu4_size;
	ui   cpu0_available_freq[MAX_AVALLABLE_FREQ], cpu4_available_freq[MAX_AVALLABLE_FREQ];  
	
	si 	 processor_num = 0;
	processor_num = get_processor_num();
	if(processor_num<=0){
		return ERROR;
	}
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	if(get_init_freq_count(CPU0_AVAILABLE_FREQ_PATH, &cpu0_size, cpu0_available_freq)<0)
		return ERROR;
	printf("lit core:\n");
	printf("/* kHz	uV */\n");
	fix_freq_get_vdd(cpu0_size, cpu0_available_freq, CPU0_SET_FREQ_PATH, LLT_GOVERNOR);
	
	if(processor_num>LIT_CLUSTER_END){
		if(get_init_freq_count(CPU4_AVAILABLE_FREQ_PATH, &cpu4_size, cpu4_available_freq)<0)
			return ERROR;
		printf("\nbig core:\n");
		printf("/* kHz	uV */\n");
		fix_freq_get_vdd(cpu4_size, cpu4_available_freq, CPU4_SET_FREQ_PATH, BIG_GOVERNOR);
	}
	
	int i = 0;
	for(i=0; i<processor_num; i++){
		//close_idle(IntToChar(i), "state0");
		//close_idle(IntToChar(i), "state1");
	}
	
}