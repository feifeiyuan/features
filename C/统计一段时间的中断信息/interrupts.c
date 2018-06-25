#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>  
#include <unistd.h>  
#include <sys/time.h>  
#include<unistd.h>

#include "interrupts.h"

static si get_argv(char *time_char)  
{  
    char *ch = time_char;  
    si ret = 0;  
      
    if( *ch == '0'){  
        fprintf(stderr, "interval should not be zero\n");  
        return INVALID_ARGV;  
    }  
  
    for(ch=time_char; *ch; ch++){  
        if( *ch == '-' || !(*ch >= '0' || *ch <= '9')){  
            fprintf(stderr, "please input the correct argv time(no negative)\n");  
            return INVALID_ARGV;  
        }  
        ret = ret*10+(*ch-'0');  
    }  
    return ret;  
}  

static si check_argv(char *tag, si *accummulate_time, char *value)
{
	si argv_time = *accummulate_time;
	//printf("%s\n",tag);
	//printf("%d\n",strcmp(tag,"-i"));
	if(strcmp(tag,"-t")==0){
		argv_time = get_argv(value); 
		if(argv_time<0)
			return INVALID_ARGV;    			 
	}else{
		fprintf(stderr, "you must input correct argvs\n");   
		fprintf(stderr, "expl: ./data/local/tmp/interrupts -t 10(s)\n");  
		return INVALID_ARGV; 
	}
	if(argv_time==0)
		argv_time = DEFAULT_TIME;
	*accummulate_time = argv_time;
	return 0;
}

static si store_IPI_interrupts(u8 cpu_count, char *buffer, char (*per_cpu)[BUFFER_SIZE], struct IPI_consumer *ipi_head)
{
	char *delim = " ", devname[BUFFER_SIZE];
	struct stru_per_cpu_match interrupt_IPI[cpu_count-1];
	char *point = strtok(buffer, delim), id[SIZE];
	struct IPI_consumer *IPI = ipi_head;
	u8 order = 0, i = 0;
	memset(devname, 0, BUFFER_SIZE);
	while(point!=NULL){
		if(order==0){
			strcpy(id, point);
		}else if(order>=cpu_count+1){
			strcat(devname, point);
			strcat(devname, " ");
		}else{
			strcpy(interrupt_IPI[order-1].name, per_cpu[order-1]);
			interrupt_IPI[order-1].num = atol(point);
			interrupt_IPI[order-1].match = 0;
		}
		point = strtok(NULL, delim);
		order++;
	}
	
	while(IPI!=NULL){
			if(IPI->Next==NULL){
				struct IPI_consumer *interrupts = NULL;
				interrupts = (struct IPI_consumer *)malloc(sizeof(*interrupts));
				if(interrupts==NULL){
					fprintf(stderr, "%s:failed malloc space\n",__func__);
					return ERROR;
				}
				IPI->Next = interrupts;
				strcpy(interrupts->consumer.id, id);
				strcpy(interrupts->consumer.dev_name, devname);
				interrupts->consumer.total = 0;
				for(i=0;i<cpu_count; i++){
					strcpy(interrupts->consumer.per_cpu[i].name, interrupt_IPI[i].name);
					interrupts->consumer.per_cpu[i].num = interrupt_IPI[i].num;
					interrupts->consumer.per_cpu[i].match = interrupt_IPI[i].match;
				}
				interrupts->Next = NULL;
				IPI = IPI->Next;
				break;
			}else{
				IPI = IPI->Next;
			}
	}
	return 0;
}


static s8 store_external_interrupts(u8 cpu_count, char *buffer, char (*per_cpu)[BUFFER_SIZE], struct external_consumer *external_head)
{
	char *delim = " ", devname[BUFFER_SIZE];
	struct stru_per_cpu_match interrupts_external[cpu_count-1];
	struct external_consumer *external = external_head;
	struct stru_per_external per_external;
	char *point = strtok(buffer, delim);
	u8 order = 0, i = 0;
	memset(devname, 0, BUFFER_SIZE);
	while(point!=NULL){
		if(order==0){
			strcpy(per_external.id, point);
		}else if(order==cpu_count+1){
			strcpy(per_external.request_irq, point);
		}else if(order==cpu_count+2){
			strcpy(per_external.request_unkown, point);
		}else if(order==cpu_count+3){
			strcpy(per_external.acpi_sci, point);
		}else if(order>=cpu_count+4){
			strcat(devname, point);
			strcat(devname, " ");
		}else{
			strcpy(interrupts_external[order-1].name, per_cpu[order-1]);
			interrupts_external[order-1].num = atol(point);
			interrupts_external[order-1].match = 0;
		}
		point = strtok(NULL, delim);
		order++;
	}
	strcpy(per_external.dev_name, devname);
	while(external!=NULL){
		if(external->Next==NULL){
			struct external_consumer *interrupts = NULL;
			interrupts = (struct external_consumer *)malloc(sizeof(*interrupts));
			if(interrupts==NULL){
				fprintf(stderr, "%s:failed malloc space\n",__func__);
				return ERROR;
			}
			external->Next = interrupts;
			interrupts->consumer.desc = per_external;
			interrupts->consumer.total = 0;
			for(i=0;i<cpu_count; i++){
				strcpy(interrupts->consumer.per_cpu[i].name, interrupts_external[i].name);
				interrupts->consumer.per_cpu[i].num = interrupts_external[i].num;
				interrupts->consumer.per_cpu[i].match = interrupts_external[i].match;
			}
			interrupts->Next = NULL;
			external = external->Next;
			break;
		}else{
			external = external->Next;
		}
	}
}

static void print_IPI_debug(struct IPI_consumer *IPI, u8 cpu_count)
{
	u8 i = 0;
	while(IPI->Next){
		if(IPI->Next->consumer.total!=0){
			printf("%5s\t", IPI->Next->consumer.id);
			for(i=0;i<cpu_count; i++){
				printf("%5ld\t", IPI->Next->consumer.per_cpu[i].num);
			}
			printf("%5lld\t%-15s", IPI->Next->consumer.total, IPI->Next->consumer.dev_name);
		}
		IPI = IPI->Next;
	}
}

static void print_external_debug(struct external_consumer *external, u8 cpu_count)
{
	u8 i = 0;
	if(external->Next!=NULL){
		printf("%5s\t", " ");
		for(i=0;i<cpu_count; i++){
			printf("%5s\t", external->Next->consumer.per_cpu[i].name);
		}
		printf("\n");
	}
	while(external->Next){
		if(external->Next->consumer.total!=0){
			printf("%5s\t", external->Next->consumer.desc.id);
			for(i=0;i<cpu_count; i++){
				printf("%5ld\t", external->Next->consumer.per_cpu[i].num);
			}
			printf("%5lld\t%-15s\t%-5s\t%-10s\t%s", external->Next->consumer.total, external->Next->consumer.desc.request_irq, external->Next->consumer.desc.request_unkown,\
			external->Next->consumer.desc.acpi_sci,  external->Next->consumer.desc.dev_name);
		}
		external = external->Next;
	}
	return;
}

static si get_original_interrupts(struct interrupts *combine)
{
	FILE *fp_interrupt = NULL;
	char buffer[BUFFER_SIZE], *delim = " " , per_cpu[SIZE][BUFFER_SIZE];
	u8 cpu_count = 0, i = 0;
	struct external_consumer *external_head, *interrupts;
	struct IPI_consumer *IPI_head, *IPI;
	
	memset(buffer, 0, BUFFER_SIZE);
	fp_interrupt = fopen(INTERRUPTS_PATH, "r");
	if(fp_interrupt==NULL){
		fprintf(stderr, "failed open the file", INTERRUPTS_PATH);
		return FAILED_OPEN_FILE;
	}
	fgets(buffer, BUFFER_SIZE, fp_interrupt);
	// debug
	//printf("%s", buffer);
	char *point = strtok(buffer, delim);
	while(point!=NULL){
		if(strstr(point,"CPU")){
			strcpy(per_cpu[cpu_count], point);
			cpu_count++;
		}
		point = strtok(NULL, delim);
	}
	// creat_link_head
	external_head = (struct external_consumer *)malloc(sizeof(*external_head));
	if(external_head==NULL){
		fprintf(stderr, "%s:failed malloc space\n",__func__);
		return ERROR;
	}
	external_head->Next = NULL;
	interrupts = external_head;
	
	// creat_link_head
	IPI_head = (struct IPI_consumer *)malloc(sizeof(*IPI_head));
	if(IPI_head==NULL){
		fprintf(stderr, "%s:failed malloc space\n",__func__);
		return ERROR;
	}
	IPI_head->Next = NULL;
	IPI = IPI_head;
	
	while(fgets(buffer, BUFFER_SIZE, fp_interrupt)){
		//printf("%s", buffer);
		if(buffer[0]=='E'&&buffer[1]=='r'&&buffer[2]=='r'&&buffer[3]==':'){
			goto end;
		// IPI中断
		}else if(buffer[0]=='I'&&buffer[1]=='P'&&buffer[2]=='I'){
			// debug
			//printf("%s", buffer);
			store_IPI_interrupts(cpu_count, buffer, per_cpu, IPI_head);
		}else{
		// 外部中断
			// debug
			//printf("%s", buffer);
			store_external_interrupts(cpu_count, buffer, per_cpu, external_head);
		}
	}
end:
	combine->ipi = IPI_head;
	combine->external = external_head;
	combine->cpu_count = cpu_count;
	fclose(fp_interrupt);
	return 0;
}

static si match_IPI(struct IPI_consumer *IPI_start, struct IPI_consumer *IPI_end)
{
	if(strcmp(IPI_start->consumer.id, IPI_end->consumer.id)!=0){
		return ERROR;
	}
	if(strcmp(IPI_start->consumer.dev_name, IPI_end->consumer.dev_name)!=0){
		return ERROR;
	}
	return 0;
}
static si deal_IPI(struct interrupts *combine_start, struct interrupts *combine_end)
{
	struct IPI_consumer *IPI_start = combine_start->ipi->Next;
	struct IPI_consumer *IPI_end = combine_end->ipi->Next;
	u8 start_cpu_count = combine_start->cpu_count, i = 0;
	u8 end_cpu_count = combine_end->cpu_count, j=0;
	
	// debug
	//printf("start_cpu_count is %d\n",  start_cpu_count);
	//printf("end_cpu_count is %d\n",  end_cpu_count);
	
	while(IPI_start){
		IPI_end = combine_end->ipi->Next;
		while(IPI_end){
			if(match_IPI(IPI_start, IPI_end)==0){
				for(i=0; i<start_cpu_count; i++){
					for(j=0;j<end_cpu_count; j++){
						if(strcmp(IPI_start->consumer.per_cpu[i].name, IPI_end->consumer.per_cpu[j].name)==0){
							IPI_start->consumer.per_cpu[i].num = IPI_end->consumer.per_cpu[j].num - IPI_start->consumer.per_cpu[i].num;
							IPI_start->consumer.per_cpu[i].match = 1;
							IPI_end->consumer.per_cpu[j].match = 1;
						}
					}
					IPI_start->consumer.total += IPI_start->consumer.per_cpu[i].num;
				}
				j = 0;
				for(i=0; i<end_cpu_count; i++){
					if(IPI_end->consumer.per_cpu[i].match == 0){
						strcpy(IPI_start->consumer.per_cpu[j+start_cpu_count].name,IPI_end->consumer.per_cpu[i].name);
						IPI_start->consumer.per_cpu[j+start_cpu_count].num = IPI_end->consumer.per_cpu[i].num;
						IPI_start->consumer.per_cpu[j+start_cpu_count].match = 1;
						IPI_start->consumer.total += IPI_end->consumer.per_cpu[i].num;
						j++;
					}
				}
				break;
			}	
			IPI_end = IPI_end->Next;
		}
		IPI_start = IPI_start->Next;
	}
	start_cpu_count = start_cpu_count+j;
	print_IPI_debug(combine_start->ipi, start_cpu_count);
	return 0;
}


static si match_external(struct external_consumer *external_start, struct external_consumer *external_end)
{
	if(strcmp(external_start->consumer.desc.id, external_end->consumer.desc.id)!=0){
		return ERROR;
	}
	if(strcmp(external_start->consumer.desc.dev_name, external_end->consumer.desc.dev_name)!=0){
		return ERROR;
	}
	if(strcmp(external_start->consumer.desc.request_irq, external_end->consumer.desc.request_irq)!=0){
		return ERROR;
	}
	if(strcmp(external_start->consumer.desc.request_unkown, external_end->consumer.desc.request_unkown)!=0){
		return ERROR;
	}
	if(strcmp(external_start->consumer.desc.acpi_sci, external_end->consumer.desc.acpi_sci)!=0){
		return ERROR;
	}
	return 0;
}

static si deal_external(struct interrupts *combine_start, struct interrupts *combine_end)
{
	struct external_consumer *external_start = combine_start->external->Next;
	struct external_consumer *external_end = combine_end->external->Next;
	u8 start_cpu_count = combine_start->cpu_count, i = 0;
	u8 end_cpu_count = combine_end->cpu_count, j = 0;
	
	// debug
	//printf("start_cpu_count is %d\n",  start_cpu_count);
	//printf("end_cpu_count is %d\n",  end_cpu_count);
	
	while(external_start){
		external_end = combine_end->external->Next;
		while(external_end){
			if(match_external(external_start, external_end)==0){
				for(i=0; i<start_cpu_count; i++){
					for(j=0;j<end_cpu_count; j++){
						if(strcmp(external_start->consumer.per_cpu[i].name, external_end->consumer.per_cpu[j].name)==0){
							external_start->consumer.per_cpu[i].num = external_end->consumer.per_cpu[j].num - external_start->consumer.per_cpu[i].num;
							external_start->consumer.per_cpu[i].match = 1;
							external_end->consumer.per_cpu[j].match = 1;
						}
					}
					external_start->consumer.total += external_start->consumer.per_cpu[i].num;
				}
				j = 0;
				for(i=0; i<end_cpu_count; i++){
					if(external_end->consumer.per_cpu[i].match == 0){
						strcpy(external_start->consumer.per_cpu[j+start_cpu_count].name,external_end->consumer.per_cpu[i].name);
						external_start->consumer.per_cpu[j+start_cpu_count].num = external_end->consumer.per_cpu[i].num;
						external_start->consumer.total += external_end->consumer.per_cpu[i].num;
						external_start->consumer.per_cpu[j+start_cpu_count].match = 1;
						j++;
					}
				}
				break;
			}
			external_end = external_end->Next;
		}
		external_start = external_start->Next;
	}
	start_cpu_count = start_cpu_count+j;
	print_external_debug(combine_start->external, start_cpu_count);
	return 0;
}

static si deal_data(struct interrupts *combine_start, struct interrupts *combine_end)
{
	deal_external(combine_start, combine_end);
	deal_IPI(combine_start, combine_end);
	return 0;
}


int main(int argc, char *argv[])  
{
	si accumulate_time = 0;
	struct interrupts combine_start, combine_end;
	if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        fprintf(stderr, "expl: ./data/local/tmp/interrupts -t 10(s)\n");  
        return INVALID_ARGV;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
		fprintf(stderr, "expl: ./data/local/tmp/interrupts -t 10(s)\n");  
        return INVALID_ARGV;  
	} 
	
	// input argc 1  
    if(argc==1){  
		accumulate_time = DEFAULT_TIME;  				
    // input argc 3  or more 
    }else if(argc==3){
		if(check_argv(argv[1], &accumulate_time, argv[2])<0)
			return INVALID_ARGV;
    }
	printf("accumulate_time is %d\n", accumulate_time);
	
	// get original data
	if(get_original_interrupts(&combine_start)<0){
		return ERROR;
	}
	
	sleep(accumulate_time);
	if(get_original_interrupts(&combine_end)<0){
		return ERROR;
	}
	
	// deal data
	if(deal_data(&combine_start, &combine_end)<0){
		return ERROR;
	}
	
	return 0;
}