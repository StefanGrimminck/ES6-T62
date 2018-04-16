#include <asm/uaccess.h> /* for copy_to_user */
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include "UM10326.h"

#define DEVICE_NAME "chardev" 

static int max_size;				/* maximum size of message buffer */
static char msg[MaxSize];			/* message buffer */
static int major_number; 			/* major number assigned to our device driver */
static int minor_number;			/* minor number */
static bool device_opened = false; 	/* prevents simultaneous access to device */


static int device_open(struct inode *inode, struct file *file)
{
    if (device_opened) {
        return -EBUSY;
    }
    
    minor_number = MINOR(inode->i_rdev);
    file->private_data = (void*)minor_number;
    device_opened = true;

    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
	
    device_opened = false;

    return 0;
}


static ssize_t device_read(struct file *filp, /* see include/linux/fs.h  */
               char *buffer,      /* buffer to fill with data */
               size_t length,     /* length of the buffer  */
               loff_t *offset)    /* offset is zero on first call */
{
    if (*offset != 0) {
        /* as we write all data in one go, we have no more data */
        return 0;
    }
    
    int minor = (int)filp->private_data;
	int pwm1_reg = *(unsigned int*)(io_p2v(REG_PWM1));
	int pwm2_reg = *(unsigned int*)(io_p2v(REG_PWM2));
	
		switch(minor) {
		case 0: 
		/* Read PWM1 state */
			//printf(Message_Ptr, "PWM1 state: %s", get_pwm_state(pwm1_reg) ? "enabled" : "disabled");
		break;
		case 1:
		/* Get reload value PWM 1*/
		case 2:
		/* Get duty cycle PWM1 */
		break;
		case 3:
		/* Read PWM2 state */
			//printf(Message_Ptr, "PWM2 state: %s", get_pwm_state(pwm2_reg) ? "enabled" : "disabled");	
		break;
		case 4:
		/* Get reload value PWM 2*/
			GET_RELOAD_VALUE(PWM2_CONTR_REG)
		break;
		case 5:
		/* Get duty cycle PWM2 */
		break;
    
    
    
	return 0;
}

/*
 * Called when a process writes to dev file: echo "hi" > /dev/chardev
 */
static ssize_t device_write(struct file *filp, const char *buff, size_t len,
                loff_t *off)
{
 //TODO: Implement writing
 
 return 0;
}

static struct file_operations fops = {
    .read 		= device_read,
    .write 		= device_write,
    .open 		= device_open,
    .release 	= device_release
};


int init_module(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n",
               major_number);
        return major_number;
    }
    return 0;
}


void cleanup_module(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
}

get_pwm_state(int pwm_reg)
{
	return pwm_reg & PWM1_EN;
}

get_duty_cycle(int dutyvalue)
{
	return (dutyvalue * 100) /256;
}

get_reload_value(int pwm_reg)
{
	return ((pwm_reg & PWM_FREQ) >> 8
}





MODULE_LICENSE("GPL");
MODULE_AUTHOR("Skip Geldens & Stefan Grimminck");
MODULE_DESCRIPTION("Controlling PWM signal");
