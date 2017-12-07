#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <sys/ioctl.h>
#include "/usr/include/linux/i2c-dev.h"
#include "people.h" 
#include "speaker.h"


//---- servo motor ---- //
#include <wiringPi.h>
#include <softPwm.h>

#define SERVO 8
#define OPEN 1
#define CLOSE 2

void door_ctl(int cmd)
{
	switch(cmd)
	{
		case OPEN:
			printf("open the door\n");
			softPwmWrite(SERVO, 24);
			usleep(200000);
			softPwmWrite(SERVO, 0);
			break;
		case CLOSE:
			printf("close the door\n");
			softPwmWrite(SERVO, 5);
			usleep(200000);
			softPwmWrite(SERVO, 0);
			break;
		default:
			printf("undefined door cmd\n");
			break;
	}
}

int door_drv_open(void)
{
        int ret=0;
        ret = wiringPiSetup();
        if(ret == 1)
                printf("failed : door_drv_open\n");
        else
        {
                printf("success : door_drv_open\n");
                softPwmCreate(SERVO, 0, 200);
                door_ctl(CLOSE);
        }
 
}

//--------------------------




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



int video_num = 0;
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
	person = faceRecognition(video_num);
}

int main(void) {
	door_drv_open();
	set_callback(sw_callback);

	while(1){
		// if person is not defined
		if(person < 0) continue;

		if(person == TAEHO)
			welcome("TAEHO");
		else if(person == TOM)
			welcome("TOM");
		else if(person == HYUNA)
			welcome("HYUNA");
		else if(person == GEUNSU)
			welcome("GEUNSU");
		else if(person == HAEYOON)
			welcome("HAEYOON");
		else if(person == SEUNG)
			welcome("SEUNG");

		else{ // else record
			char video_name[30];
			snprintf(video_name, 30, "%s%d%s", "visitor_recording", video_num, ".avi");
			play_video(video_name);
			video_num++;
		}

		if(person == TAEHO || person == TOM || person == HYUNA ||
                person == GEUNSU || person == HAEYOON || person == SEUNG)
		{
			printf("%d\n", person);		
			door_ctl(OPEN);
		}
		usleep(200000);
		door_ctl(CLOSE);
		person = -1;
	}
	return 0;
}
