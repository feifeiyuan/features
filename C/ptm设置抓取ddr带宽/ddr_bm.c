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

#include "ddr_bm.h"

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

static si close_thermal_IPA()
{
	int ret = 0;
	char thermal_enable[] = "0";
	char thermal_mode[] = "disabled";
	ret = write_node(thermal_enable, THERMAL_ENABLE);
	ret = write_node(thermal_mode, THERMAL_MODE);
	return ret;
}

static si set_ddr_mode(int test_time)
{
	char mode_initial[] = "initial";
	char mode_legacy[] = "legacy";
	char ddr_bm[] = "10";
	si ret = 0;
	ret = write_node(mode_initial, DDR_MODE);
	ret = write_node(mode_legacy, DDR_MODE);
	sleep (2);
	if(!access(DDR_BANDWIDTH,F_OK)){
		ret = write_node(ddr_bm, DDR_BANDWIDTH);
	}
	sleep(test_time);
	return ret;
}

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

static si LogToCSV()
{
	si ret = 0;
	char displag_all[] = "1";
	if(!access(DDR_LOG,F_OK)){
		if(!access(DDR_LOG_TO_CSV,F_OK)){
			char command[BUFFER_SIZE] = ".";
			strcat(command, DDR_LOG_TO_CSV);
			strcat(command, " ");
			strcat(command, DDR_LOG);
			strcat(command, " ");
			strcat(command, displag_all);
			strcat(command, " > ");
			strcat(command, DDR_BM_RESULT);
			printf("%s\n", command);
			ret = system(command);
		}else{
			fprintf(stderr, "there is no log tool %s\n", DDR_LOG_TO_CSV);
			return ERROR;
		}
	}else{
		fprintf(stderr, "there is no log %s\n", DDR_LOG);
		return ERROR;
	}
	return ret;
}

int main(int argc, char *argv[])  
{
	si   accummulate_time = 0;
	if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        fprintf(stderr, "expl: ./data/power_tools/ddr/ddr_bm/sharkl3_ddr_bm -t 240(s)\n");  
        return INVALID_ARGV;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
		fprintf(stderr, "expl: ./data/power_tools/ddr/ddr_bm/sharkl3_ddr_bm -t 240(s)\n\n");  
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
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	if(close_thermal_IPA()<0){
		return ERROR;
	}
	if(set_ddr_mode(accummulate_time)<0){
		return ERROR;
	}
	printf("displag_all data\n");
	printf("./data/power_tools/ddr/ddr_bm/ddr_bm_log /mnt/obb/axi_per_log 1 > /data/power_tools/ddr/ddr_bm/sharkl3_ddr_bm.log\n");
	printf("only displag wb and rb\n");
	printf("./data/power_tools/ddr/ddr_bm/ddr_bm_log /mnt/obb/axi_per_log 0 > /data/power_tools/ddr/ddr_bm/sharkl3_ddr_bm.log\n");
	return 0;
}