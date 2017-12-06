#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <sys/ioctl.h>
#include "/usr/include/linux/i2c-dev.h"
#include "mod_piezo_shared.h"

#define PIEZO_NODE_NAME         "mod_piezo"



#define SIG_CALLBACK 44
#define NODE_NAME "/dev/mod_sw_drv"
#define NODE_NAME2 "/dev/mod_piezo"


int curr_menu_pos=1;
int person = -1;

int fd_piezo;
void piezo_on(void)
{
        fd_piezo = open(NODE_NAME2, O_RDWR);
        if(fd_piezo < 0)
                printf("piezo open failed\n");
}

void piezo_off()
{
        close(fd_piezo);
}

void alert_sound()
{
        int i;
        int alert_data=261;

        piezo_on();

        for (i=0; i<100 ; i++) {
                alert_data += i;
                write(fd_piezo, &alert_data, sizeof(alert_data));
                usleep(10*1000);
        }

   //	  alert_data = 0;
   //     write(fd_piezo, &alert_data, sizeof(alert_data));
         piezo_off();
}



static int video_num = 0;
// drv jog here
int set_callback(void (*pfn_callback)(int));
int fd;

static void (*g_pfn_callback)(int);
void __attribute__((constructor)) init_callback(void)
{       
	fd = open(NODE_NAME, O_WRONLY);
	//fd2 = open(NODE_NAME2, O_WRONLY);
	//piezo_on();
}
void __attribute__((destructor)) exit_callback(void)
{
	if(fd > 0){
		close(fd);
	}
	if(fd_piezo > 0){
		piezo_off();	
}
}
static void receive_signal(int n, siginfo_t *info, void *unused)
{
	g_pfn_callback(info->si_int);
}
int set_callback(void (*pfn_callback)(int))
{

	int ret = 0, pid;
	struct sigaction sig;

	sig.sa_sigaction = receive_signal;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIG_CALLBACK, &sig, 0);
	g_pfn_callback = pfn_callback;
	pid = getpid();

	if(fd > 0){

		ret = write(fd, &pid, sizeof(pid));
	}
	return ret;
}
void sw_callback(int sw){

	printf("sw = %d\n", sw);
	alert_sound();
    person = faceRecognition();
    printf("end alert_sount\n");
}


int main(void) {
	int person;
    set_callback(sw_callback);
    //person = faceRecognition();
	//printf("The result is %dth person\n", person);
	//record_video(video_num++);
	//play_video("visitor_recording0.avi");
    
    while(1){
        // if person is owner
        if(person >= 0){
            
        }

        // else record
        else{
        }
    }
	return 0;
}
