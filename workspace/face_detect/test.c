#include <stdio.h>
#include <stdlib.h>

static int video_num = 0;

int main(void) {
    int person = faceRecognition();
	printf("The result is %dth person\n", person);

    record_video(video_num++);
    play_video("visitor_recording0.avi");
	
    return 0;
}
