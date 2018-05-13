#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <mach/hardware.h>
#include "gpio_ports.h"
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>



/* SYSFS variables */
#define sysfs_dir "buffer"
#define sysfs_file "data"
#define sysfs_max_data_size 1024
static char sysfs_buffer[sysfs_max_data_size + 1] = "GPIO Assignment";
static ssize_t used_buffer_size = 0;

/* DEVFS variables */
#define DEVICE_NAME "chardev"
#define BUF_SIZE 100 /* maximum size of message buffer */
static char msg[BUF_SIZE];
static char *msg_ptr;
static int major_number;
static int minor_number;
static bool device_opened = false;
Pinfo port;

/* Module information */
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Skip Geldens & Stefan Grimminck");
MODULE_DESCRIPTION("GPIO Assignment");

/* Structure for mapping jumper pins to their corresponding registers */
static  Pinfo PinInfo[] = {
	{ 49, 1, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, 8},
	{ 50, 1, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, 9},
	{ 51, 1, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, 10},
	{ 52, 1, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, 11},
	{ 53, 1, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, 12},
	{ 27, 1, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, 7},
	{ 24, 2, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, 1},
	{ 11, 2, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, 2},
	{ 12, 2, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, 3},
	{ 13, 2, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, 4},
	{ 14, 2, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, 5},
	{ 33, 4, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, 6},
	{ 47, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, 0},
	{ 56, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, 1}, 
	{ 48, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, 2},
	{ 57, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, 3},
	{ 49, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, 4},
	{ 58, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, 5},
	{ 50, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, 6},
	{ 45, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {-1, P2_OUTP_SET, P2_OUTP_CLR}, {P2_INP_STATE}, 7},
	{ 40, 4, {P0_DIR_SET, P0_DIR_CLR, 1}, {-1, P0_OUTP_SET, P0_OUTP_CLR}, {P0_INP_STATE}, 0},
	{ 24, 1, {P2_DIR_SET, P2_DIR_CLR, 1}, {P3_OUTP_STATE, P3_OUTP_SET, P3_OUTP_CLR}, {P3_INP_STATE}, 30},
	{ 54, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {P3_OUTP_STATE, P3_OUTP_SET, P3_OUTP_CLR}, {P3_INP_STATE}, 25},
	{ 46, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {P3_OUTP_STATE, P3_OUTP_SET, P3_OUTP_CLR}, {P3_INP_STATE}, 26},
	{ 36, 3, {P2_DIR_SET, P2_DIR_CLR, 1}, {P3_OUTP_STATE, P3_OUTP_SET, P3_OUTP_CLR}, {P3_INP_STATE}, 29},

};


Pinfo pinToPort(int pin, int jumper)
{	
	int size, i = 0;
	Pinfo error = {-1, -1, {-1, -1, -1}, {-1, -1}, {-1}, -1};
	
	if (jumper < 1 || jumper > 3) { return error;}
	
	size = sizeof(PinInfo)/sizeof(PinInfo[0]);
	
	for (i = 0; i < size; i++) 
	{
		if (PinInfo[i].pin == pin && PinInfo[i].jumper == jumper) 
		{
			return PinInfo[i];
		}
	}
	
	return error;	
}

/* #################################################
 * ########### DEVFS ASSIGNMENT GPIO ###############
 * #################################################
 */
 
 
 static int device_open(struct inode *inode, struct file *file) {

    if (device_opened) { return -EBUSY; }

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

    int bytes_remaining, msg_length = 0;
    unsigned int direction, state = 0;
    
    minor_number = (int) filp->private_data;

    if (*offset != 0) { return 0; }
    
    /* Gathering information about the pins its current state */
    direction = *(unsigned int*)(io_p2v(port.dir.state)) & PIN_TO_BIT(port.LOC_IN_REG);
    state = *(unsigned int*)(io_p2v(port.input.state)) & PIN_TO_BIT(port.LOC_IN_REG);
    
    /* Printing gathered information back to user */
	msg_length = sprintf(msg_ptr, "Pin: %d on jumper %d has been set to direction %d, with state %d", port.pin, port.jumper, direction, state);
 

    bytes_remaining = copy_to_user(buffer, msg, msg_length);
    *offset += msg_length - bytes_remaining;
    
    return msg_length - bytes_remaining;
}

static ssize_t device_write(struct file *filp, char *buffer, size_t length, loff_t *offset) {

	int i, bytes_remaining, jumper, pin = 0;
    char command = '0';
	Pinfo port2get;
	unsigned int address;
	
    minor_number = (int) filp->private_data;

    for (i = 0; i < length && i < BUF_SIZE; i++) {
        get_user(msg[i], buffer + i);
    }

    msg_ptr = msg;
    bytes_remaining =  copy_from_user(buffer, msg, BUF_SIZE);

    if (sscanf(msg_ptr, "%c %d %d", &command, &jumper, &pin) != 3) {
        printk(KERN_ERR "Wrong input.");
        return -EINVAL;        
    }
	port2get = pinToPort(pin, jumper);
	
	if (port2get.pin < 1){ return -1; }
	
	switch(command) {
		case 'r' :
			port = port2get;
		break;
		case 'h' :
			address = PIN_TO_BIT(port.LOC_IN_REG);
			memcpy(io_p2v(port.output.set),&address, sizeof(unsigned int));	 
			printk(KERN_INFO "Pin: %d on jumper: %d is set", pin, jumper);
		break;
		case 'l' :
			address = PIN_TO_BIT(port.LOC_IN_REG);
			memcpy(io_p2v(port.output.clr),&address, sizeof(unsigned int));
			printk(KERN_INFO "Pin: %d on jumper: %d is cleared", pin, jumper);
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

/* #################################################
 * ########### SYSFS ASSIGNMENT GPIO ###############
 * #################################################
 */
 
void gpio_init_ports(void) {
    *(unsigned int*)(io_p2v(P2_MUX_SET)) = 8;
    *(unsigned int*)(io_p2v(LCDCLK_CTRL)) = 0;
}


static ssize_t sysfs_show(struct device *dev, struct device_attribute *attr, char *buffer)
{
  printk(KERN_INFO "sysfile_read (/sys/kernel/%s/%s) called\n", sysfs_dir,
         sysfs_file);
         
  return sprintf(buffer, "%s", sysfs_buffer);
}

static ssize_t sysfs_store(struct device *dev, struct device_attribute *attr,
         const char *buffer, size_t count)
{
	unsigned char io;
    unsigned int pin;
    unsigned int jumper;
    Pinfo pinformatie;
    
   used_buffer_size = count > sysfs_max_data_size ? sysfs_max_data_size : count; /* handle MIN(used_buffer_size, count) bytes */
    
    if( sscanf(buffer,"%c %d %d", &io, &pin, &jumper) != 3){
		
        printk(KERN_WARNING "User input was not correct. \n Use format: [i/o] pin[0 - x] Jumper[1 - 3]");
        return -EINVAL;
    }
    
    
    pinformatie = pinToPort(pin, jumper);
    
    if (pinformatie.pin < 0){
		
		printk(KERN_WARNING "Non-existent pin, jumper or combination of those two.");
		return -EINVAL;
	}

    if (io == 'i')
    {
		*(unsigned int*)(io_p2v(pinformatie.dir.clr)) = PIN_TO_BIT(pinformatie.LOC_IN_REG);
        printk(KERN_INFO "Pin: %d on Jumper %d has been set to input", jumper, pin);

    }
    else if (io == 'o')
    {
        *(unsigned int*)(io_p2v(pinformatie.dir.set)) =  PIN_TO_BIT(pinformatie.LOC_IN_REG);
        printk(KERN_INFO "Pin: %d on Jumper %d has been set to output", jumper, pin);
    }
    else {
        printk(KERN_WARNING "Invalid input. Choose [i]nput or [o]utput");
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
	gpio_init_ports();
  
  
   
  return result;
}

void __exit mod_exit(void)
{
  kobject_put(hello_obj);
  unregister_chrdev(major_number, DEVICE_NAME);
  
  printk(KERN_INFO "/sys/kernel/%s/%s removed\n", sysfs_dir, sysfs_file);
  printk(KERN_INFO "unregistered character device: %s", DEVICE_NAME);
}

module_init(mod_init);
module_exit(mod_exit);
