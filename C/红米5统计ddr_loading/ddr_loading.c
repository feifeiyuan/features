#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>  
#include <unistd.h>  
#include <sys/time.h>  
#include<unistd.h>

#include "ddr_loading.h"

typedef signed      int    si;  
typedef signed      char   s8;  
typedef unsigned    char   u8;  
typedef unsigned    int    ui;  
typedef unsigned long int  uli;  
typedef unsigned long long ulli;

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

static s8 get_data(uli *ret_data)  
{  
    FILE    *fp = NULL;  
    uli data = 0;  
	char file_path[]="/sys/kernel/debug/clk/bimc_clk/measure";
    fp = fopen(file_path, "r");  
    if(fp==NULL){  
        fprintf(stderr, "failed opend file:\n", file_path);  
        return INVALID_ARGV;  
    }  
    fscanf(fp, "%ld", &data);  
    fclose(fp);  
	*ret_data = data;
    return 0;  
}


static SearchTree InsertNode(TreeElement element, SearchTree tree)
{
	if(tree==NULL){
		tree = (SearchTree)malloc(sizeof(*tree));
		if(tree==NULL){
			fprintf(stderr,"there is no space\n");
			return NULL;
		}
		tree->element= element;
		tree->count = 1;
		tree->RightNode = NULL;
		tree->LeftNode = NULL;
	}else if(element>tree->element){
		tree->RightNode = InsertNode(element, tree->RightNode);
	}else{
		tree->LeftNode = InsertNode(element, tree->LeftNode);
	}
	return tree;
}
 
static SearchTree CreateHead()
{
	SearchTree head = NULL;
	SearchTree position = NULL;
	head  = (SearchTree)malloc(sizeof(struct Node));
	if(head==NULL){
		fprintf(stderr, "there is no space\n");
		return NULL;
	}
	head->element = 0;
	head->count = 0;
	head->RightNode = NULL;
	head->LeftNode = NULL;
	return head;
}

static SearchTree Find(TreeElement element, SearchTree tree)
{
	if(tree){
		if(tree->element==element){
			return tree;
		}else if(tree->element>element){
			return Find(element, tree->LeftNode);
		}else{
			return Find(element, tree->RightNode);
		}
	}
	return NULL;
}

static void OverViewList(SearchTree tree, uli counts)
{
	if(tree){
		
		OverViewList(tree->RightNode, counts);
		if(tree->count!=0)
			printf("freq:%d, perc:%.2f%\n", tree->element, tree->count*100.0/counts);
		OverViewList(tree->LeftNode, counts);
	}
	return;	
}

static s8 data_simpling(si   accummulate_time, uli polling_interval)
{
	uli cur_freq =0, counts = 0, count = 0;
	counts = accummulate_time*1000/polling_interval;
	SearchTree head =  CreateHead();
	SearchTree position = NULL;
	SearchTree find_position = NULL;
	struct timeval start, end;  
	gettimeofday(&start, NULL);  
	float timeuse = 0.0;
	while(count<counts){
		get_data(&cur_freq);
		cur_freq /= 1000000;
		find_position = Find(cur_freq, head);
		if(find_position){
			find_position->count++;
		}else{
			InsertNode(cur_freq, head);
		}
		usleep(polling_interval*840);
		count++;
	}
	gettimeofday(&end, NULL);  
	timeuse = (1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec)/1000.0;  
	printf("time_use is %.2fms\n", timeuse);  
	OverViewList(head, counts);
	return 0;
}

int main(int argc, char *argv[])  
{
	si   accummulate_time = 0;
	uli polling_interval = 100 ;
	if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        fprintf(stderr, "expl: ./data/local/tmp/ddr_loading -t 60(s)\n");  
        return INVALID_ARGV;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
		fprintf(stderr, "expl: ./data/local/tmp/ddr_loading -t 60(s)\n");  
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
	
	data_simpling(accummulate_time, polling_interval);
}