typedef signed      int    si;  
typedef signed      char   s8;  
typedef unsigned    char   u8;  
typedef unsigned    int    ui;  
typedef unsigned long int  uli;  
typedef unsigned long long ulli;

#define INVALID_ARGV        		-1  
#define FAILED_OPEN_FILE    		-2  
#define ERROR						-22
#define NOT_ROOT					-4

#define SIZE						215
#define MAX_AVALLABLE_FREQ  		40 
#define LIT_CLUSTER_END				4
#define BUFFER_SIZE					512	
#define MAX_CPU						7
#define MIN_CPU						0
#define DEFAULT_TIME				10
#define DEFAULT_NUM					10	

#define CPU0_SET_FREQ_PATH          "/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed"  
#define CPU4_SET_FREQ_PATH          "/sys/devices/system/cpu/cpu4/cpufreq/scaling_setspeed"  
#define LLT_GOVERNOR				"/sys/devices/system/cpu/cpufreq/policy0/scaling_governor"
#define BIG_GOVERNOR				"/sys/devices/system/cpu/cpufreq/policy4/scaling_governor"
#define CPU0_AVAILABLE_FREQ_PATH    "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies"  
#define CPU4_AVAILABLE_FREQ_PATH    "/sys/devices/system/cpu/cpu4/cpufreq/scaling_available_frequencies" 
#define CPU0_CUR_FREQ_PATH          "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq"  
#define CPU4_CUR_FREQ_PATH          "/sys/devices/system/cpu/cpu4/cpufreq/scaling_cur_freq"
#define DDR_AVAILABLE_FREQ_PATH		"/sys/class/devfreq/scene-frequency/sprd_governor/ddrinfo_freq_table"
#define DDR_CUR_FREQ				"/sys/class/devfreq/scene-frequency/sprd_governor/ddrinfo_cur_freq"
#define DDR_OVERFLOW				"/sys/class/devfreq/scene-frequency/sprd_governor/scaling_overflow"
#define DDR_UNDERFLOW				"/sys/class/devfreq/scene-frequency/sprd_governor/scaling_underflow"
#define THERMAL_ZONE0				"/sys/class/thermal/thermal_zone0"
#define THERMAL_ZONE1				"/sys/class/thermal/thermal_zone1"
#define THERMAL_ZONE3				"/sys/class/thermal/thermal_zone3"
#define  FILE_PATH					"/vendor/etc/power_scene_config.xml"
#define  FILE_RESOURCE				"/vendor/etc/power_resource_file_info.xml"
#define TOP_DIR						"/sdcard/top"
#define TOP_RESULT_PATH				"/sdcard/top/top_result.txt"

struct top_component{
	float   cpu_per;
	char    args[BUFFER_SIZE];
	uli		pid;
	u8		count;
};

struct top_info{
	struct top_component top_menber;
	struct top_info * Next;
};

struct top_heap{
	float   cpu_per;
	char    args[BUFFER_SIZE];
	struct top_heap * Left;
	struct top_heap * Right;
};

typedef struct top_info  * top_node;
typedef struct top_heap  * top_tree;