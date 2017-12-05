#include <unistd.h>			
#include <fcntl.h>			
#include <sys/ioctl.h>			
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "/usr/include/linux/i2c-dev.h"

#define FND_ADDR	    0x20
#define CONFIG_PORT 	0x06
#define OUT_PORT      0x02

#define FND_DEVICE_FILE 	"/dev/i2c-1"

char data[] = {0xFC,0x60,0xDA,0xF2,0x66,0xB6,0x3E,0xE0,0xFE,0xF6, 0x1};
char digit[] = {0x7F,0xBF,0xDF,0xEF,0xF7,0xFB};

int fd_fnd;
int fnd_thread_id;
int fnd_thread_start_flag=1;
pthread_t fnd_thread;  /* User Thread */

char fnd_data[]={1,2,3,4,5,6};

void init_fnd(void);
void display_fnd_one(char segment,char number);
void display_fnd_all(char * number,int msec);
void exit_fnd(void);
void *fnd_thread_func(void * unuse); 

void init_fnd()
{
	if ((fd_fnd = open(FND_DEVICE_FILE, O_RDWR)) < 0) {
		//ERROR HANDLING: you can check errno to see what went wrong
		printf("Failed to open the i2c bus");
		return;
	}

	if (ioctl(fd_fnd, I2C_SLAVE, FND_ADDR) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return;
	}	

	int result = i2c_smbus_write_word_data(fd_fnd, CONFIG_PORT,0x0000);

	if(result < 0)
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write to  the i2c bus.\n");
		return ;
	}        

	fnd_thread_id = pthread_create(&fnd_thread, 0, fnd_thread_func, 0);
	if (fnd_thread_id< 0) {
		printf("#### ERROR pthread_create\n");
		exit(0);
	}
}

void exit_fnd()
{
	display_fnd_one(5,0);

	if(fd_fnd > 0)
		close(fd_fnd);
}

void display_fnd_one(char segment,char number)
{
	__u16 data;
	data = number <<8 | segment;

	int result = i2c_smbus_write_word_data(fd_fnd, OUT_PORT, data); 	
	if(result < 0) {
		printf("Failed to write to the i2c bus.\n");
		return;
	}

	return ;
}

void display_fnd_all(char * number, int msec)
{
	int i,j;

	for(i=0; i< msec/6; i++) {  // msec (mili second) duration..
		for(j=0; j<6; j++) {
			display_fnd_one(digit[j], data[number[j]]);
			usleep(1*1000);  // 1 ms
		}
	}
}

void *fnd_thread_func(void * unuse) 
{
	int ret;
	int stat;
	while(fnd_thread_start_flag) {
		display_fnd_all(fnd_data,500);
	}

	printf("#### Terminate thead...\n");
	pthread_exit((void *)0);
}
