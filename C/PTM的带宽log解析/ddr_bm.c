#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <unistd.h>
#include <sched.h>
#include <float.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <assert.h>
#include <ctype.h>

#include "ddr_bm.h"

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

// argc是传入的参数的个数
// ×argv[]是传入的参数
int main(int argc, char *argv[])
{
	FILE *fp = NULL;
	struct bm_per_info bm_info;
	chn_node chn_head = NULL;
	char *chn_clom[]={" rt"," rb"," rl"," wt"," wb"," wl"};
	int count_num=0;
	int dis_all=0;
	
	// check argument
	if(argc!=3){
		fprintf(stderr, "you should input correct argv\n");
		printf("fist: log_to_csv tool \n");
		printf("secn: log_file \n");
		printf("thir: wheather displag all data(0/1) \n");
		printf("expl: ./data/ddr_bm_log /data/axi_per_log 1\n");
		return -1;
	}
	
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

	fp = fopen(argv[1], "rb");
	if (fp==NULL){
		fprintf(stderr, "file open the file", argv[1]);
		return -1;
	}
	
	if(strcmp(argv[2],"1")==0){
		dis_all = 1;
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
