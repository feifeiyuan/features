#include<stdio.h>
#include<error.h>
#include<sys/time.h>

#define ERROR			-22

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
int main()
{
	long int start_frame = 0;
	float fps = 0.0;
	while(1){
		start_frame = cal_fps(start_frame, &fps);
		printf("%.2f\n", fps);
	}
}
