#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <mach/hardware.h>
#include "gpio_ports.h"
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>

/* SYSFS variables */
#define sysfs_dir "gpio"
#define sysfs_file "data"
#define sysfs_max_data_size 1024
static char sysfs_buffer[sysfs_max_data_size + 1] = "GPIO Assignment";
static ssize_t used_buffer_size = 0;

/* DEVFS variables */
#define DEVICE_NAME "chardev"
#define BUF_SIZE 100 /* maximum size of message buffer */
static int major_number;
Pinfo port;

/* Module information */
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Skip Geldens & Stefan Grimminck");
MODULE_DESCRIPTION("GPIO Assignment");

/* Structure for mapping connector pins to their corresponding registers */
static  Pinfo PinInfo[] = {
	{ 49, 1, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, {8, 8}},	
	{ 50, 1, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, {9, 9}},
	{ 51, 1, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, {10, 10}},
	{ 52, 1, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, {11, 11}},
	{ 53, 1, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, {12, 12}},
	{ 27, 1, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, {7, 7 }},
	{ 24, 2, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, {1, 1 }},
	{ 11, 2, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, {2, 2 }},
	{ 12, 2, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, {3, 3 }},
	{ 13, 2, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, {4, 4 }},
	{ 14, 2, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, {5, 5 }},
	{ 33, 3, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, {6, 6 }},
	{ 47, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, {0, 0 }},
	{ 56, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, {1, 1 }}, 
	{ 48, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, {2, 2 }},
	{ 57, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, {3, 3 }},
	{ 49, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, {4, 4 }},
	{ 58, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, {5, 5 }},
	{ 50, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, {6, 6 }},
	{ 45, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, {7, 7 }},
	{ 40, 3, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, {0, 0 }},
	{ 24, 1, {P2_DIR_SET, P2_DIR_CLR, 1}, {P3_OUTP_STATE, P3_OUTP_SET, P3_OUTP_CLR}, {P3_INP_STATE}, {30, 24}},
	{ 54, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {P3_OUTP_STATE, P3_OUTP_SET, P3_OUTP_CLR}, {P3_INP_STATE}, {25, 10}},
	{ 46, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {P3_OUTP_STATE, P3_OUTP_SET, P3_OUTP_CLR}, {P3_INP_STATE}, {26, 11}},
	{ 36, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {P3_OUTP_STATE, P3_OUTP_SET, P3_OUTP_CLR}, {P3_INP_STATE}, {29, 14}},
};

typedef struct DriverInformation{
	char msg[BUF_SIZE];
	int minor;
}DriverInfo;

Pinfo pinToPort(int pin, int connector)
{	
	int size, i = 0;
	Pinfo error = {-1, -1, {-1, -1, -1}, {-1, -1}, {-1}, {-1, -1}};
	
	if (connector < 1 || connector > 3) { return error;}
	
	size = sizeof(PinInfo)/sizeof(PinInfo[0]);
	
	for (i = 0; i < size; i++) 
	{
		if (PinInfo[i].pin == pin && PinInfo[i].connector == connector) 
		{
			return PinInfo[i];
		}
	}
	
	return error;	
}

 static int device_open(struct inode *inode, struct file *file) {
	DriverInfo* info;
    file->private_data = kmalloc(sizeof(DriverInfo), GFP_KERNEL);
    info = (DriverInfo*)file->private_data;
    info->minor = MINOR(inode->i_rdev);

    try_module_get(THIS_MODULE);

    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
	kfree(file->private_data);
	module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset) {
	DriverInfo* info;
    int bytes_remaining, msg_length = 0;
    unsigned int state = 0;
    info = (DriverInfo*)filp->private_data;

    if (*offset != 0) { return 0; }
    
    if (port.pin < 0) 
    {
		 printk(KERN_ERR "No selected port.\n");
		 return -EINVAL;
	}
    if(info->minor == 0){
		state = *(unsigned int*)(io_p2v(port.input.state)) & PIN_TO_BIT(port.loc_in_reg.outputbit);

		/* Printing gathered information back to user */
		msg_length = sprintf(info->msg, "Pin: %d on connector %d has value %d\n", port.pin, port.connector, state);
	}
	else{
		printk(KERN_ERR "Only use minor number 0. \n");
		return 0;
	}

    bytes_remaining = copy_to_user(buffer, info->msg, msg_length);
    *offset += msg_length - bytes_remaining;
    
    return msg_length - bytes_remaining;
}

static ssize_t device_write(struct file *filp, char *buffer, size_t length, loff_t *offset) {
	
	DriverInfo* info;
	int i, bytes_remaining, connector, pin = 0;
    char command = '0';
	Pinfo port2get;
	unsigned int bit;
	
	info = (DriverInfo*)filp->private_data;

    for (i = 0; i < length && i < BUF_SIZE; i++) {
        get_user(info->msg[i], buffer + i);
    }
    
    if(info->minor != 0){
		printk(KERN_ERR "Only use minor number 0. \n");
		return -EINVAL;
	}
    
    bytes_remaining =  copy_from_user(buffer, info->msg, BUF_SIZE);

    if (sscanf(info->msg, "%c %d %d", &command, &connector, &pin) != 3) {
        return -EINVAL;        
    }
    
	port2get = pinToPort(pin, connector);
	
	if (port2get.pin < 1)
	{ 
		printk(KERN_ERR "Pin not found!\n");
		return -EINVAL; 
	}
	
	switch(command) {
		case 'r' :
			port = port2get;
		break;
		case 'h' :
			bit = PIN_TO_BIT(port2get.loc_in_reg.outputbit);
			printk(KERN_INFO "BIT: %u\n", bit);
			printk(KERN_INFO "INSIDE REG: 0x%x\n", port2get.output.set);
			
			memcpy(io_p2v(port2get.output.set),&bit, sizeof(unsigned int));	 
			printk(KERN_INFO "Pin: %d on connector: %d is set\n", pin, connector);
		break;
		case 'l' :
			bit = PIN_TO_BIT(port2get.loc_in_reg.outputbit);
			printk(KERN_INFO "BIT: %u\n", bit);
			printk(KERN_INFO "INSIDE REG: 0x%x\n", port2get.output.clr);
			
			memcpy(io_p2v(port2get.output.clr),&bit, sizeof(unsigned int));
			printk(KERN_INFO "Pin: %d on connector: %d is cleared\n", pin, connector);
		break;
		default :
			printk(KERN_ERR "Invalid command");
		return -EINVAL;
	}

	return length - bytes_remaining;
}

static struct file_operations fops = {
        .read = device_read,
        .write = device_write,
	    .open = device_open,
        .release = device_release
};
 

static ssize_t sysfs_show(struct device *dev, struct device_attribute *attr, char *buffer)
{
	printk(KERN_INFO "Set pin state: \n echo \"[i/o] [pin] [connector]\" > /sys/kernel/gpio/data\n\n Set pin value or read:\n echo \"[r/l/h] [pin] [connector]\" > /dev/gpio\n");

  return sprintf(buffer, "%s", sysfs_buffer);
}

static ssize_t sysfs_store(struct device *dev, struct device_attribute *attr, const char *buffer, size_t count)
{
	unsigned char io;
    unsigned int pin, connector;
    Pinfo pinformatie;
    
   used_buffer_size = count > sysfs_max_data_size ? sysfs_max_data_size : count; 
    
    if( sscanf(buffer,"%c %d %d", &io, &connector, &pin) != 3){
		
        printk(KERN_WARNING "User input was not correct. \n Use format: [i/o] pin[0 - x] connector[1 - 3]\n");
        return -EINVAL;
    }
    
    
    pinformatie = pinToPort(pin, connector);
    
    if (pinformatie.pin < 0){
		
		printk(KERN_WARNING "Non-existent pin, connector or combination of those two.\n");
		return -EINVAL;
	}

    if (io == 'i')
    {
		*(unsigned int*)(io_p2v(pinformatie.dir.clr)) = PIN_TO_BIT(pinformatie.loc_in_reg.inputbit);
        printk(KERN_INFO "Pin: %d on connector %d has been set to input\n", connector, pin);

    }
    else if (io == 'o')
    {
        *(unsigned int*)(io_p2v(pinformatie.dir.set)) =  PIN_TO_BIT(pinformatie.loc_in_reg.outputbit);
        printk(KERN_INFO "Pin: %d on connector %d has been set to output\n", pin, connector);
    }
    else {
        printk(KERN_WARNING "Invalid input. Choose [i]nput or [o]utput\n");
        return -EINVAL;
    }

    used_buffer_size = count > sysfs_max_data_size ? sysfs_max_data_size : count;
    memcpy(sysfs_buffer, buffer, used_buffer_size);
    sysfs_buffer[used_buffer_size] = '\0';

    return used_buffer_size;
}


static DEVICE_ATTR(data, S_IWUGO | S_IRUGO, sysfs_show, sysfs_store);

static struct attribute *attrs[] = {
	&dev_attr_data.attr, NULL 
};

static struct attribute_group attr_group = {.attrs = attrs,};
static struct kobject *hello_obj = NULL;

int __init mod_init(void)
{
	int result = 0;
  
	/* DEVFS */
	major_number = register_chrdev(0, DEVICE_NAME, &fops);

	if (major_number < 0) {
		printk(KERN_ALERT "Registering char device failed with %d\n", major_number);
		return major_number;
	}
	
    printk(KERN_INFO"I was assigned major number %d\n", major_number);
	
	/* SYSFS */
	hello_obj = kobject_create_and_add(sysfs_dir, kernel_kobj);
	if (hello_obj == NULL) {
	printk(KERN_WARNING "%s module failed to load: kobject_create_and_add failed\n", sysfs_file);
	return -ENOMEM;
	}

	result = sysfs_create_group(hello_obj, &attr_group);
	
	if (result != 0) {
	printk(KERN_WARNING "%s module failed to load: sysfs_create_group " "failed with result %d\n", sysfs_file, result);
	kobject_put(hello_obj);
	return -ENOMEM;
	}

	printk(KERN_INFO "/sys/kernel/%s/%s created\n", sysfs_dir, sysfs_file);
	
	/* Disabling LCD */
	*(unsigned int*)(io_p2v(LCDCLK_CTRL)) = 0;
   
  return result;
}

void __exit mod_exit(void)
{
  kobject_put(hello_obj);
  unregister_chrdev(major_number, DEVICE_NAME);
  
  printk(KERN_INFO "/sys/kernel/%s/%s removed\n", sysfs_dir, sysfs_file);
  printk(KERN_INFO "unregistered character device: %s\n", DEVICE_NAME);
}

module_init(mod_init);
module_exit(mod_exit);
