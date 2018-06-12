#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>  
#include <sys/time.h>  
  
#include "sgm.h"  
  
static void debug_cpufreq_count(u8 cpu_size,  ui *cpu_available_freq)  
{  
    u8 i = 0;  
    for(i=0;i<cpu_size; i=i+2){  
        printf("%d\t%d\n", cpu_available_freq[i], cpu_available_freq[i+1]);  
    }  
}  
  
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
  
static s8 get_init_freq_count(const char *cpu_cur_path, u8 *size, ui *cpu_available_freq)  
{  
    FILE    *fp = NULL;  
    fp = fopen(cpu_cur_path, "r");  
    u8 i = 0;  
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:", cpu_cur_path);  
        return INVALID_ARGV;  
    }  
    // even freq     
    // odd count  
    for(i=0; i<MAX_AVALLABLE_FREQ; i=i+2){  
        cpu_available_freq[i+1] = 0;  
        if(fscanf(fp,"%d", cpu_available_freq+i)!=1)  
            break;  
    }  
    *size = i;  
    fclose(fp);  
    return 0;  
}  
  
static u8 clear_envirment()  
{   
	if((access(PCPU_PATH, F_OK))!=-1){   
		remove(PCPU_PATH);  
    } 
	return 0;
}  

static ui get_cur_freq(const char *file_path)  
{  
    FILE    *fp = NULL;  
    ui cur_cpu_freq = 0;  
    fp = fopen(file_path, "r");  
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:\n", file_path);  
        return INVALID_ARGV;  
    }  

    fscanf(fp, "%d", &cur_cpu_freq);  
    fclose(fp);  
    return cur_cpu_freq;  
}  
  
static s8 statistic_cpu_stat(const char *stat_path, ui count, 
 uli (*time)[BUFFER_SIZE], si processor_num, u8 jeffies_count)  
{  
    FILE *fp = NULL;  
    char cpu_stat[BUFFER_SIZE];  
    char *cpu_id, data[MAX_AVALLABLE_FREQ];  
    u8 i = 0, j = 0; 
	
    fp = fopen(stat_path, "r"); 
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:\n", stat_path);  
        return INVALID_ARGV;  
    }  
    // aband sum pcpu   
    fgets(cpu_stat, BUFFER_SIZE, fp);  
	
	while(i<processor_num&&fscanf(fp, "%s", data)==1){
		if(data[0]=='c'&&data[1]=='p'&&data[2]=='u'){
			cpu_id = data+3;
			time[i][0] = atoi(cpu_id);
			j = 1;
			while(j<jeffies_count&&fscanf(fp, "%d", &(time[i][j]))){
				//printf("%d\t", time[i][j]);
				j++;
			}
			if(count%2==1){
				if(i<LIT_CLUSTER_END)
					time[i][j] = get_cur_freq(CPU0_CUR_FREQ_PATH);
				else
					time[i][j] = get_cur_freq(CPU4_CUR_FREQ_PATH);
			}else{
				time[i][j] = 1;
			}
			i++;
		}else{
			break;
		}
	}
    fclose(fp);   
    return 0;  
}  
  
static s8 store_pcpu(u8 cpu_id,  float pcpu, uli freq)  
{  
    FILE *fp = NULL;  
    fp = fopen(PCPU_PATH, "a");  
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:\n", PCPU_PATH);  
        return FAILED_OPEN_FILE;  
    }
	fprintf(fp, "%d\t", cpu_id);
    fprintf(fp, "%.f\t",  pcpu*100);  
	fprintf(fp, "%d\n",  freq);  
    fclose(fp);  
    return 0;  
}
  
static s8 data_simpling(si accummulate_time, si interval, ulli total_count, 
si processor_num, u8 jeffies_count, ulli *cluster_little, ulli *cluster_big)  
{     
    struct timeval start, end;  
    float timeuse = 0.0, time_minus[MAX_AVALLABLE_FREQ];  
    ulli   count = 0, lit_count = 0, big_count = 0;
	uli    minus, idle_time;  
	uli start_time[MAX_AVALLABLE_FREQ][BUFFER_SIZE], end_time[MAX_AVALLABLE_FREQ][BUFFER_SIZE];
    u8 i, j;  
	//printf("total_count is %lld\n", total_count);
	
	gettimeofday(&start, NULL);  
    while(count<total_count){  
		if(count%2==0){
			if(statistic_cpu_stat(PROC_CPU_STAT, count, start_time, processor_num, jeffies_count)<0){
				return ERROR;
			}
		}else{
			if(statistic_cpu_stat(PROC_CPU_STAT, count, end_time, processor_num, jeffies_count)<0){
				return ERROR;
			}
			
			for(i=0; i<processor_num; i++){
				
				// debug
				#if 0
				for(j=0; j<(jeffies_count+1); j++){
					printf("%d\t", start_time[i][j]);
				}
				printf("\n");
				for(j=0;j<(jeffies_count+1);j++){
					printf("%d\t", end_time[i][j]);
				}
				printf("\n");
				#endif
				
				if(end_time[i][jeffies_count]!=0&&start_time[i][jeffies_count]==1){
					minus = 0;
					for(j=0; j<jeffies_count; j++){
						if(j!=NICE_SORT){
							minus += end_time[i][j]-start_time[i][j];
							if(j==ILDE_SORT){
								idle_time = end_time[i][j]-start_time[i][j];
								//printf("idle_time is %d\n", idle_time);
							}
						}else{
							continue;
						}
					}
					if(minus!=0)
						time_minus[i] = (minus-idle_time)*1.0/minus;
					else
						time_minus[i] = 0;
					//printf("%f\n", time_minus[i]);
					store_pcpu(i, time_minus[i], end_time[i][j]);
					if(i<LIT_CLUSTER_END){
						lit_count++;
					}else{
						big_count++;
					}
				}
			}
			memset(start_time, 0, sizeof(uli)*MAX_AVALLABLE_FREQ*BUFFER_SIZE);
			memset(end_time, 0, sizeof(uli)*MAX_AVALLABLE_FREQ*BUFFER_SIZE);
		}
        usleep(interval*1000);  
        count++; 
    }   
	gettimeofday(&end, NULL);  
	timeuse = (1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec)/1000.0;  
	printf("time_use is %.2fms\n", timeuse);  
	*cluster_little = lit_count;
	*cluster_big = big_count;
	
	//printf("cluster_little is %lld\n", lit_count);
	return 0;	
}  
  
static long double print_result(const u8 cpu_size,  ui *cpu_available_freq, uli *online, uli *offline, 
const u8 min_core, const u8 max_core, uli  (*pcpu_per)[BUFFER_SIZE*10], ui (*pcpu_per_count)[BUFFER_SIZE*10], ulli cluster_count)
{	
	//printf("cluster_count is %lld\n", cluster_count);
	uli freq, count, percent;
	float lit_avarage[BUFFER_SIZE*10], online_percent, offline_percent;
	long double total = 0.0;
	u8 i = 0, j = 0;
	printf("");
	
	for(j=0; j<cpu_size; j=j+2){  
        if(j==0)  
            printf("   \t");  
        printf("%d\t\t", cpu_available_freq[j]);  
        if(j==cpu_size-2)  
            printf("\n");  
    }
	
	for(j=0; j<cpu_size; j=j+2){  
        if(j==0)  
			printf("\t");  
		printf("%s\t%s\t", "online", "offline");  
        if(j==cpu_size-2)  
            printf("\n");
	}
	
	for(j=0;j<cpu_size;j=j+2){
		if(j==0)
			printf("\t");
		freq = cpu_available_freq[j]/1000;
		online_percent = online[freq]*1.0/cluster_count*100;
		offline_percent = offline[freq]*1.0/cluster_count*100;
		printf("%.2f%\t%.2f%\t", online_percent, offline_percent);
	}
	printf("\n");
	
	for(i=min_core; i<max_core; i++){
		printf("cpu%d\t", i);
		for(j=0;j<cpu_size; j=j+2){
			freq = cpu_available_freq[j]/1000;
			percent = pcpu_per[i][freq];
			count = pcpu_per_count[i][freq];
			if(pcpu_per_count[i][freq]!=0){
				lit_avarage[freq]+=percent/(count*100.0);
				printf("%.2f%\t\t", percent/(count*1.0));
			}else{
				printf("0.00%\t\t");
			}
		}
		printf("\n");
	}
	for(i=0;i<cpu_size; i=i+2){
		freq = cpu_available_freq[i]/1000;
		online_percent = online[freq]*1.0/cluster_count;
		total += (lit_avarage[freq]*cpu_available_freq[i]*online_percent);
	}
	printf("total : %lf\n\n", total);
	return total;
}

static si compute_cpu_pcpu_freq(const u8 cpu0_size,  
ui *cpu0_available_freq,const u8 cpu4_size,  ui *cpu4_available_freq, 
si processor_num, ulli cluster_little, ulli cluster_big)
{
	uli   cpu0_freq_online[BUFFER_SIZE*10],cpu0_freq_offline[BUFFER_SIZE*10];
	uli   cpu4_freq_online[BUFFER_SIZE*10],cpu4_freq_offline[BUFFER_SIZE*10];
	uli   data; 
	uli pcpu_per[MAX_AVALLABLE_FREQ][BUFFER_SIZE*10] = {0}, percent;
	long double ret_total = 0.0;
	ui  pcpu_per_count[MAX_AVALLABLE_FREQ][BUFFER_SIZE*10] = {0};
	FILE *fp = NULL;
	u8 cpu_id = 0, i, j;
	fp = fopen(PCPU_PATH, "r");
	if(fp ==NULL){
		fprintf(stderr, "failed opend file", PCPU_PATH);  
        return FAILED_OPEN_FILE;  
	}
	while(i<3&&fscanf(fp,"%ld", &data)==1){
		switch(i){
			case 0:
				cpu_id = data;
				break;
			case 1:
				percent = data;
				break;
			case 2:
				if(cpu_id<LIT_CLUSTER_END){
					if(percent!=0){
						cpu0_freq_online[data/1000]++;
					}else{
						cpu0_freq_offline[data/1000]++;
					}
				}else{
					if(percent!=0){
						cpu4_freq_online[data/1000]++;
					}else{
						cpu4_freq_offline[data/1000]++;
					}
				}
				pcpu_per[cpu_id][data/1000] += percent;
				pcpu_per_count[cpu_id][data/1000]++;
				break;
		}
		if(i++==2){
			i = 0;
		}
			
	}
	
	ret_total = print_result(cpu0_size, cpu0_available_freq, cpu0_freq_online, cpu0_freq_offline,
	LIT_CLUSTER_START, LIT_CLUSTER_END, pcpu_per, pcpu_per_count, cluster_little);
	if(processor_num > LIT_CLUSTER_END){
		ret_total += print_result(cpu4_size, cpu4_available_freq, cpu4_freq_online, cpu4_freq_offline, 
		LIT_CLUSTER_END, BIG_CLUSTER_END, pcpu_per, pcpu_per_count, cluster_big);
		printf("TOTAL : %lf\n", ret_total);
	}
	return 0;
}

static si check_argv(si *interval, char *tag, si *accummulate_time, char *value)
{
	si argv_interval=*interval, argv_time = *accummulate_time;
	//printf("%s\n",tag);
	//printf("%d\n",strcmp(tag,"-i"));
	if(strcmp(tag,"-i")==0){
		argv_interval = get_argv(value);  
		if(argv_interval<0)
			return INVALID_ARGV;  
	}else if(strcmp(tag,"-t")==0){
		argv_time = get_argv(value); 
		if(argv_time<0)
			return INVALID_ARGV;    			 
	}else{
		fprintf(stderr, "you must input correct argvs\n");   
		fprintf(stderr, "expl: ./data/local/tmp/cpu_sgm -i 100(ms) -t 60(s)\n");  
		return INVALID_ARGV; 
	}
    if(argv_interval==0)
		argv_interval  = DEFAULT_INTERVAL;
	if(argv_time==0)
		argv_time = DEFAULT_TIME;
	*interval = argv_interval;
	*accummulate_time = argv_time;
	return 0;
}

static si get_processor_num()
{
	FILE *file_process = NULL;
	si core_num = 0;
	file_process = popen("cat /proc/cpuinfo| grep \"processor\"| wc -l","r");
	if(file_process==NULL){
		fprintf(stderr, "%s:failed run commond\n", __func__, "/proc/cpuinfo| grep \"processor\"| wc -l");  
        return FAILED_OPEN_FILE; 
	}
	fscanf(file_process, "%d", &core_num);
	fclose(file_process);
	return core_num;
}

static s8 get_init_proc_stat(const char *file_path)
{
	FILE *fp = NULL;  
    char cpu_stat[BUFFER_SIZE], *ch; 
	ui count = 0;
	
    fp = fopen(file_path, "r"); 
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:\n", file_path);  
        return FAILED_OPEN_FILE;  
    }     
    fgets(cpu_stat, BUFFER_SIZE, fp);  
	for(ch=cpu_stat; *ch; ch++){
		if(*ch==' '){
			count++;
		}
	}
	//printf("%d\n", count);
	fclose(fp);
	return count;
}

int main(int argc, char *argv[])  
{  
    si   interval=0 , accummulate_time = 0, processor_num = 0;   
    ulli total_count = 0, cluster_little, cluster_big;  
    u8   cpu0_size, cpu4_size, i , j, jeffies_count;  
    ui   cpu0_available_freq[MAX_AVALLABLE_FREQ], cpu4_available_freq[MAX_AVALLABLE_FREQ];  
  
	// input argc less than 1  
    if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        fprintf(stderr, "expl: ./data/local/tmp/cpu_sgm -i 100(ms) -t 60(s)\n");  
        return INVALID_ARGV;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
		fprintf(stderr, "expl: ./data/local/tmp/cpu_sgm -i 100(ms) -t 60(s)\n");  
        return INVALID_ARGV;  
	}
	
	// input argc 1  
    if(argc==1){  
        interval = DEFAULT_INTERVAL;  
        accummulate_time = DEFAULT_TIME;  		
    // input argc 3  
    }else if(argc==3){
		if(check_argv(&interval, argv[1], &accummulate_time, argv[2])<0)
			return INVALID_ARGV;
    // input argc 3  or more
    }else{
		if(check_argv(&interval, argv[1], &accummulate_time, argv[2])<0)
			return INVALID_ARGV;
		if(check_argv(&interval, argv[3], &accummulate_time, argv[4])<0)
			return INVALID_ARGV;
	}
	
	printf("interval is %d\n", interval);
	if(interval<20){
		fprintf(stderr, "the interval time should not less than 20ms\n");
		return INVALID_ARGV;
	}
	printf("accummulate_time is %d\n", accummulate_time); 
	if(accummulate_time>3600){
		fprintf(stderr, "the accummulate time should not more than 1 hour\n");
		return INVALID_ARGV;
	}
	processor_num = get_processor_num();
	if(processor_num<=0)
		return ERROR;
	//printf("%d\n", processor_num);
	
	if(get_init_freq_count(CPU0_AVAILABLE_FREQ_PATH, &cpu0_size, cpu0_available_freq)<0)
		return ERROR;
	if(processor_num>LIT_CLUSTER_END){
		if(get_init_freq_count(CPU4_AVAILABLE_FREQ_PATH, &cpu4_size, cpu4_available_freq)<0)
			return ERROR;
	} 
	
	jeffies_count = get_init_proc_stat(PROC_CPU_STAT);
	if(jeffies_count<0){
		return ERROR;
	}
      
    // debug  
    // debug_cpufreq_count(cpu0_size, cpu0_available_freq);  
	// debug_cpufreq_count(cpu4_size, cpu4_available_freq);  
    total_count = (accummulate_time*1000)/interval;  
    clear_envirment();  
	
    // data cimpling  
    if(data_simpling(accummulate_time, interval, total_count, processor_num, jeffies_count, &cluster_little, &cluster_big)<0){
		return ERROR;
	}

	if(compute_cpu_pcpu_freq(cpu0_size, cpu0_available_freq, cpu4_size,cpu4_available_freq, processor_num,  cluster_little, cluster_big)<0){
		return ERROR;
	}
  
    return 0;  
}  