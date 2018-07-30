#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>  
#include <unistd.h>  
#include <sys/time.h>  
#include<unistd.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <dirent.h>

#include "ddr_loading.h"

static int write_node(char *value, char *path)
{
	FILE *fp  = NULL;
	fp = fopen(path, "w");
	if(fp==NULL){
		fprintf(stderr, "faile open the file %s\n", path);
		return ERROR;
	}
	fprintf(fp, "%s", value);
	fclose(fp);
	return 0;
}

static int CharToInt(char *str)
{
	int freq = 0;
	char *ptr = str;
	while(*ptr){
		freq = freq*10+(*ptr-'0');
		ptr++;
	}
	return freq;
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
		fprintf(stderr, "expl: ./data/local/tmp/cpu_sgm -i 100(ms) -t 60(s)\n");  
		return INVALID_ARGV; 
	}
	if(argv_time==0)
		argv_time = DEFAULT_TIME;
	*accummulate_time = argv_time;
	return 0;
}

static s8 get_init_freq_count(const char *ddr_cur_path, u8 *size, ui *ddr_available_freq)  
{  
    FILE    *fp = NULL;  
    fp = fopen(ddr_cur_path, "r");  
    u8 i = 0;  
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:", ddr_cur_path);  
        return INVALID_ARGV;  
    }  
    // even freq     
    // odd count  
    for(i=0; i<MAX_AVALLABLE_FREQ; i=i+2){
		ddr_available_freq[i+1] = 0;
        if(fscanf(fp,"%d", ddr_available_freq+i)!=1)  
            break;  
    }  
    *size = i;  
    fclose(fp);  
    return 0;  
} 

static int read_node(char *path)
{
	FILE *fp  = NULL;
	int freq = 0;
	fp = fopen(path, "r");
	if(fp==NULL){
		fprintf(stderr, "faile open the file %s\n", path);
		return FAILED_OPEN_FILE;
	}
	fscanf(fp, "%d", &freq);
	fclose(fp);
	return freq;
}

static char * read_node_str(char *path)
{
	FILE *fp  = NULL;
	char buffer[BUFFER_SIZE], *str;
	fp = fopen(path, "r");
	if(fp==NULL){
		fprintf(stderr, "faile open the file %s\n", path);
		return NULL;
	}
	fgets(buffer, BUFFER_SIZE, fp);
	str = (char *)malloc(sizeof(buffer));
	if(str==NULL){
		fprintf(stderr, "%s there is no space\n", __func__);
		return NULL;
	}
	memset(str, 0, sizeof(buffer));
	strcat(str,buffer);
	fclose(fp);
	return str;
}

static int data_simpling(si accummulate_time, u8 ddr_size, ui *ddr_available_freq)
{
	struct timeval start, end;  
    float timeuse = 0.0;
	uli total_count = 0, count = 0;
	int freq = 0, i = 0;
	int interval = 2;
	total_count = accummulate_time*1000/interval;
	gettimeofday(&start, NULL);  
	while(count<total_count){
		freq = read_node(DDR_CUR_FREQ);
		if(freq<0){
			return ERROR;
		}
		for(i=0; i<ddr_size; i=i+2){
			if(freq == ddr_available_freq[i]){
				ddr_available_freq[i+1]++;
				break;
			}
		}
		count++;
		usleep((interval-1)*1000);
	}
	gettimeofday(&end, NULL);  
	timeuse = (1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec)/1000.0;  
	printf("time_use is %.2fms\n", timeuse); 
	
	for(i=0; i<ddr_size; i=i+2){
		printf("%d: %20d\t\t%.2f%\n", ddr_available_freq[i], ddr_available_freq[i+1], ddr_available_freq[i+1]*1.0*100/total_count);
	}
	return 0;
}

static void print_other_info(u8 ddr_size, ui *ddr_available_freq)
{
	printf("%s\n", "unit:10ms");
	int i = 0;
	printf("theory_bw = ");
	for(i=0; i<ddr_size; i=i+2){
		printf("%d\t", ddr_available_freq[i]*8000);
	}
	printf("\n");
	printf("overflow  = %s", read_node_str(DDR_OVERFLOW));
	printf("underflow = %s", read_node_str(DDR_UNDERFLOW));
}

int main(int argc, char *argv[])  
{
	u8   ddr_size;
	ui   ddr_available_freq[MAX_AVALLABLE_FREQ];  
	si   accummulate_time = 0;
	if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        fprintf(stderr, "expl: ./xxxx -t 240(s)\n");  
        return INVALID_ARGV;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
		fprintf(stderr, "expl: ./xxxx -t 240(s)\n\n");  
        return INVALID_ARGV;  
	} 
	
	// input argc 1  
    if(argc==1){  
        accummulate_time = DEFAULT_TIME;  		
    // input argc 3  or more
    }else if(argc==3){
		if(check_argv(argv[1], &accummulate_time, argv[2])<0)
			return INVALID_ARGV;
    }
	printf("accummulate_time is %d(s)\n", accummulate_time); 
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	
	if(get_init_freq_count(DDR_AVAILABLE_FREQ_PATH, &ddr_size, ddr_available_freq)<0)
		return ERROR;
	
	if(data_simpling(accummulate_time, ddr_size, ddr_available_freq)){
		return ERROR;
	}
	print_other_info(ddr_size, ddr_available_freq);
	
	return 0;
}