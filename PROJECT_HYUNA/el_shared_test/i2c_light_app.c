/*
 * i2c_light_app.c  
 * BH1750 i2c Light Sensor Test Application For Raspberry Pi3 
 */

#include <unistd.h>			
#include <fcntl.h>			
#include <sys/ioctl.h>			
#include "/usr/include/linux/i2c-dev.h"
#include <stdio.h>

#define LIGHT_ADDR	0x23
#define CON_HR_MODE 	0x10
#define RESET       	0x07

#define LIGHT_DEVICE_FILE 	"/dev/i2c-1"

int fd_light;
void light_on(void);
float get_light_data(void);
void light_off(void);

void light_on()
{
	if ((fd_light = open(LIGHT_DEVICE_FILE, O_RDWR)) < 0) {
		printf("Failed to open the i2c bus");
		return;
	}

	if (ioctl(fd_light, I2C_SLAVE, LIGHT_ADDR) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return;
	}
}

void light_off()
{
	if(fd_light > 0)
		close(fd_light);
}

float get_light_data()
{
	float light_val = 0;
	__u8 data1,data2;
	int result = i2c_smbus_read_word_data(fd_light, CON_HR_MODE); 	

	if(result < 0) {
		printf("Failed to read from the i2c bus.\n");
		return -1;
	}

	data1 = result & 0xff;
	data2 = (result >> 8) & 0xff;
	light_val = ((data2 + (256*data1))/1.2);

	// printf(">>>result=0x%x  light_val = %.2f\n", result,light_val);
	return light_val;
}

#if 0
int main() 
{
	float light_data;

	init_light_sensor();

	while(1) {
		light_data = get_light_data();
		printf(">>>light_data = %.2f\n",light_data);
		usleep(500*1000); //  per 0.5 sec
	}
	exit_light_sensor();
	return;
}
#endif
