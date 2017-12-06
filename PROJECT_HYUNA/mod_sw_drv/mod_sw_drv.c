#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/gpio.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

#include <linux/fs.h>
#include <linux/miscdevice.h>

#include <asm/uaccess.h>

#include <asm/siginfo.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>

int pid = 0;
int sw_stat;
struct task_struct *sw_task;

#define SIG_TEST 44

#define JOG_UP      5
#define JOG_DOWN    6
#define JOG_LEFT   16
#define JOG_RIGHT  20
#define JOG_CENTER 21

static unsigned int sw_irq[] = {
	JOG_UP,       // irq:171
	JOG_DOWN,     // irq:172
	JOG_LEFT,     // irq:182 
	JOG_RIGHT,    // irq:186
	JOG_CENTER,   // irq:187
};

static void sw_send_signal(int sw){

	int ret;
	struct siginfo info;
	struct task_struct *t;

	//printk("%s : sw = %d\n",__FUNCTION__,sw);	

	if(pid == 0) return;
	
	memset(&info, 0, sizeof(struct siginfo));
	info.si_signo = SIG_TEST;
	info.si_code = SI_QUEUE;

	rcu_read_lock();
	t = pid_task(find_pid_ns(pid, &init_pid_ns), PIDTYPE_PID);
	if(t == NULL){
		printk("no such pid\n");
		rcu_read_unlock();
		return;
	}
	rcu_read_unlock();

	info.si_int = sw;

	ret = send_sig_info(SIG_TEST, &info, t);
	if(ret < 0){
		printk("error sending signal\n");
	}
}

void sw_tasklet_func(unsigned long data){
	//printk("%s", __FUNCTION__);
	mdelay(5);
	sw_send_signal(data);
}

DECLARE_TASKLET(sw_tasklet, sw_tasklet_func, 0);

irqreturn_t sw_isr(int irq, void *unuse) {
	int i;

	for(i = 0; i < ARRAY_SIZE(sw_irq); ++i){
		if(sw_irq[i] == irq){
			sw_stat = i + 1;
			break;
		}
	}
			
	//printk("#### [%s], irq = %d\n", __FUNCTION__, irq);
	sw_tasklet.data = irq - 166;
	tasklet_schedule(&sw_tasklet);
	return IRQ_HANDLED;
}

static ssize_t mod_sw_drv_write(struct file * file, const char * buf, size_t length, loff_t * ofs)
{
	int ret;
	printk("%s\n",__FUNCTION__);	
	ret = copy_from_user(&pid, buf, length);
	if(ret != 0){
		printk("%s: pid = %d \n",__FUNCTION__, pid);
		return -EFAULT;
	}
	
	return 0;
}

static int mod_sw_drv_release(struct inode * inode, struct file * file)
{
	printk("%s\n",__FUNCTION__);
	pid = 0;	
	return 0;
}

static struct file_operations mod_sw_drv_fops =
{
	.owner = THIS_MODULE,
	.release = mod_sw_drv_release,
	.write = mod_sw_drv_write,
};

static struct miscdevice mod_sw_drv_driver = 
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = "mod_sw_drv",
	.fops = &mod_sw_drv_fops,
};


static int mod_sw_drv_init(void)
{
	int ret;
	int i;

	for (i = 0; i < ARRAY_SIZE(sw_irq); i++) {
		sw_irq[i] = gpio_to_irq(sw_irq[i]);
		printk("sw_irq=%d\n", sw_irq[i]);
		ret = request_irq(sw_irq[i], sw_isr, IRQF_TRIGGER_RISING, "jog_sw irq", NULL);
		if(ret){
			printk("FAILED Request irq %d. error : %d \n", sw_irq[i], ret);
		}
	}
	if(ret) {
		return ret;
	}

	return misc_register(&mod_sw_drv_driver);
}

static void mod_sw_drv_exit(void)
{
	int i;

	tasklet_kill(&sw_tasklet);
	for (i = 0; i < ARRAY_SIZE(sw_irq); i++){
		free_irq(sw_irq[i], NULL);
	}

	misc_deregister(&mod_sw_drv_driver);
}

module_init(mod_sw_drv_init);
module_exit(mod_sw_drv_exit);

MODULE_AUTHOR("PlanX Studio");
MODULE_DESCRIPTION("mod_sw_drv");
MODULE_LICENSE("Dual BSD/GPL");
