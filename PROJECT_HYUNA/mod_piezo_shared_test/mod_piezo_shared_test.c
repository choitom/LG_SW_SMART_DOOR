#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>			
#include <string.h>
#include <sys/ioctl.h>			
#include "/usr/include/linux/i2c-dev.h"
#include "../mod_piezo_shared/mod_piezo_shared.h"

#define JOG_UP      5
#define JOG_DOWN    6
#define JOG_LEFT   16
#define JOG_RIGHT  20
#define JOG_CENTER 21

/* TODO 4.1 miscdevice 장치 작성 */
#define MOTOR_NODE_NAME 
#define PIEZO_NODE_NAME 	"mod_piezo"
#define USONIC_NODE_NAME 


/* TODO 4.2 온도 센서 값 정의 */
#define TEMP_VAL_1 
#define TEMP_VAL_2 
#define TEMP_VAL_3 

int curr_menu_pos=1;

int fd_piezo;


/*
 * message format
 *
 * low 16bit : command
 * 		1 : start
 * 		2 : stop
 * 		3 : direction
 * 		4 : speed
 *
 * hi 16bit : expend
 * 		command(3)
 * 			0 : cw
 * 			1 : ccw
 * 		command(4)
 * 			speed(1 ~ 10)
 */
void piezo_on(void)
{
	fd_piezo = open(PIEZO_NODE_NAME, O_RDWR);
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

	piezo_off();
}

/*
 * menu item(curr_menu_pos) of fnd 
 * 
 * 1 : temp
 * 2 : light
 * 3 : usonic 
 */

void on_button_clicked(int clicked)
{
	int status;
	switch(clicked) {
		printf("cur menu %d\n", curr_menu_pos);

	case JOG_DOWN: // down item
		if(curr_menu_pos < 3) 
			++curr_menu_pos;
		break;

	case JOG_UP: // up item
		if(curr_menu_pos > 1)
			--curr_menu_pos;
		break;
	case JOG_LEFT: // selected sensor on
		//sensor_on();
		break;
	case JOG_RIGHT: // selected sensor off
		//sensor_off();
		break;
	case JOG_CENTER: // all sensor on/off
		/* TODO 4.3 모든 장치를 OFF 후 프로그램 종료 함수 구현  */
		alert_sound();
		printf("#### Exit program, status = %d\n", status);
		exit(0);
	}
}

int main(int argc, char * argv[])
{
	int i;

	set_callback(on_button_clicked);

	while(1) {

	}
	return 0;
}
