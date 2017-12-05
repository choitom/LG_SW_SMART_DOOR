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

struct task_struct *play_task;

int scale[] = {261,294,329,349,392,440,493,523,587,659,};  // C4,D4,E4,F4,G4,A4,B4,C5,D5,E5

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
	drv_hw_piezo_on();
	udelay((1000000/gfreq)/2);
	drv_hw_piezo_off();
	udelay((1000000/gfreq)/2);
}

int piezo_play_thread(void *data){

	int count = 0;

	while(!kthread_should_stop()) {
		drv_hw_piezo_play();
		if(count++ > 30) {
			count = 0;
			ssleep(0);
		}
	}

	printk("stop thread...\n");

	return 0;
}


static int drv_hw_piezo_open(struct inode *inode, struct file *file)
{
	play_task = kthread_run(piezo_play_thread, NULL, "play_thread");
	gfreq = scale[0];
	return 0;
}

static int drv_hw_piezo_release(struct inode *inode, struct file *file)
{
	if(play_task)
		kthread_stop(play_task);
	return 0;
}


static ssize_t drv_hw_piezo_write(struct file * file, const char __user * buf, size_t length, loff_t * ofs)
{
	int ret;
	
	ret = copy_from_user(piezo_data,buf, length);

	gfreq = piezo_data[0];

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
	.name = "drv_hw_piezo",
	.fops = &drv_hw_piezo_fops,
};


static int drv_hw_piezo_init(void)
{
	int piezo = gpio_request(PIEZO,"gpio piezo");

	if(piezo != 0 ) {
		printk("#### FAILED Request gpio %d error!! \n", piezo);
	}

	gpio_direction_output(PIEZO, LOW);

	return misc_register(&drv_hw_piezo_driver);
}

static void drv_hw_piezo_exit(void)
{
	gpio_set_value(PIEZO, HIGH);

	gpio_free(PIEZO);
	misc_deregister(&drv_hw_piezo_driver);
}

module_init(drv_hw_piezo_init);
module_exit(drv_hw_piezo_exit);

MODULE_AUTHOR("PlanX Studio");
MODULE_DESCRIPTION("drv_hw_piezo");
MODULE_LICENSE("Dual BSD/GPL");
