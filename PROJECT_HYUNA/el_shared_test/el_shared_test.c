#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>			
#include <string.h>
#include <sys/ioctl.h>			
#include "/usr/include/linux/i2c-dev.h"
#include "../el_shared/el_shared.h"

#define JOG_UP      5
#define JOG_DOWN    6
#define JOG_LEFT   16
#define JOG_RIGHT  20
#define JOG_CENTER 21

/* TODO 4.1 miscdevice 장치 작성 */
#define MOTOR_NODE_NAME 
#define PIEZO_NODE_NAME 
#define USONIC_NODE_NAME 


/* TODO 4.2 온도 센서 값 정의 */
#define TEMP_VAL_1 
#define TEMP_VAL_2 
#define TEMP_VAL_3 

int curr_menu_pos=1;

int motor_fd;
int fd_piezo;
int usonic_fd;

int usonic_val;
float temp_val;
float light_val;

int temp_status;
int light_status;
int usonic_status;

extern pthread_t fnd_thread;
extern void init_fnd(void);
extern int fnd_thread_start_flag;
extern char fnd_data[];

extern void init_temp_humi_sensor(void);
extern float get_temp_data(void);
extern void exit_temp_humi_sensor(void);

extern void light_on(void);
extern float get_light_data(void);
extern void light_off(void);

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

static int motor_ctrl(int msg)
{
	return write(motor_fd, &msg, sizeof(msg));
}

int motor_on()
{
	motor_fd=open(MOTOR_NODE_NAME, O_WRONLY);
	if (motor_fd < 0) {
		printf("motor open filed\n");
		return motor_fd;
	}
	motor_ctrl(1);
	return motor_fd;
}

void motor_off()
{
	motor_ctrl(2);
	close(motor_fd);
}

void motor_set_direction(int flag) {
	int cmd = 3;
	cmd |= flag << 16;
	motor_ctrl(cmd);
}

void motor_set_speed(int n) { 
	int cmd = 4;
	cmd |= n << 16;
	motor_ctrl(cmd);
}

void led_on(int val) 
{
	char buf[100];
	memset(buf, 0 , sizeof(buf)); 
	sprintf(buf, "echo %d > /sys/devices/platform/sysfs_el_led/value", val);
	system (buf);
}

void led_off(void)
{
	system ("echo 0 > /sys/devices/platform/sysfs_el_led/value");
}

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

void usonic_on(void)
{
	usonic_fd = open(USONIC_NODE_NAME, O_RDWR);
	if (usonic_fd < 0)
		printf("usonic open failed\n");
}

void usonic_off()
{
	close(usonic_fd);
}

void sensor_on()
{
	if (curr_menu_pos == 1) {
		init_temp_humi_sensor();
		motor_fd=motor_on();
		temp_status=1;
	}
	else if (curr_menu_pos == 2) {
		light_on();
		light_status=1;
	}
	else if (curr_menu_pos == 3) {
		usonic_on();
		usonic_status=1;
	}
	else  {
		printf("no such sensor\n");
	}
}

void sensor_off()
{
	if (curr_menu_pos == 1) {
		exit_temp_humi_sensor();
		motor_off();
		temp_status=0;
		temp_val=0;
	}
	else if (curr_menu_pos == 2) {
		light_off();
		led_off();
		light_status=0;
		light_val=0;
	}
	else if (curr_menu_pos == 3) {
		usonic_off();
		usonic_status=0;
		usonic_val=0;
		temp_val=0;
	}
	else {
		printf("no such sensor\n");
	}
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
		sensor_on();
		break;
	case JOG_RIGHT: // selected sensor off
		sensor_off();
		break;
	case JOG_CENTER: // all sensor on/off
		/* TODO 4.3 모든 장치를 OFF 후 프로그램 종료 함수 구현  */
		printf("#### Exit program, status = %d\n", status);
		exit(0);
	}
}

int main(int argc, char * argv[])
{
	int i;

	char sensor_data[6];

	set_callback(on_button_clicked);

	init_fnd();

	while(1) {
		if (temp_status == 1) {
			temp_val=get_temp_data(); // about 260*1000us
			printf("%f\n", temp_val);
			if (temp_val < TEMP_VAL_1) {
				motor_set_speed(0);
			}
			else if (temp_val < TEMP_VAL_2) {
				motor_set_speed(3);
			}
			else if (temp_val < TEMP_VAL_3) {
				motor_set_speed(6);
			}
			else
				motor_set_speed(10);
		}

		if (light_status ==1 ) {
			light_val=get_light_data();
			printf("light data : %f\n", light_val);

			if (light_val < 100 ) {
				led_on(3);
			}
			else if (light_val < 200) {
				led_on(1);
			}
			else
				led_off();
		}

		if (usonic_status == 1) {
			read(usonic_fd, &usonic_val, sizeof(int));
			printf("usonic data = %d\n", usonic_val);
			if (usonic_val< 10)
				alert_sound();
		}

		if (curr_menu_pos == 1) {
			sprintf(sensor_data, "%6f", temp_val);
		}
		else if (curr_menu_pos==2) {
			sprintf(sensor_data, "%6f", light_val);
		}
		else {
			sprintf(sensor_data, "%6d", usonic_val);
		}

		for (i=0; i<6; i++) {
			if (sensor_data[i] == '.')
				sensor_data[i]=10;
			else
				sensor_data[i]= sensor_data[i]-'0';
		}

		memcpy(fnd_data, sensor_data, sizeof(sensor_data) );
		sleep(1);
	}
	return 0;
}
