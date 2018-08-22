#include<stdio.h>
#include<stdlib.h>
#include<error.h>
#include<string.h>
#include "para.h"

#define PWCTL_CONFIG_FILE	"/system/etc/pwctl_config.xml"
#define APP_POWER_SAVING	"/system/etc/appPowerSaveConfig.xml"



static char * matchstr(char *tag, char *str, char start_char, char end_char)
{
	char *substr = strstr(str, tag);
	char buffer[BUFFER_SIZE];
	int i =  0, count = 0;
	while(*substr){
		if(*substr==start_char||count>0){
			if(*substr!=start_char&&*substr!=end_char){
				buffer[i++] = *substr;
			}else{
				count++;
			}
		}
		if(*substr==end_char&&i>0){
			buffer[i] = '\0';
			break;
		}
		substr++;
	}
	
	char *ret_str = (char *)malloc(strlen(buffer)+1);
	if(ret_str==NULL){
		fprintf(stderr, "%s:there is no space\n", __func__);
		return NULL;
	}
	strcpy(ret_str, buffer);
	return ret_str;
}

int get_ctrl_file()
{
	FILE  *fp = NULL;
	fp = fopen(PWCTL_CONFIG_FILE, "r");
	char buffer[BUFFER_SIZE];
	int start_flag = 0;
	while(fgets(buffer, BUFFER_SIZE, fp)){
		if(strstr(buffer, "<item")){
			printf("item:   %s\n", matchstr("name=",buffer, '"', '"'));
			start_flag = 1;
			continue;
		}
		if(strstr(buffer, "</item>")){
			start_flag = 0;
			continue;
		}
		if(start_flag){
			printf("packageName: %s\n", matchstr("packageName",buffer, '>', '<'));
		}
	}
	printf("\n");
	fclose(fp);
	return 0;
}

int get_app_power_control()
{
	FILE  *fp = NULL;
	fp = fopen(APP_POWER_SAVING, "r");
	char buffer[BUFFER_SIZE];
	printf("%-40s\t", "name");
	printf("%-10s", "optimize");
	printf("%-10s", "alarm");
	printf("%-10s", "wakelock");
	printf("%-10s", "network");
	printf("%-15s", "autolaunch");
	printf("%-20s", "secondarylaunch");
	printf("%-20s", "lockscreencleanup");
	printf("%-20s\n", "consumertype");
	
	while(fgets(buffer, BUFFER_SIZE, fp)){
		if(strstr(buffer, "<package")){
			printf("%-40s\t", matchstr("name=", buffer, '"', '"'));
			printf("%-10s", matchstr("optimize=", buffer, '"', '"'));
			printf("%-10s", matchstr("alarm=", buffer, '"', '"'));
			printf("%-10s", matchstr("wakelock=", buffer, '"', '"'));
			printf("%-10s", matchstr("network=", buffer, '"', '"'));
			printf("%-15s", matchstr("autolaunch=", buffer, '"', '"'));
			printf("%-20s", matchstr("secondarylaunch=", buffer, '"', '"'));
			printf("%-20s", matchstr("lockscreencleanup=", buffer, '"', '"'));
			printf("%-20s\n", matchstr("consumertype=", buffer, '"', '"'));
		}
	}
	fclose(fp);
	return 0;
}
