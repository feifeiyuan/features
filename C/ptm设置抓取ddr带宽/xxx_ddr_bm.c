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


typedef signed      int    si;  
typedef signed      char   s8;  
typedef unsigned    char   u8;  
typedef unsigned    int    ui;  
typedef unsigned long int  uli;  
typedef unsigned long long ulli;

#define INVALID_ARGV        -1  
#define FAILED_OPEN_FILE    -2  
#define ERROR				-22
#define NOT_ROOT			-4

#define BUFFER_SIZE			256
#define SIZE				20
#define MAX_AVALLABLE_FREQ  40 
#define LIT_CLUSTER_END		4
#define DEFAULT_TIME		240

#define DDR_MODE			"/sys/class/misc/sprd_ptm/mode"
#define DDR_BANDWIDTH		"/sys/class/misc/sprd_ptm/bandwidth"
#define DDR_LOG				"/mnt/obb/axi_per_log"
#define DDR_LOG_TO_CSV		"/data/power_tools/ddr/ddr_bm/ddr_bm_log"
#define DDR_BM_RESULT		"/data/power_tools/ddr/ddr_bm/sharkl3_ddr_bm.log"

#define THERMAL_ENABLE		"/sys/devices/virtual/thermal/thermal_zone0/thm_enable"
#define THERMAL_MODE		"/sys/devices/virtual/thermal/thermal_zone0/mode"

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
	if(strcmp(tag,"-t")==0){
		argv_time = get_argv(value); 
		if(argv_time<0)
			return INVALID_ARGV;    			 
	}else{
		fprintf(stderr, "you must input correct argvs\n");   
		return INVALID_ARGV; 
	}
	if(argv_time==0)
		argv_time = DEFAULT_TIME;
	*accummulate_time = argv_time;
	return 0;
}

static int write_node(char *value, char *path)
{
	FILE *fp  = NULL;
	fp = fopen(path, "w");
	if(fp==NULL){
		fprintf(stderr, "faile open the file %s\n", path);
		fprintf(stderr, "please reboot the phone\n");
		return ERROR;
	}
	fprintf(fp, "%s", value);
	fclose(fp);
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
	ret = write_node("0", DDR_BANDWIDTH);
	return ret;
}

static si get_ddr_log(si accummulate_time)
{
	if(close_thermal_IPA()<0){
		return ERROR;
	}
	if(set_ddr_mode(accummulate_time)<0){
		return ERROR;
	}
}

static int log_to_csv()
{
	char log_to_csv_sh[] = "/data/power_tools/ddr/ddr_bm/log_to_csv.sh";
	char axi_per_log[] = "/mnt/obb/axi_per_log";
	char log_tool[] = "/data/power_tools/ddr/ddr_bm/ddr_bm_log";
	if(!access(log_to_csv_sh,F_OK)){
		if(!access(axi_per_log,F_OK)){
			if(!access(log_tool,F_OK)){
				system("sh /data/power_tools/ddr/ddr_bm/log_to_csv.sh");
			}else{
				fprintf(stderr, "there is no file %s\n", log_tool);
				return -2;
			}
		}else{
			fprintf(stderr, "there is no file %s\n", axi_per_log);
			return -2;
		}
	}else{
		fprintf(stderr, "there is no file %s\n", log_to_csv_sh);
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[])  
{
	char log_path[] = "/mnt/obb/axi_per_log";
	si   accummulate_time = 0;
	if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        fprintf(stderr, "expl: ./data/power_tools/ddr/ddr_bm/ddr_bm -t 240(s) -d 1\n");  
        return INVALID_ARGV;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
		fprintf(stderr, "expl: ./data/power_tools/ddr/ddr_bm/ddr_bm -t 240(s) -d 1\n");  
        return INVALID_ARGV;  
	} 
	
	// input argc 1  
    if(argc==1){  
        accummulate_time = DEFAULT_TIME;  		
    // input argc 3  
    }else{
		if(check_argv(argv[1], &accummulate_time, argv[2])<0)
			return INVALID_ARGV;
    }
	
	printf("accummulate_time is %d(s)\n", accummulate_time); 
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	
	if(get_ddr_log(accummulate_time)<0){
		return ERROR;
	}
	
	if(log_to_csv()<0){
		return ERROR;
	}
}