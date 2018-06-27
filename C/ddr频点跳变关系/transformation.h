typedef signed      int    si;  
typedef signed      char   s8;  
typedef unsigned    char   u8;  
typedef unsigned    int    ui;  
typedef unsigned long int  uli;  
typedef unsigned long long ulli;

#define INVALID_ARGV        -1  
#define FAILED_OPEN_FILE    -2  
#define ERROR				-3

#define DEFAULT_TIME		60
#define MAX_AVALLABLE_FREQ  40  
#define BUFFER_SIZE         512  

#define DDR_AVALIBLE_FREQ	"/sys/class/devfreq/scene-frequency/sprd_governor/ddrinfo_freq_table"
#define DDR_CUR_FREQ		"/sys/class/devfreq/scene-frequency/sprd_governor/ddrinfo_cur_freq"
#define DDR_SCANLING_TIMER	"/sys/class/devfreq/scene-frequency/sprd_governor/scaling_timer"

struct transformation{
	ulli   previous;
	ulli   current;
	ulli   count;
};
