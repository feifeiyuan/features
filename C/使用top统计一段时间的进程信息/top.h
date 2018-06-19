typedef signed      int    si;  
typedef signed      char   s8;  
typedef unsigned    char   u8;  
typedef unsigned    int    ui;  
typedef unsigned long int  uli;  
typedef unsigned long long ulli;
typedef struct top_info  * top_node;
typedef struct top_heap  * top_tree;

#define BUFFER_SIZE         512

#define INVALID_ARGV		-1
#define FAILED_OPEN_FILE    -2  
#define ERROR				-22

#define DEFAULT_TIME		10
#define DEFAULT_NUM			10

#define TOP_DIR				"/sdcard/top"
#define TOP_RESULT_PATH		"/sdcard/top/top_result.txt"

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
	struct top_component top_menber;
	struct top_heap * Left;
	struct top_heap * Right;
};

