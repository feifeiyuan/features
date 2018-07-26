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

#define ERROR					-22
#define BUFFER_SIZE				256
#define INVALID_ARGV        	-1  
#define FAILED_OPEN_FILE    	-2
#define NOT_ROOT				-4

#define DYNAMIC_HOTPLUG				"/sys/devices/system/cpu/cpuhotplug/dynamic_load_disable"
#define THERMAL_ENABLE				"/sys/devices/virtual/thermal/thermal_zone0/thm_enable"
#define THERMAL_MODE				"/sys/devices/virtual/thermal/thermal_zone0/mode"
#define CORE_ONLINE					"/sys/devices/system/cpu/online"
#define CORE_OFFLINE				"/sys/devices/system/cpu/offline"
#define PER_ONLINE					"/sys/devices/system/cpu/cpu"
#define LIT_POLICY_GOVERNOR			"/sys/devices/system/cpu/cpufreq/policy0/scaling_governor"
#define BIG_POLICY_GOVERNOR			"/sys/devices/system/cpu/cpufreq/policy4/scaling_governor"
#define LIT_SET_SPEED				"/sys/devices/system/cpu/cpufreq/policy0/scaling_setspeed"
#define LIT_MAX_FREQ				"/sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq"
#define CPU0_AVAILABLE_FREQ_PATH    "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies"  
#define CPU4_AVAILABLE_FREQ_PATH    "/sys/devices/system/cpu/cpu4/cpufreq/scaling_available_frequencies" 


typedef signed      int    si;  
typedef signed      char   s8;  
typedef unsigned    char   u8;  
typedef unsigned    int    ui;  
typedef unsigned long int  uli;  
typedef unsigned long long ulli;

#define	ORIGIMAL					 		 0
#define	CLOSE_BIG_NO_LIMIT_LIT		 		 1
#define	CLOSE_BIG_LIMIT_LIT_1100	 		 2
#define	CLOSE_BIG_LIMIT_LIT_1000	 		 3
#define	CLOSE_BIG_LIMIT_LIT_884 	 		 4
#define	CLOSE_BIG_LIMIT_LIT_768		 		 5
#define	CLOSE_BIG_LIMIT_LIT_768_CLOSE_3		 6
#define	CLOSE_BIG_LIMIT_LIT_768_CLOSE_2		 7
#define	CLOSE_BIG_LIMIT_LIT_768_CLOSE_1		 8


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

static int close_power_hint()
{
	FILE *fp = NULL;
	fp = popen("setprop persist.sys.power.hint 0", "w");
	if(fp==NULL){
		fprintf(stderr, "Failed execute %s\n", "setprop persist.sys.power.hint 0");
		return ERROR;
	}
	pclose(fp);
	return 0;
}

static long int dumpsys()
{
	long int frame = 0;
	FILE *fp = NULL;
	fp = popen("dumpsys SurfaceFlinger | grep -o -E 'flips=[0-9]*' | sed 's/flips=//g'", "r");
	if(fp==NULL){
		fprintf(stderr, "Failed execute %s\n", "dumpsys SurfaceFlinger | grep flips");
		return ERROR;
	}
	fscanf(fp, "%ld", &frame);
	pclose(fp);
	return frame;
}


static long int cal_fps(long int start_frame, float *ret_fps)
{
	struct timeval start, end;  
    float timeuse = 0.0, fps = 0.0;
	long int end_frame = 0, frame=0;
	  
	gettimeofday(&start, NULL);  
	usleep(890000);
	end_frame = dumpsys();
	frame = end_frame - start_frame;
	gettimeofday(&end, NULL);  
	timeuse = (1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec)/1000000.0; 
	fps = frame/timeuse;
	//printf("time: %.2fs\t frame:%ld\t fps:%.2f\n", timeuse, frame, fps); 
	*ret_fps = fps;
	return end_frame;
}

static int hotplug(char *close_cpu, char *switch_on)
{
	char close_path[BUFFER_SIZE] = PER_ONLINE;
	strcat(close_path, close_cpu);
	strcat(close_path, "/online");
	if(write_node(switch_on, close_path)<0){
		return ERROR;
	}
	return 0;
}

// debug
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
	strcat(str, buffer);
	fclose(fp);
	return str;
}

static int show_core()
{
	char *core_online, *core_offline;
	core_online = read_node(CORE_ONLINE);
	if(core_online==NULL){
		return ERROR;
	}
	if(strlen(core_online)>1){
		printf("\nonline: %s", core_online);
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


static int close_big_no_limit_lit()
{
	// open lit
	hotplug("1", "1");
	hotplug("2", "1");
	hotplug("3", "1");
	hotplug("4", "1");
	
	// default governor()
	char governor_default[] = "schedutil";
	if(write_node(governor_default, LIT_POLICY_GOVERNOR)<0){
		return ERROR;
	}
	
	// close big
	hotplug("4", "0");
	hotplug("5", "0");
	hotplug("6", "0");
	hotplug("7", "0");
	return 0;
}


static int close_big_fix_lit_freq(char *max_freq)
{	
	// userspace governor()
	char governor_userspace[] = "userspace";
	if(write_node(governor_userspace, LIT_POLICY_GOVERNOR)<0){
		return ERROR;
	}
	// max_freq
	if(write_node(max_freq, LIT_MAX_FREQ)<0){
		return ERROR;
	}
	return 0;
}

static int set_original()
{
		// open lit
	hotplug("1", "1");
	hotplug("2", "1");
	hotplug("3", "1");
	hotplug("4", "1");
	
	// default governor()
	char governor_default[] = "schedutil";
	if(write_node(governor_default, LIT_POLICY_GOVERNOR)<0){
		return ERROR;
	}
	if(write_node(governor_default, BIG_POLICY_GOVERNOR)<0){
		return ERROR;
	}
	
	// close big
	hotplug("4", "1");
	hotplug("5", "1");
	hotplug("6", "1");
	hotplug("7", "1");
	return 0;
	
}

static int hotplug_low_freq(int *statu)
{
	switch (*statu){
		case ORIGIMAL:
			close_big_no_limit_lit();
			*statu = CLOSE_BIG_NO_LIMIT_LIT;
			break;
		case CLOSE_BIG_NO_LIMIT_LIT:
			close_big_fix_lit_freq("1100000");
			*statu = CLOSE_BIG_LIMIT_LIT_1100;
			break;
		case CLOSE_BIG_LIMIT_LIT_1100:
			close_big_fix_lit_freq("1000000");
			*statu = CLOSE_BIG_LIMIT_LIT_1000;
			break;
		case CLOSE_BIG_LIMIT_LIT_1000:
			close_big_fix_lit_freq("884000");
			*statu = CLOSE_BIG_LIMIT_LIT_884;
			break;
		case CLOSE_BIG_LIMIT_LIT_884:
			close_big_fix_lit_freq("768000");
			*statu = CLOSE_BIG_LIMIT_LIT_768;
			break;
		case CLOSE_BIG_LIMIT_LIT_768:
			hotplug("3", "0");
			*statu = CLOSE_BIG_LIMIT_LIT_768_CLOSE_3;
			break;
		case CLOSE_BIG_LIMIT_LIT_768_CLOSE_3:
			hotplug("2", "0");
			*statu = CLOSE_BIG_LIMIT_LIT_768_CLOSE_2;
			break;
		case CLOSE_BIG_LIMIT_LIT_768_CLOSE_2:
			hotplug("1", "0");
			*statu = CLOSE_BIG_LIMIT_LIT_768_CLOSE_1;
			break;
	}
}

static int unhotplug_high_freq(int *statu)
{
	switch (*statu){
		case CLOSE_BIG_LIMIT_LIT_768_CLOSE_1:
			hotplug("1", "1");
			*statu = CLOSE_BIG_LIMIT_LIT_768_CLOSE_2;
			break;
		case CLOSE_BIG_LIMIT_LIT_768_CLOSE_2:
			hotplug("2", "1");
			*statu = CLOSE_BIG_LIMIT_LIT_768_CLOSE_3;
			break;
		case CLOSE_BIG_LIMIT_LIT_768_CLOSE_3:
			hotplug("3", "1");
			*statu = CLOSE_BIG_LIMIT_LIT_768;
			break;
		case CLOSE_BIG_LIMIT_LIT_768:
			close_big_fix_lit_freq("884000");
			*statu = CLOSE_BIG_LIMIT_LIT_884;
			break;
		case CLOSE_BIG_LIMIT_LIT_884:
			close_big_fix_lit_freq("1000000");
			*statu = CLOSE_BIG_LIMIT_LIT_1000;
			break;
		case CLOSE_BIG_LIMIT_LIT_1000:
			close_big_fix_lit_freq("1100000");
			*statu = CLOSE_BIG_LIMIT_LIT_1100;
			break;
		case CLOSE_BIG_LIMIT_LIT_1100:
			close_big_no_limit_lit();
			*statu = CLOSE_BIG_NO_LIMIT_LIT;
			break;
		case CLOSE_BIG_NO_LIMIT_LIT:
			set_original();
			*statu = ORIGIMAL;
			break;
	}
}



int main()
{
	long int start_frame = 0;
	float fps = 0.0;
	float stander = 0;
	float interval = 2;
	int flag = 0;
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	int statu = 0;
	// close dynamic hotplug
	if(close_dynamic_hotplug()<0){
		return ERROR;
	}
	
	// close thermal IPA
	if(close_thermal_IPA()<0){
		return ERROR;
	}
	
	// close power hint
	if(close_power_hint()<0){
		return ERROR;
	}
	
	// define orignal
	if(set_original()<0){
		return ERROR;
	}
	
	#if 1
	statu = ORIGIMAL;
	while(1){
		start_frame = dumpsys();  
		if(0==start_frame){
			continue;
		}
		start_frame = cal_fps(start_frame, &fps);
		if(0.0!=(fps-0)){
			if(flag==0){
				stander = fps;
				flag = 1;
			}
			
			// 升频插核
			if ((fps-stander)<(-1*interval)){
				unhotplug_high_freq(&statu);
			// 拔核降频(满足功耗)
			}else{
				hotplug_low_freq(&statu);
			}
			printf("%.2f, statu is %d\n", fps, statu);
		}
	}
	#endif
}
