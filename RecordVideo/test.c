#include <stdio.h>
#include <stdlib.h>

static int video_num = 0;

void record()
{
    record_video(video_num++);
}

int main(void)
{
	record();
   	//play_video("visitor_recording0.avi");

    record();
    //play_video("visitor_recording1.avi");
	printf("DONE\n");
	return 0;
}
