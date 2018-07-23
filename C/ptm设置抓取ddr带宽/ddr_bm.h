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