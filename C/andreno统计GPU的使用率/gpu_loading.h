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
#define BUFFER_SIZE			256

#define GPU_AVAILABLE_FREQ_PATH    "/sys/class/kgsl/kgsl-3d0/gpu_available_frequencies"  
#define GPU_POLLING_INTERVAL	   "/sys/class/kgsl/kgsl-3d0/devfreq/polling_interval"
#define GPU_CUR_FREQ			   "/sys/class/kgsl/kgsl-3d0/gpuclk"
#define GPU_CUR_UTILISATION		   "/sys/class/kgsl/kgsl-3d0/gpu_busy_percentage"
