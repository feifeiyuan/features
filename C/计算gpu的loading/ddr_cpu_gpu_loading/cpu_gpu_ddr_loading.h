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
#define DEFAULT_CPU_TIME	60
#define DEFAULT_GPU_TIME	60
#define DEFAULT_DDR_TIME	60

#define CPU0_AVAILABLE_FREQ_PATH    "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies"  
#define CPU4_AVAILABLE_FREQ_PATH    "/sys/devices/system/cpu/cpu4/cpufreq/scaling_available_frequencies" 

#define POLICY0_TIME_STATS			"/sys/devices/system/cpu/cpufreq/policy0/stats/time_in_state"
#define POLICY4_TIME_STATS			"/sys/devices/system/cpu/cpufreq/policy4/stats/time_in_state"

#define DDR_AVAILABLE_FREQ_PATH		"/sys/class/devfreq/scene-frequency/sprd_governor/ddrinfo_freq_table"
#define DDR_CUR_FREQ				"/sys/class/devfreq/scene-frequency/sprd_governor/ddrinfo_cur_freq"
#define DDR_OVERFLOW				"/sys/class/devfreq/scene-frequency/sprd_governor/scaling_overflow"
#define DDR_UNDERFLOW				"/sys/class/devfreq/scene-frequency/sprd_governor/scaling_underflow"


#define CPU0_SET_FREQ_PATH          "/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed"  
#define CPU4_SET_FREQ_PATH          "/sys/devices/system/cpu/cpu4/cpufreq/scaling_setspeed"  

#define LLT_GOVERNOR				"/sys/devices/system/cpu/cpufreq/policy0/scaling_governor"
#define BIG_GOVERNOR				"/sys/devices/system/cpu/cpufreq/policy4/scaling_governor"


#define CPU0_CUR_FREQ_PATH          "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq"  
#define CPU4_CUR_FREQ_PATH          "/sys/devices/system/cpu/cpu4/cpufreq/scaling_cur_freq"

#define MAX_CPU						7
#define MIN_CPU						0