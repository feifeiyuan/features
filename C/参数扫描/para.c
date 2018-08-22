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

#include "para.h"
#include "dvfs_table.h"
#include "top.h"
#include "power_hint.h"

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

static char *get_certain_path(char *file, char *file_name)
{	
	//printf("file name is %s\n", file_name);
	char file_path[BUFFER_SIZE];
	char *pattern = "*.gpu";
	char *path;
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
				strcat(file_path, file_name);
				//printf("file_path:%s\n", file_path);
				break;
			}else if(ret == FNM_NOMATCH){
				continue;
			}else{
				printf("error file=%s\n", entry->d_name);
			}
		}
		closedir(dir);
	}
	path = (char *)malloc(BUFFER_SIZE);
	if(path==NULL){
		fprintf(stderr, "%s:there is no space\n", __func__);
		return NULL;
	}
	strcpy(path, file_path);
	return path;
}

static s8 get_gpu_init_freq(char *file, u8 *size, ui *gpu_available_freq)  
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


static int get_gpu_freq()
{
	u8   gpu_size;
	ui   gpu_available_freq[MAX_AVALLABLE_FREQ];  
	char *file_path;
	file_path = access_dir_exist();
	if(file_path==NULL){
		return ERROR;
	}
	if(get_gpu_init_freq(file_path, &gpu_size, gpu_available_freq)<0)
		return ERROR;
	int i = 0 ;
	printf("gpu_freq: \t");
	for(i=0; i<gpu_size; i++){
		printf("%d\t", gpu_available_freq[i]);
	}
	printf("\n");
}


static int print_other_info(u8 ddr_size, ui *ddr_available_freq)
{
	int i = 0;
	printf("\ntheory_bw\t");
	for(i=0; i<ddr_size; i++){
		printf("%d\t", ddr_available_freq[i]*8000);
	}
	printf("\n");
	u8   over_size = 0, under_size = 0;
	ui   over_size_freq[MAX_AVALLABLE_FREQ], under_size_freq[MAX_AVALLABLE_FREQ];  
	if(get_init_freq_count(DDR_OVERFLOW, &over_size, over_size_freq)<0)
		return ERROR;
	if(get_init_freq_count(DDR_UNDERFLOW, &under_size, under_size_freq)<0)
		return ERROR;
	
	printf("overflow\t");
	for(i=0; i<over_size; i++){
		printf("%d\t", over_size_freq[i]);
	}
	printf("\n");
	printf("underflow\t");
	for(i=0; i<under_size; i++){
		printf("%d\t", under_size_freq[i]);
	}
	printf("\n");
	return ;
}


static int get_ddr_info()
{
	u8   ddr_size = 0;
	ui   ddr_available_freq[MAX_AVALLABLE_FREQ];  
	if(get_init_freq_count(DDR_AVAILABLE_FREQ_PATH, &ddr_size, ddr_available_freq)<0)
		return ERROR;
	printf("ddr_freq: \t");
	int i = 0;
	for(i=0; i<ddr_size; i++){
		printf("%d\t", ddr_available_freq[i]);
	}
	print_other_info(ddr_size, ddr_available_freq);
}

static int get_thermal_temp()
{
	char node[BUFFER_SIZE] ;
	printf("%s: ", THERMAL_ZONE0);
	strcpy(node, THERMAL_ZONE0);
	strcat(node, "/temp");
	printf("%s", read_node_str(node));
	strcpy(node, "");
	printf("%s: ", THERMAL_ZONE1);
	strcpy(node, THERMAL_ZONE1);
	strcat(node, "/temp");
	printf("%s", read_node_str(node));
	strcpy(node, "");
	printf("%s: ", THERMAL_ZONE3);
	strcpy(node, THERMAL_ZONE3);
	strcat(node, "/temp");
	printf("%s", read_node_str(node));
}

int main()
{
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	

	printf("\n**************************  GPU  *****************************\n");
	get_gpu_freq();
	printf("\n**************************  DDR  *****************************\n");
	get_ddr_info();
	printf("\n**************************thermal*****************************\n");
	get_thermal_temp();
	printf("\n**************************  CPU  *****************************\n");
	printf("dvfs_table:\n");
	get_vdd();
	printf("\nthread_cpu load(top):\n");
	get_top();
	printf("\n**************************power hint**************************\n");
	get_file_content();
	printf("\n**************************pwctl config *****************************\n");
	get_ctrl_file();
	get_app_power_control();
}
