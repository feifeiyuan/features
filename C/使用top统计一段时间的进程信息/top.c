#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>  
#include <unistd.h>  
#include <sys/time.h>  
#include<unistd.h>

#include "top.h"

static void clear_envirment(char *file)  
{   
	if((access(file, F_OK))!=-1){   
		remove(file);  
    } 
} 

static top_tree InsertNode(top_tree tree, struct top_component top_men)
{
	if(tree==NULL){
		tree = (top_tree)malloc(sizeof(*tree));
		if(tree==NULL){
			fprintf(stderr,"there is no space\n");  
			return NULL;
		}
		tree->top_menber = top_men;
		tree->Left = NULL;
		tree->Right = NULL;
	}else if(top_men.cpu_per >= tree->top_menber.cpu_per){
		tree->Right = InsertNode(tree->Right, top_men);
	}else{
		tree->Left = InsertNode(tree->Left, top_men);
	}
	return tree;
}

static s8 TailView(top_tree tree, s8 count, ui num)
{
	static s8 order = 0;
	if(tree){
		order = TailView(tree->Right, order, num);
		if(order<num){
			printf("%ld\t%.1f\t%s",tree->top_menber.pid,  tree->top_menber.cpu_per, tree->top_menber.args);
			order++;
		}
		order = TailView(tree->Left, order, num);
	}
	return order;
}

static void ComputeAverage(top_node head, ui num)  
{  
    top_node position = head->Next;  
	top_tree tree = NULL;
	printf("PID\t%[CPU]\t%s\n", "ARGS");
    while(position!=NULL){
		position->top_menber.cpu_per = position->top_menber.cpu_per/position->top_menber.count;
		//printf("%ld\t%.1f\t%s", position->top_menber.pid,  position->top_menber.cpu_per, position->top_menber.args);
        position = position->Next;  
    }
	// change double find heap
	position = head->Next;  
	while(position){
		tree = InsertNode(tree, position->top_menber);
		position = position->Next;
	}
	//printf("\nroot: %ld\t%.1f\t%s\n",tree->top_menber.pid,  tree->top_menber.cpu_per, tree->top_menber.args);
	TailView(tree, 0, num);
	return;
}

static s8 get_top_info(ui accummulate_time)
{
	u8 ret = 0, interval = 3;
	ui times = 0;
	char command[BUFFER_SIZE] = "top -n 1";
	strcat(command, " -d 3 >> "TOP_RESULT_PATH);
	
	clear_envirment(TOP_DIR);
	ret = system("mkdir "TOP_DIR);
	if(ret!=0){
		fprintf(stderr, "failed to mkdir\n");
		return ERROR;
	}
	
	while(times<accummulate_time/interval){
		ret = system(command);
		ret = system("echo " " >> "TOP_RESULT_PATH);
		ret = system("echo " " >> "TOP_RESULT_PATH);
		if(ret!=0){
			fprintf(stderr, "failed to exce top comamnd\n");
			return ERROR;
		}
		sleep(interval);
		times++;
	}
	
	return ret;
}

static top_node FindNode(top_node head, uli pid)
{
	top_node position = NULL;
	position = head->Next;
	while(position){
		//printf("pid is %ld\t position->pid is %ld\n", pid, position->pid);
		if(position->top_menber.pid==pid){
			return position;
		}
		position = position->Next;
	}
	return NULL;
}

static top_node CreateListCell(top_node head, top_node ahead, float percent, char *cmd, uli pid){  
    top_node position = NULL;  
	position = FindNode(head, pid);
	if(position==NULL){
		position  = (top_node)malloc(sizeof(struct top_info));  
		if(position==NULL){  
			fprintf(stderr, "there is no space\n");  
			return position;  
		}  
		position->top_menber.cpu_per = percent;  
		position->top_menber.count = 1;
		position->top_menber.pid = pid;
		strcpy(position->top_menber.args, cmd);
		//printf("pid is %ld\n", pid);
		ahead->Next = position;  
		position->Next = NULL;  
	}else{
		//printf("find it pid is %ld\n", pid);
		position->top_menber.cpu_per += percent;
		position->top_menber.count++;
		while(position->Next){
			position = position->Next;
		}
	}
    return position;  
}  

static top_node CreateHead()  
{  
    top_node head = NULL;  
    head  = (top_node)malloc(sizeof(struct top_info));  
    if(head==NULL){  
        fprintf(stderr, "there is no space\n");  
        return NULL;  
    }  
    head->top_menber.cpu_per = 0;  
	strcpy(head->top_menber.args, "head");
	head->top_menber.count = 1;
	head->top_menber.pid = 0;
    head->Next = NULL;  
    return head;  
}  
  

static s8 deal_statistic(u8 cpu_index, u8 arg_index, u8 pid_index, ui num)
{
	top_node head =  CreateHead();  
    top_node position = head;  
	s8 flag = 0, count = 0, order, j = 0, k = 0;
	char buffer[BUFFER_SIZE], *child="PID USER", command[BUFFER_SIZE], cache[BUFFER_SIZE];
	char *delim = " ";
	struct top_info data;
	float percent = 0.0;
	uli pid =0;
	FILE *fp = NULL;
	
	fp = fopen(TOP_RESULT_PATH, "r");
	if(fp==NULL){
		fprintf(stderr, "failed to open the file ", TOP_RESULT_PATH);
		return FAILED_OPEN_FILE;
	}
	while(fgets(buffer, BUFFER_SIZE, fp)){
		if(strcmp(cache, "") ==0){
			strcpy(cache, buffer);
		}
		
		if(strcmp(buffer,"\n")==0){
			flag = 0;
			continue;
		}
		
		if(strstr(cache, child)){
			flag = 1;
			count = 0;
			memset(cache, 0, BUFFER_SIZE);
			strcpy(cache, buffer);
			continue;
		}
		
		//printf("%s", cache);
		if(flag==1&&count<num){
			//printf("%s\n", cache);
			order = 0;
			memset(command, 0, BUFFER_SIZE);
			char *point = strtok(cache, delim);
			while(point!=NULL){
				if(order==pid_index){
					pid = atol(point);
				}else if(order==cpu_index){
					if(strstr(point,"%")){
						//printf("%s\n",point);
					}
					percent = atof(point);
				}else if(order>=arg_index){
					strcat(command, point);
					strcat(command, " ");
				}
				point = strtok(NULL, delim);
				order++;
			}
			//printf("%ld\t%.1f\t%s", pid, percent, command);
			position = CreateListCell(head, position, percent, command, pid);  
			//printf("%s", data[i/TOP_AHEAD][count].args);
			count++;
		}
		memset(cache, 0, BUFFER_SIZE);
		strcpy(cache, buffer);
	}
	fclose(fp);
	ComputeAverage(head, num);  
	return 0;
}

static si get_processor_num(u8 *cpu_index, u8 *arg_index, u8 *pid_index)
{
	FILE *fp = NULL;
	char buffer[BUFFER_SIZE], *child="PID USER", *delim = " ";
	fp = popen("top -n 1","r");
	u8 order = 0;
	if(fp==NULL){
		fprintf(stderr, "%s:failed run commond\n", __func__, "/proc/cpuinfo| grep \"processor\"| wc -l");  
        return FAILED_OPEN_FILE; 
	}
	while(fgets(buffer, BUFFER_SIZE, fp)){
		if(strstr(buffer, child)){
			if(strstr(buffer, "Name")){
				char *point = strtok(buffer, delim);
				while(point!=NULL){
					if(strstr(point, "PID")){
						*pid_index = order;
					}else if(strstr(point,"CPU")){
						*cpu_index = order;
					}else if(strstr(point,"Name")){
						*arg_index = order;
					}
					point = strtok(NULL, delim);
					order++;
				}
			}else{
				char *point = strtok(buffer, delim);
				while(point!=NULL){
					if(strstr(point, "PID")){
						*pid_index = order-1;
					}else if(strstr(point,"[%CPU]")){
						*cpu_index = order;
					}else if(strstr(point,"ARGS")){
						*arg_index = order;
					}
					point = strtok(NULL, delim);
					order++;
				}
			}
		}
	}
	pclose(fp);
	return 0;
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

static si check_argv(ui *num,char *tag, si *accummulate_time, char *value)
{
	si argv_time = *accummulate_time, argv_num = *num;
	//printf("%s\n",tag);
	//printf("%d\n",strcmp(tag,"-i"));
	if(strcmp(tag,"-n")==0){
		argv_num = get_argv(value); 
		if(argv_num<0)
			return INVALID_ARGV;    			 
	}else if(strcmp(tag,"-t")==0){
		argv_time = get_argv(value); 
		if(argv_time<0)
			return INVALID_ARGV;    			 
	}else{
		fprintf(stderr, "you must input correct argvs\n");   
		fprintf(stderr, "expl: ./data/local/tmp/cpu_sgm -t 60(s)\n");  
		return INVALID_ARGV; 
	}
	if(argv_time==0)
		argv_time = DEFAULT_TIME;
	if(argv_num==0)
		argv_num = DEFAULT_NUM;
	
	*accummulate_time = argv_time;
	*num = argv_num;
	return 0;
}

int main(int argc, char *argv[])
{	
	u8 cpu_index=0, arg_index = 0, pid_index;
	ui accummulate_time = 0, num = 0;
	if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        fprintf(stderr, "expl: ./data/local/tmp/sprd_top -t 60(s)\n");  
        return INVALID_ARGV;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
		fprintf(stderr, "expl: ./data/local/tmp/cpu_sgm -t 60(s)\n");  
        return INVALID_ARGV;  
	}
	if(argc==1){  
        accummulate_time = DEFAULT_TIME;  	
		num = DEFAULT_NUM;
    // input argc 3  
    }else if(argc==3){
		if(check_argv(&num, argv[1], &accummulate_time, argv[2])<0)
			return INVALID_ARGV;
	// input argc 3  or more
    }else{
		if(check_argv(&num, argv[1], &accummulate_time, argv[2])<0)
			return INVALID_ARGV;
		if(check_argv(&num, argv[3], &accummulate_time, argv[4])<0)
			return INVALID_ARGV;
	}
	//printf("%d\t%d\n", accummulate_time, num);
	
	
	clear_envirment(TOP_RESULT_PATH);
	if(get_top_info(accummulate_time)<0)
		return ERROR;
	
	
	if(get_processor_num(&cpu_index, &arg_index, &pid_index)<0){
		return ERROR;
	}
	printf("%d\t%d\t%d\n", pid_index, cpu_index, arg_index);
	
	
	if(deal_statistic(cpu_index, arg_index, pid_index, num)<0){
		return ERROR;
	}
	return 0;
}
