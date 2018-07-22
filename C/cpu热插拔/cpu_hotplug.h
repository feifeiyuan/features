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

#define DYNAMIC_HOTPLUG		"/sys/devices/system/cpu/cpuhotplug/dynamic_load_disable"
#define THERMAL_ENABLE		"/sys/devices/virtual/thermal/thermal_zone0/thm_enable"
#define THERMAL_MODE		"/sys/devices/virtual/thermal/thermal_zone0/mode"
#define CORE_ONLINE			"/sys/devices/system/cpu/online"
#define CORE_OFFLINE		"/sys/devices/system/cpu/offline"
#define PER_ONLINE			"/sys/devices/system/cpu/cpu"

