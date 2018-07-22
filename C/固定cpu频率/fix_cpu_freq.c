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

#include "fix_cpu_freq.h"


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

static char * read_node(char *path)
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
	printf("cur_freq:%s\n", str);
	fclose(fp);
	return str;
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

static int fix_freq(char *desc, int  cpu_size, ui *cpu_available_freq, char *path)
{
	int i = 0;
	printf("%s\n", desc);
	for(i=0; i<cpu_size; i++){
		printf("%d\t", cpu_available_freq[i]);
	}
	printf("\n");
	char select_freq[SIZE];
	int selected_freq = 0;
	int match_flag = 0;
	printf("input fix freq\n");
	scanf("%s", select_freq);
	selected_freq = CharToInt(select_freq);
	for(i=0;i<cpu_size; i++){
		if(cpu_available_freq[i]==selected_freq)
			match_flag = 1;
	}
	if(match_flag==1){
		if(write_node(select_freq, path)){
			return ERROR;
		}
	}else{
		fprintf(stderr, "You should avalible freq\n");
		return ERROR;
	}
	return 0;
}

int main(int argc, char *argv[])  
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
	if(processor_num>LIT_CLUSTER_END){
		if(get_init_freq_count(CPU4_AVAILABLE_FREQ_PATH, &cpu4_size, cpu4_available_freq)<0)
			return ERROR;
	} 
	if(fix_freq("lit core", cpu0_size, cpu0_available_freq, CPU0_SET_FREQ_PATH)<0){
		return ERROR;
	}
	if(read_node(CPU0_CUR_FREQ_PATH)==NULL){
		return ERROR;
	}
	
	if(processor_num>LIT_CLUSTER_END){
		if(fix_freq("big core", cpu4_size, cpu4_available_freq, CPU4_SET_FREQ_PATH)<0){
			return ERROR;
		}
		if(read_node(CPU4_CUR_FREQ_PATH)==NULL){
			return ERROR;
		}
	}
	return 0;
}