/*
* i2c_temp_app.c                                                          
* SHT20 i2c Temp & Humidity Sensor Test Application For Raspberry Pi3    
*/

#include <unistd.h>			
#include <fcntl.h>			
#include <sys/ioctl.h>			
#include "/usr/include/linux/i2c-dev.h"
#include <stdio.h>

#define TEMP_HUMI_ADDR	0x40
#define CMD_TEMP		0xf3
#define CMD_HUMI		0xf5
#define SOFT_RESET		0xfe

#define TEMP_HUMI_DEVICE_FILE 	"/dev/i2c-1"

int fd_temp_humi;
void init_temp_humi_sensor(void);
float get_temp_data(void);
float get_humi_data(void);
void exit_temp_humi_sensor(void);

void init_temp_humi_sensor()
{
	if ((fd_temp_humi = open(TEMP_HUMI_DEVICE_FILE, O_RDWR)) < 0) {
		printf("Failed to open the i2c bus");
		return;
	}

	if (ioctl(fd_temp_humi, I2C_SLAVE, TEMP_HUMI_ADDR) < 0)	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		return;
	}

	int result = i2c_smbus_write_byte(fd_temp_humi, SOFT_RESET);
	if(result < 0) {
		printf("Failed to write to  the i2c bus.\n");
		return ;
	}
	usleep(50*1000); 	
}

void exit_temp_humi_sensor()
{
	if(fd_temp_humi > 0)
		close(fd_temp_humi);
}

float get_temp_data()
{
	float temp_val = 0;
	__u8 data1,data2;

	int result = i2c_smbus_write_byte(fd_temp_humi, CMD_TEMP); 
	if(result < 0) {
		printf("Failed to write to  the i2c bus.\n");
		return -1;
	}

	usleep(260*1000); 	

	data1 = i2c_smbus_read_byte(fd_temp_humi); 
	if(data1 < 0) {
		printf("Failed to read from the i2c bus.\n");
		return -1;
	}

	data2 = i2c_smbus_read_byte(fd_temp_humi); 
	if(data2 < 0) {
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to read from the i2c bus.\n");
		return -1;
	}

	result = data1<<8 | data1;
	temp_val = -46.85+175.72/65536*result;

	//printf(">>>result=0x%x  temp_val = %.2f\n", result,temp_val);
	return temp_val;
}


float get_humi_data()
{
	float humi_val = 0;
	__u8 data1,data2;

	int result = i2c_smbus_write_byte(fd_temp_humi, CMD_HUMI); 	
	if(result < 0) {
		printf("Failed to write to  the i2c bus.\n");
		return -1;
	}

	usleep(260*1000); 	

	data1 = i2c_smbus_read_byte(fd_temp_humi); 
	if(data1 < 0) {
		printf("Failed to read from the i2c bus.\n");
		return -1;
	}

	data2 = i2c_smbus_read_byte(fd_temp_humi); 	
	if(data2 < 0) {
		printf("Failed to read from the i2c bus.\n");
		return -1;
	}

	result = data1<<8 | data1;
	humi_val = -6.0+125.0/65536*result;
	//printf(">>>result=0x%x  humi_val = %.2f\n", result,humi_val);
	return humi_val;
}

