#include<stdio.h>
#include<stdlib.h>
#include<error.h>
#include<string.h>
#include "para.h"

static char * matchstr(char *tag, char *str)
{
	char *substr = strstr(str, tag);
	char buffer[SIZE];
	int i =  0, count = 0;
	while(*substr){
		if(*substr=='"'||count==1){
			if(*substr=='"')
				count++;
			else
				buffer[i++] = *substr;
		}
		if(count==2){
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

static int anysis_resource(char *path, char *file, char *value)
{
	FILE  *fp = NULL;
	fp = fopen(FILE_RESOURCE, "r");
	char buffer[BUFFER_SIZE];
	int start_flag = 0;
	int conf_flag = 0;
	while(fgets(buffer, BUFFER_SIZE, fp)){
		if(strstr(buffer, path)&&strstr(buffer, file)){
			start_flag = 1;
			continue;
		}
		if(strstr(buffer, path)){
			start_flag = 0;
			continue;
		}
		if(start_flag){
			if(strstr(buffer, value)){
				conf_flag = 1;
				continue;
			}
			if(strstr(buffer, "</conf>")){
				conf_flag = 0;
				continue;
			}
			if(conf_flag){
				printf("%s%-50s\t", "path: ", matchstr("path=", buffer));
				printf("%s\t%-20s\t", "file: ", matchstr("file=", buffer));			
				printf("%s\t%s\n", "value: ", matchstr("value=", buffer));
			}
		}
	}
	fclose(fp);
	return 0;
}


int get_file_content()
{
	FILE  *fp = NULL;
	fp = fopen(FILE_PATH, "r");
	char buffer[BUFFER_SIZE];
	int start_flag = 0;
	char *path, *file, *value;

	memset(buffer, BUFFER_SIZE, 0);
	if(fp==NULL){
		fprintf(stderr, "%s:failed open file %s\n", __func__, FILE_PATH);
		return ERROR;
	}
	while(fgets(buffer, BUFFER_SIZE, fp)){
		if(strstr(buffer, "<scene name=")){
			//printf("%d\n", strstr(buffer, "<scene name="));
			printf("%s\t%s\n", "scene_name: ",matchstr("name", buffer));
			start_flag = 1;
			continue;
		}
		if(strstr(buffer, "</scene>"))
			start_flag = 0;
		if(start_flag){
			path = matchstr("path=", buffer);
			file = matchstr("file=", buffer);
			value = matchstr("value=", buffer);
			if(strcmp(path, "subsys")!=0){
				printf("%s%-50s\t", "path: ", path);
				printf("%s\t%-20s\t", "file: ", file);			
				printf("%s\t%s\n", "value: ", value);
			}else{
				anysis_resource(path, file, value);
			}
		}
			
	}
	fclose(fp);
	return 0;
}

int main()
{
	get_file_content();
}
