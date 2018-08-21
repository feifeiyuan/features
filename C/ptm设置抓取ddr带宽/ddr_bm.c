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

#include "ddr_bm.h"

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

static si close_thermal_IPA()
{
	int ret = 0;
	char thermal_enable[] = "0";
	char thermal_mode[] = "disabled";
	ret = write_node(thermal_enable, THERMAL_ENABLE);
	ret = write_node(thermal_mode, THERMAL_MODE);
	return ret;
}

static si set_ddr_mode(int test_time)
{
	char mode_initial[] = "initial";
	char mode_legacy[] = "legacy";
	char ddr_bm[] = "10";
	si ret = 0;
	ret = write_node(mode_initial, DDR_MODE);
	ret = write_node(mode_legacy, DDR_MODE);
	sleep (2);
	if(!access(DDR_BANDWIDTH,F_OK)){
		ret = write_node(ddr_bm, DDR_BANDWIDTH);
	}
	sleep(test_time);
	return ret;
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

static si check_argv(si *dis_all, char *tag, si *accummulate_time, char *value)
{
	si argv_dis=*dis_all, argv_time = *accummulate_time;
	printf("%s\n",tag);
	printf("%d\n",strcmp(tag,"-i"));
	if(strcmp(tag,"-d")==0){
		argv_dis = get_argv(value);  
		if(argv_dis<0)
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
    if(argv_dis==0)
		argv_dis  = DEFAULT_DISPLAY;
	if(argv_time==0)
		argv_time = DEFAULT_TIME;
	*dis_all = argv_dis;
	*accummulate_time = argv_time;
	return 0;
}


static si LogToCSV()
{
	si ret = 0;
	char displag_all[] = "1";
	if(!access(DDR_LOG,F_OK)){
		if(!access(DDR_LOG_TO_CSV,F_OK)){
			char command[BUFFER_SIZE] = ".";
			strcat(command, DDR_LOG_TO_CSV);
			strcat(command, " ");
			strcat(command, DDR_LOG);
			strcat(command, " ");
			strcat(command, displag_all);
			strcat(command, " > ");
			strcat(command, DDR_BM_RESULT);
			printf("%s\n", command);
			ret = system(command);
		}else{
			fprintf(stderr, "there is no log tool %s\n", DDR_LOG_TO_CSV);
			return ERROR;
		}
	}else{
		fprintf(stderr, "there is no log %s\n", DDR_LOG);
		return ERROR;
	}
	return ret;
}

static si get_ddr_log(si accummulate_time)
{
	if(close_thermal_IPA()<0){
		return ERROR;
	}
	if(set_ddr_mode(accummulate_time)<0){
		return ERROR;
	}
}


static chn_node CreateChnLink(chn_node tail, char *buffer, int buffer_len)
{
	chn_node new_node = NULL;
	new_node = (chn_node)malloc(sizeof(*new_node));
	if(new_node==NULL){
		fprintf(stderr, "there is no space\n");
		return NULL;
	}
	new_node->name = (char *)malloc(buffer_len);
	strcpy(new_node->name, buffer);
	new_node->next = NULL;
	tail->next = new_node;
	return new_node;
}

static chn_node find_chn_link(void)
{
	FILE *chn_file = NULL;
	chn_node chn_head = NULL;
	chn_node tail	= NULL;
	char buffer[BUFSIZE];
	
	chn_file = fopen(CHANNEL_PATH, "r");
	if(chn_file==NULL){
		fprintf(stderr, "file open the file", CHANNEL_PATH);
		return NULL;
	}
	
	chn_head = (chn_node)malloc(sizeof(*chn_head));
	if(chn_head==NULL){
		fprintf(stderr, "%s :there is no space\n", __func__);
		fclose(chn_file);
		return NULL;
	}
	chn_head->next = NULL;
	
	tail = chn_head;
	// 以行读取数据
	while(fgets(buffer, BUFSIZE, chn_file)){
		tail = CreateChnLink(tail, buffer, sizeof(buffer)+1);
		if(tail==NULL){
			fclose(chn_file);
			return NULL;
		}
	}
	fclose(chn_file);
	return chn_head;
}

static u32 show_title(u32 dis_all, chn_node point)
{
	u32 count_num = 0;
	char chn_title[BUFSIZE];
	char *ch = NULL;
	char *chn_clom[]={"rt","rb","rl","wt","wb","wl"};
	
	if(dis_all==1){
		printf("%s,","count");
		printf("%s,","time");
		printf("%s,%s,","t_start","t_stop");
	}
	
	// print title data
	while(point != NULL){
		int flag = 0;
		int i = 0;
		int j = 0;
		memset(chn_title,0,BUFSIZE);
		for(ch=point->name; *ch; ch++){
			if(*ch==':'){
				flag = 1;
			}
			if(flag==1&&*ch!=9&&*ch!=':'&&*ch!='\n'){
				chn_title[i]= *ch;
				i++;
			}
		}
		for(j=0; j<SIZE_CLOM; j++){
			if(dis_all==1){
				printf("%s %s,",chn_title,chn_clom[j]);
			}else{
				if(strcmp(chn_clom[j], "rb") == 0 || strcmp(chn_clom[j],"wb")==0){
					printf("%s %s,", chn_title, chn_clom[j]);
				}
			}
		}
		point = point->next; 
		count_num++;
	}
	printf("\n");
	return count_num;
}


static si anasyis_log(int dis_all, char *log_path)
{
		FILE *fp = NULL;
	struct bm_per_info bm_info;
	chn_node chn_head = NULL;
	char *chn_clom[]={" rt"," rb"," rl"," wt"," wb"," wl"};
	int count_num=0;
	
	chn_head = find_chn_link();
	if(chn_head==NULL||chn_head->next==NULL){
		fprintf(stderr, "failed to find chn\n");
		return -1;
	}
	
#if DEBUG
	chn_node point	= NULL;
	point = chn_head->next;
	while(point){
		printf("%s\n", point->name);
		point = point->next;
	}
#endif

	fp = fopen(log_path, "rb");
	if (fp==NULL){
		fprintf(stderr, "file open the file", argv[1]);
		return -1;
	} 
	
	// show title
	count_num = show_title(dis_all, chn_head->next);

	// show data
	// 一次读取size大小个字节
	while (1 == fread(&bm_info, sizeof(bm_info), 1, fp)){
		int i = 0;
		int j = 0;
		
		for(i=0; i<count_num; ++i){
			if(bm_info.per_data[i][0]!=0)
				bm_info.per_data[i][2]=bm_info.per_data[i][2]/bm_info.per_data[i][0];
			if(bm_info.per_data[i][3]!=0)
				bm_info.per_data[i][5]=bm_info.per_data[i][5]/bm_info.per_data[i][3];
		}
		
		if(dis_all==1){
			printf("%d,", bm_info.count);
			printf("%d,", bm_info.tmp1);
			printf("%u,%u,",bm_info.t_start, bm_info.t_stop);
		}
		for(i=0; i<count_num; i++){
			for(j=0; j<SIZE_CLOM; j++){
				if(dis_all==1){
					printf("%u,",bm_info.per_data[i][j]);
				}else{
					if(j==1||j==4){
						printf("%u,",bm_info.per_data[i][j]);
					}
				}
			}
		}
		printf("\n");
	}
	fclose(fp);
	return 0;
}

int main(int argc, char *argv[])  
{
	char log_path[] = "/mnt/obb/axi_per_log";
	si   accummulate_time = 0;
	int dis_all = 0;
	if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        fprintf(stderr, "expl: ./data/power_tools/ddr/ddr_bm/ddr_bm -t 240(s) -d 1\n");  
        return INVALID_ARGV;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
		fprintf(stderr, "expl: ./data/power_tools/ddr/ddr_bm/ddr_bm -t 240(s) -d 1\n");  
        return INVALID_ARGV;  
	} 
	
	printf("yuanhui");
	// input argc 1  
    if(argc==1){  
        dis_all = DEFAULT_DISPLAY;  
        accummulate_time = DEFAULT_TIME;  		
    // input argc 3  
    }else if(argc==3){
		if(check_argv(&dis_all, argv[1], &accummulate_time, argv[2])<0)
			return INVALID_ARGV;
    // input argc 3  or more
    }else{
		printf("yuanhui");
		if(check_argv(&dis_all, argv[1], &accummulate_time, argv[2])<0)
			return INVALID_ARGV;
		if(check_argv(&dis_all, argv[3], &accummulate_time, argv[4])<0)
			return INVALID_ARGV;
	}
	
	printf("accummulate_time is %d(s)\n", accummulate_time); 
	if(getuid()!=0){
		fprintf(stderr, "You should download userdebug and su root Firstly\n");
		return NOT_ROOT;
	}
	
	if(get_ddr_log(accummulate_time)<0){
		return ERROR;
	}
	
	anasyis_log(dis_all, log_path);
	
	return 0;
}