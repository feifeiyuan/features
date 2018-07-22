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

#include "fix_ddr_freq.h"


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

static s8 get_init_freq_count(const char *ddr_cur_path, u8 *size, ui *ddr_available_freq)  
{  
    FILE    *fp = NULL;  
    fp = fopen(ddr_cur_path, "r");  
    u8 i = 0;  
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:", ddr_cur_path);  
        return INVALID_ARGV;  
    }  
    // even freq     
    // odd count  
    for(i=0; i<MAX_AVALLABLE_FREQ; i++){   
        if(fscanf(fp,"%d", ddr_available_freq+i)!=1)  
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

static int fix_freq(char *desc, int  cpu_size, ui *ddr_available_freq, char *path)
{
	int i = 0;
	printf("%s\n", desc);
	for(i=0; i<cpu_size; i++){
		printf("%d\t", ddr_available_freq[i]);
	}
	printf("\n");
	char select_freq[SIZE];
	int selected_freq = 0;
	int match_flag = 0;
	printf("input fix freq\n");
	scanf("%s", select_freq);
	selected_freq = CharToInt(select_freq);
	for(i=0;i<cpu_size; i++){
		if(ddr_available_freq[i]==selected_freq)
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

static int close_auto_dfs()
{
	char disable[] = "0";
	int ret = 0;
	ret = write_node(disable, DDR_AUTO_FREQ_PATH);
	return ret;
}

int main(int argc, char *argv[])  
{
	u8   ddr_size;
	ui   ddr_available_freq[MAX_AVALLABLE_FREQ];  
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	if(close_auto_dfs()<0){
		return ERROR;
	}
	if(get_init_freq_count(DDR_AVAILABLE_FREQ_PATH, &ddr_size, ddr_available_freq)<0)
		return ERROR;

	if(fix_freq("ddr avalible freq:", ddr_size, ddr_available_freq, DDR_SET_FREQ_PATH)<0){
		return ERROR;
	}
	if(read_node(DDR_CUR_FREQ_PATH)==NULL){
		return ERROR;
	}
	return 0;
}