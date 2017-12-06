#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

#include <linux/delay.h>
#include <linux/kthread.h>

#define PIEZO		13

#define HIGH		1
#define LOW		0

static int piezo_data[1];
struct task_struct *play_task=NULL;

int scale[] = {1790, 1396};

static int gfreq;

static void drv_hw_piezo_on(void)
{
	gpio_set_value(PIEZO, LOW);
}

static void drv_hw_piezo_off(void)
{
	gpio_set_value(PIEZO, HIGH);
}
static void drv_hw_piezo_play(void)
{
	int i;
	for(i = 0;i <1000;i++)
	{
		drv_hw_piezo_on();
		udelay((1000000/scale[0])/2);

		drv_hw_piezo_off();
		udelay((1000000/scale[0])/2);
	}

	for(i = 0;i <1000;i++)
	{
		drv_hw_piezo_on();
		udelay((1000000/scale[1])/2);

		drv_hw_piezo_off();
		udelay((1000000/scale[1])/2);
	}
}

int piezo_play_thread(void *data){
	/* TODO 3.1  piezo pwm을 위한 스레드 함수 구현 */
	int count = 0;
	while(!kthread_should_stop()){
		drv_hw_piezo_play();
		if(count++ > 300) {
			count = 0;
	//		usleep(0);
		}
	}
	printk("stop thread...\n");
	return 0;
}

static int drv_hw_piezo_open(struct inode *inode, struct file *file)
{
	/* TODO 3.2  커널 스레드 함수 (piezo_play_thread) 구현 */
	play_task = kthread_run(piezo_play_thread, NULL, "play_thread");
	//gfreq = scale[0];
	return 0;
}

static int drv_hw_piezo_release(struct inode *inode, struct file *file)
{
	/* TODO 3.3 커널 스레드 종료함수 구현 */
	if(play_task)
		kthread_stop(play_task);
	return 0;
}

static ssize_t drv_hw_piezo_write(struct file * file, const char __user * buf, size_t length, loff_t * ofs)
{
	int ret;
	ret = copy_from_user(piezo_data,buf, length);
	gfreq = piezo_data[0];
	//gfreq = 2000;
	return ret;
}

static struct file_operations drv_hw_piezo_fops = 
{
	.owner = THIS_MODULE,
	.write = drv_hw_piezo_write,
	.open    = drv_hw_piezo_open,
	.release = drv_hw_piezo_release,
};

static struct miscdevice drv_hw_piezo_driver = 
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = "mod_piezo",
	.fops = &drv_hw_piezo_fops,
};

static int drv_hw_piezo_init(void)
{
	int piezo;

	printk("%s\n", __FUNCTION__);

	piezo=gpio_request(PIEZO,"gpio piezo");
	if(piezo != 0 ) {
		printk("#### FAILED Request gpio %d error!! \n", piezo);
	}

	gpio_direction_output(PIEZO, HIGH);
	return misc_register(&drv_hw_piezo_driver);
}

static void drv_hw_piezo_exit(void)
{ 
	/* 3.4 piezo 드라이버 종료 함수 구현 */
	gpio_set_value(PIEZO, HIGH);
	gpio_free(PIEZO);
	misc_deregister(&drv_hw_piezo_driver);
}

module_init(drv_hw_piezo_init);
module_exit(drv_hw_piezo_exit);

MODULE_AUTHOR("PlanX Studio");
MODULE_DESCRIPTION("drv_hw_piezo");
MODULE_LICENSE("Dual BSD/GPL");
