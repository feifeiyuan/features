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

#include "cpu_hotplug.h"


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
	fclose(fp);
	return str;
}

static int close_dynamic_hotplug()
{
	int ret = 0;
	char disable[] = "1";
	if(!access(DYNAMIC_HOTPLUG,F_OK)){
		ret = write_node(disable, DYNAMIC_HOTPLUG);
	}
	return 0;
}

static si close_thermal_IPA()
{
	int ret = 0;
	char thermal_enable[] = "0";
	char thermal_mode[] = "disabled";
	ret = write_node(thermal_enable, THERMAL_ENABLE);
	ret = write_node(thermal_mode, THERMAL_MODE);
	return ret;
}

static int show_core()
{
	char *core_online, *core_offline;
	core_online = read_node(CORE_ONLINE);
	if(core_online==NULL){
		return ERROR;
	}
	if(strlen(core_online)>1){
		printf("online: %s", core_online);
	}
	
	core_offline = read_node(CORE_OFFLINE);
	if(core_offline==NULL){
		return ERROR;
	}
	if(strlen(core_offline)>1){
		printf("offline: %s", core_offline);
	}
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



int main(int argc, char *argv[])  
{
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	
	// close dynamic hotplug
	if(close_dynamic_hotplug()<0){
		return ERROR;
	}
	
	// close thermal IPA
	if(close_thermal_IPA()<0){
		return ERROR;
	}
	
	// show core
	if(show_core()<0){
		return ERROR;
	}
	
	char switch_on[SIZE];
	printf("please choose 0(off)/1(on)\n");
	scanf("%s", switch_on);
	if(strcmp(switch_on, "0")!=0&&strcmp(switch_on, "1")!=0){
		printf("You should input 0/1\n");
		return ERROR;
	}
	
	char close_path[BUFFER_SIZE] = PER_ONLINE;
	if(strcmp(switch_on,"0")==0){
		char close_cpu[SIZE];
		printf("please choose close core 0~%d\n", get_processor_num());
		scanf("%s", close_cpu);
		strcat(close_path, close_cpu);
		strcat(close_path, "/online");
		if(write_node(switch_on, close_path)<0){
			return ERROR;
		}
	}else if(strcmp(switch_on,"1")==0){
		char open_cpu[SIZE];
		printf("please choose open core 0~%d\n", get_processor_num());
		scanf("%s", open_cpu);
		strcat(close_path, open_cpu);
		strcat(close_path, "/online");
		if(write_node(switch_on, close_path)<0){
			return ERROR;
		}
	}
	
	// show core
	if(show_core()<0){
		return ERROR;
	}
}