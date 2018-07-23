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

#include "fix_gpu_freq.h"

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
	char *file_path = get_certain_path(path, "/cur_freq");
	fp = fopen(file_path, "r");
	if(fp==NULL){
		fprintf(stderr, "faile open the file %s\n", file_path);
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

static int fix_freq(char *desc, int  cpu_size, ui *gpu_available_freq, char *path)
{
	int i = 0;
	printf("%s\n", desc);
	for(i=0; i<cpu_size; i++){
		printf("%d\t", gpu_available_freq[i]);
	}
	printf("\n");
	char select_freq[SIZE];
	int selected_freq = 0;
	int match_flag = 0;
	printf("input fix freq\n");
	scanf("%s", select_freq);
	selected_freq = CharToInt(select_freq);
	for(i=0;i<cpu_size; i++){
		if(gpu_available_freq[i]==selected_freq)
			match_flag = 1;
	}
	if(match_flag==1){
		char *min_freq_path = get_certain_path(path, "/min_freq");
		char *max_freq_path = get_certain_path(path, "/max_freq");
		if(write_node(select_freq, min_freq_path)<0){
			return ERROR;
		}
		if(write_node(select_freq, max_freq_path)<0){
			return ERROR;
		}
	}else{
		fprintf(stderr, "You should avalible freq\n");
		return ERROR;
	}
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

static s8 get_init_freq(char *file, u8 *size, ui *gpu_available_freq)  
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
        if(fscanf(fp,"%d", gpu_available_freq+i)!=1)  
            break;  
    }  
    *size = i;  
    fclose(fp); 
    return 0;  
} 

int main(int argc, char *argv[])  
{
	u8   gpu_size;
	ui   gpu_available_freq[MAX_AVALLABLE_FREQ];  
	char *file_path;
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	file_path = access_dir_exist();
	if(file_path==NULL){
		return ERROR;
	}
	if(get_init_freq(file_path, &gpu_size, gpu_available_freq)<0)
		return ERROR;
	
	if(fix_freq("gpu avalible freq:", gpu_size, gpu_available_freq, file_path)<0){
		return ERROR;
	}
	
	if(read_node(file_path)==NULL){
		return ERROR;
	}
	return 0;
}