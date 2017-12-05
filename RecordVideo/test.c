#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    record_video();
    play_video("visitor_recording.avi");
    printf("DONE\n");
    return 0;
}
