#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <mach/hardware.h>
#include "UM10326.h"

#define DEVICE_NAME "chardev"
#define BUF_SIZE 100 /* maximum size of message buffer */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Skip Geldens & Stefan Grimminck");
MODULE_DESCRIPTION("Controlling PWM signal");


static char msg[BUF_SIZE];
static char *msg_ptr;
static int major_number;
static int minor_number;
static bool device_opened = false;
static int pwm1;
static int pwm2;

static int device_open(struct inode *inode, struct file *file) {

    if (device_opened) {
        return -EBUSY;
    }

    minor_number = MINOR(inode->i_rdev);
    file->private_data = (void *) minor_number;
    device_opened = true;
    msg_ptr = msg;

    return 0;
}

static int device_release(struct inode *inode, struct file *file) {

    device_opened = false;

    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset) {

    int bytes_remaining = 0;
    int msg_length = 0;

    if (*offset != 0) {
        return 0;
    }

    minor_number = (int) filp->private_data;
    pwm1 = *(unsigned int *) (io_p2v(PWM1_CONTR_REG));
    pwm2 = *(unsigned int *) (io_p2v(PWM2_CONTR_REG));


    switch (minor_number) {
        case 0:
            msg_length = sprintf(msg_ptr, "PWM1 state: %s", get_pwm_state(pwm1) ? "enabled" : "disabled\n");
            break;
        case 1: {
            int reload_val = get_reload_value(pwm1);
            if (reload_val) {
                msg_length = sprintf(msg_ptr, "PWM1 reload value : %d\n", reload_val);
            } else {
                msg_length = sprintf(msg_ptr, "PWM1 reload value : 0\n");
            }
            break;
        }
        case 2: {
            int duty_cycle_val = get_duty_cycle(pwm1);
            if (duty_cycle_val) {
                msg_length = sprintf(msg_ptr, "PWM1 duty cycle: %d\n", duty_cycle_val);
            } else {
                msg_length = sprintf(msg_ptr, "PWM1 duty cycle: 0\n");
            }
            break;
        }
        case 3:
            msg_length = sprintf(msg_ptr, "PWM2 state: %s", get_pwm_state(pwm2) ? "enabled" : "disabled\n");
            break;
        case 4: {
            int reload_val = get_reload_value(pwm2);
            if (reload_val) {
                msg_length = sprintf(msg_ptr, "PWM2 reload value : %d\n", reload_val);
            } else {
                msg_length = sprintf(msg_ptr, "PWM2 reload value : 0\n");
            }
            break;
        }
        case 5: {
            int duty_cycle_val = get_duty_cycle(pwm2);
            if (duty_cycle_val) {
                msg_length = sprintf(msg_ptr, "PWM1 duty cycle: %d\n", duty_cycle_val);
            } else {
                msg_length = sprintf(msg_ptr, "PWM1 duty cycle: 0\n");
            }
            break;
        }
        default:
            msg_length = sprintf(msg_ptr, "Node doesn't exist\n");
            break;
    }

    bytes_remaining = copy_to_user(buffer, msg, msg_length);
    *offset += msg_length - bytes_remaining;
    return msg_length - bytes_remaining;
}

static ssize_t device_write(struct file *filp, char *buffer, size_t length, loff_t *offset) {

    int write_val = -1;
    int i = 0;
    int bytes_remaining = 0;
    minor_number = (int) filp->private_data;
    pwm1 = *(unsigned int *) (io_p2v(PWM1_CONTR_REG));
    pwm2 = *(unsigned int *) (io_p2v(PWM2_CONTR_REG));

    for (i = 0; i < length && i < BUF_SIZE; i++) {
        get_user(msg[i], buffer + i);
    }

    msg_ptr = msg;
    bytes_remaining =  copy_from_user(buffer, msg, BUF_SIZE);
    sscanf(msg_ptr, "%d", &write_val);


    switch (minor_number) {
        case 0: {
            if (write_val) {
                *(unsigned int *) (io_p2v(PWM1_CONTR_REG)) |= 1 << PWM_SHIFT_ENABLE;
            } else {
                *(unsigned int *) (io_p2v(PWM1_CONTR_REG)) &= ~1 << PWM_SHIFT_ENABLE;
            }
            break;
        }
        case 1: {
            pwm1 = pwm1 & ~PWM_FREQ;
            *(unsigned int *) (io_p2v(PWM1_CONTR_REG)) = pwm1 | ((write_val << 8) & PWM_FREQ);

            break;
        }
        case 2: {

            pwm1 = pwm1 & ~PWM_DUTY;
            *(unsigned int *) (io_p2v(PWM1_CONTR_REG)) = pwm1 | (write_val & PWM_DUTY);


            break;
        }
        case 3: {

            if (write_val) {
                *(unsigned int *) (io_p2v(PWM2_CONTR_REG)) |= 1 << PWM_SHIFT_ENABLE;
            } else {
                *(unsigned int *) (io_p2v(PWM2_CONTR_REG)) &= ~1 << PWM_SHIFT_ENABLE;
            }

            break;
        }
        case 4: {
            pwm1 = pwm1 & ~PWM_FREQ;
            *(unsigned int *) (io_p2v(PWM2_CONTR_REG)) = pwm2 | ((write_val << 8) & PWM_FREQ);
            break;
        }
        case 5: {
            pwm2 = pwm2 & ~PWM_DUTY;
            *(unsigned int *) (io_p2v(PWM2_CONTR_REG)) = pwm2 | (write_val & PWM_DUTY);
            break;
        }
        default:
            sprintf(msg_ptr, "Node doesn't exist");
            break;
    }


    return length - bytes_remaining;
}

static struct file_operations fops = {
        .read = device_read,
        .write = device_write,
        .open = device_open,
        .release = device_release
};

int init_module(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0) {
        printk(KERN_ALERT
        "Registering char device failed with %d\n",
                major_number);
        return major_number;
    }
    printk(KERN_INFO
    "I was assigned major number %d. To talk to\n",
            major_number);

    *(unsigned int *) (io_p2v(PWMCLOCK_REG)) = PWMCLOCK;
    *(unsigned int *) (io_p2v(LCDCONFIG_REG)) = LCDCONFIG;


    return 0;
}

int get_reload_value(int pwm_reg) {

    int pwm_reload = ((pwm_reg & PWM_FREQ) >> 8);
    if (pwm_reload == 0) {
        return 0;
    }
    return (PWM_FREQ / pwm_reload) / 512;
}

int get_pwm_state(int pwm_reg) {
    return pwm_reg & PWM_EN;
}

int get_duty_cycle(int pwm_reg) {
    int pwm_duty = (pwm_reg & PWM_DUTY);

    return (pwm_duty * 100) / 256;
}

void cleanup_module(void) {
    unregister_chrdev(major_number, DEVICE_NAME);
}
