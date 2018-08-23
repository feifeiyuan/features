#include<stdio.h>
#include<error.h>
#include<sys/time.h>

#define ERROR			-22
#define INVALID_ARGV	-2

#define DEFAULT_TIME	60
#define DEFAULT_DIS		1

typedef signed      int    si;  
typedef signed      char   s8;  
typedef unsigned    char   u8;  
typedef unsigned    int    ui;  
typedef unsigned long int  uli;  
typedef unsigned long long ulli;

static long int dumpsys()
{
	long int frame = 0;
	FILE *fp = NULL;
	fp = popen("dumpsys SurfaceFlinger | grep -o -E 'flips=[0-9]*' | sed 's/flips=//g'", "r");
	if(fp==NULL){
		fprintf(stderr, "Failed execute %s\n", "dumpsys SurfaceFlinger | grep flips");
		return ERROR;
	}
	fscanf(fp, "%ld", &frame);
	pclose(fp);
	return frame;
}

static long int cal_fps(long int start_frame, float *ret_fps)
{
	struct timeval start, end;  
    float timeuse = 0.0, fps = 0.0;
	long int end_frame = 0, frame=0;
	
	gettimeofday(&start, NULL);  
	if(start_frame==0){
		start_frame = dumpsys();
	}
	usleep(890000);
	end_frame = dumpsys();
	frame = end_frame - start_frame;
	gettimeofday(&end, NULL);  
	timeuse = (1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec)/1000000.0; 
	fps = frame/timeuse;
	//printf("time: %.2fs\t frame:%ld\t fps:%.2f\n", timeuse, frame, fps); 
	*ret_fps = fps;
	return end_frame;
}

static int get_argv(char *time_char)  
{  
    char *ch = time_char;  
    int ret = 0;  
      
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

static si check_argv(si *dis, char *tag, si *accummulate_time, char *value)
{
	si argv_dis=*dis, argv_time = *accummulate_time;
	if(strcmp(tag,"-d")==0){
		if(strcmp(value, "0")==0)
			argv_dis = 0;
		else
			argv_dis = 1;
		if(argv_dis<0)
			return INVALID_ARGV;  
	}else if(strcmp(tag,"-t")==0){
		argv_time = get_argv(value); 
		if(argv_time<0)
			return INVALID_ARGV;    			 
	}else{
		fprintf(stderr, "you must input correct argvs\n");   
		fprintf(stderr, "expl: ./data/power_tools/fps/fps -t  1(s)\n");  
		return INVALID_ARGV; 
	}
	if(argv_time==0)
		argv_time = DEFAULT_TIME;
	*dis = argv_dis;
	*accummulate_time = argv_time;
	return 0;
}

int main(int argc, char *argv[])
{
	int accumulate_time = 0;
	int dis = 0;
	long int start_frame = 0;
	float fps = 0.0;
		if( argc < 1 ){  
        fprintf(stderr, "you must input at least one argv");   
        fprintf(stderr, "expl: ./data/power_tools/fps/fps -t  1(s)\n");  
        return INVALID_ARGV;  
    }  
	if( argc%2!=1){
		fprintf(stderr, "you must input odd number argvs");   
		fprintf(stderr, "expl: ./data/power_tools/fps/fps -t  1(s)\n");  
        return INVALID_ARGV;  
	} 
	
	// input argc 1  
    if( argc==1){  
		accumulate_time = DEFAULT_TIME; 
		dis				= DEFAULT_DIS;
    // input argc 3  or more 
    }else if(argc==3){
		if(check_argv(&dis, argv[1], &accumulate_time, argv[2])<0)
			return INVALID_ARGV;
    }else{
		if(check_argv(&dis, argv[1], &accumulate_time, argv[2])<0)
			return INVALID_ARGV;
		if(check_argv(&dis, argv[3], &accumulate_time, argv[4])<0)
			return INVALID_ARGV;
	}
	printf("accumulate_time is %d\n", accumulate_time);
	
	double fps_sum = 0.0;
	int i = 0;
	
	while(i<accumulate_time){
		start_frame = cal_fps(start_frame, &fps);
		if(dis==1)
			printf("fps: %.2f\n", fps);
		fps_sum +=fps;
		i++;
	}
	printf("average fps : %.2f\n", fps_sum/i);
}
