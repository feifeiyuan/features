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

#define DDR_SET_FREQ_PATH          "/sys/class/devfreq/scene-frequency/sprd_governor/scaling_force_ddr_freq"  

#define DDR_AVAILABLE_FREQ_PATH    "/sys/class/devfreq/scene-frequency/sprd_governor/ddrinfo_freq_table"   

#define DDR_CUR_FREQ_PATH          "/sys/class/devfreq/scene-frequency/sprd_governor/ddrinfo_cur_freq"  

#define DDR_AUTO_FREQ_PATH			"/sys/class/devfreq/scene-frequency/sprd_governor/auto_dfs_on_off"