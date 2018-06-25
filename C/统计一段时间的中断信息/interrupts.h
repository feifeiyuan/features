typedef signed      int    si;  
typedef signed      char   s8;  
typedef unsigned    char   u8;  
typedef unsigned    int    ui;  
typedef unsigned long int  uli;  
typedef unsigned long long ulli;

#define INVALID_ARGV        -1  
#define FAILED_OPEN_FILE    -2  
#define ERROR				-3

#define DEFAULT_TIME		10
#define BUFFER_SIZE			512
#define SIZE				256

#define INTERRUPTS_PATH		"/proc/interrupts"


struct stru_per_cpu_match{
	char name[SIZE];
	uli  num;
	u8 match;
};

struct stru_per_external{
	char id[SIZE];
	char request_irq[SIZE];
	char request_unkown[SIZE];
	char acpi_sci[SIZE];
	char dev_name[BUFFER_SIZE];
};

struct interrupt_component_external{
	struct stru_per_external desc;
	struct stru_per_cpu_match per_cpu[SIZE];
	ulli total;
};

struct interrupt_component_IPI{
	char id[SIZE];
	struct stru_per_cpu_match per_cpu[SIZE];
	char dev_name[BUFFER_SIZE];
	ulli total;
};

struct IPI_consumer{
	struct interrupt_component_IPI consumer;
	struct IPI_consumer * Next;
};

struct external_consumer{
	struct interrupt_component_external consumer;
	struct external_consumer * Next;
};

struct interrupts{
	struct external_consumer *external;
	struct IPI_consumer *ipi;
	u8 cpu_count;
};
