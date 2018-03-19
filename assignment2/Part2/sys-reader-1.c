/*
 * sys-reader-1.c 
 * 
 * ES6 Peek & Poke assignment
 * Made by Stefan Grimminck & Skip Geldens
 * 
 * This is a kernel module that will read a file
 * on the /sys filesystem, and will return the 
 * contents of the registers that were asked for.
 * The module can also be used to write a value 
 * to a certain register.
 * 
 * The protocol is as follows:
 * To read 8 registers from address 0x400a8014:
 * r 400a8014 8
 * 
 * To write the value 0x3ff to register 0x400a8014:
 * w 400a8014 3ff
 * 
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/device.h>
#include <mach/hardware.h>

#define sysfs_dir  "es6"
#define sysfs_file "hw"

#define sysfs_max_data_size 1024 /* due to limitations of sysfs, you mustn't go above PAGE_SIZE, 1k is already a *lot* of information for sysfs! */
static char sysfs_buffer[sysfs_max_data_size+1]; /* an extra byte for the '\0' terminator */

/*
 * This function is called when a user wants to read the sys file
 */
static ssize_t
sysfs_show(struct device *dev,
           struct device_attribute *attr,
           char *buffer)
{
    printk(KERN_INFO "sysfile_read (/sys/kernel/%s/%s) called\n", sysfs_dir, sysfs_file);
    
    /*
     * Return the sysfs_buffer we first filled in the sysfs_store function
     */
    return sprintf(buffer, "%s", sysfs_buffer);
}

/*
 * This function is called when a user writes something to the sys file
 */
static ssize_t
sysfs_store(struct device *dev,
            struct device_attribute *attr,
            const char *buffer,
            size_t count)
{   
    char io = '';
    int i = 0;
    uint32_t addr = 0;
    uint32_t *regaddr = 0;
    int value = 0;
    size_t buflen = 0;
    static char temp_buffer[sizeof(uint32_t)]; /* Used to concat the value of a register to the sys file */
    sscanf(buffer, "%c %x %x", &io, &addr, &value);
    memset(sysfs_buffer, 0, sysfs_max_data_size);
    regaddr = io_p2v(addr);
	
	
	switch(io)
	{
	case 'r':	
		for (i = 0; i < value; i++)
		{
      		printk(KERN_INFO "Value of Register : %u\n", *(volatile uint32_t*)regaddr); /* print to the kernel log */
            sprintf(temp_buffer, "%u", *(uint32_t*)regaddr);
            buflen = strlen(sysfs_buffer);
            if((sysfs_max_data_size - buflen) > (strlen(temp_buffer) + 1)){
                strcat(sysfs_buffer, " ");
                strcat(sysfs_buffer, temp_buffer);
            }
            regaddr++;
		}
					
		break;
	case 'w':
		*(uint32_t*)regaddr = value;
        printk(KERN_INFO "Wrote: %u to address: %x", value, addr);
		break;
	default:	
		printk(KERN_INFO "Wrong input parameters");
        return -EINVAL;
		break;
		
	}

    return count;
}


/* 
 * This line is now changed: in the previous example, the last parameter to DEVICE_ATTR
 * was NULL, now we add a store function as well. We must also add writing rights to the file:
 * 
 * We also changes the first parameter of this macro from 'data' to 'hw'
 */
static DEVICE_ATTR(hw, S_IWUGO | S_IRUGO, sysfs_show, sysfs_store);


/*
 * because we changed the macro above, the struct is now: dev_attr_hw
 */
static struct attribute *attrs[] = {
    &dev_attr_hw.attr,
    NULL   /* need to NULL terminate the list of attributes */
};
static struct attribute_group attr_group = {
    .attrs = attrs,
};
static struct kobject *hello_obj = NULL;


int __init sysfs_init(void)
{
    int result = 0;
    hello_obj = kobject_create_and_add(sysfs_dir, kernel_kobj);
    if (hello_obj == NULL)
    {
        printk (KERN_INFO "%s module failed to load: kobject_create_and_add failed\n", sysfs_file);
        return -ENOMEM;
    }

    result = sysfs_create_group(hello_obj, &attr_group);
    if (result != 0)
    {
        /* creating files failed, thus we must remove the created directory! */
        printk (KERN_INFO "%s module failed to load: sysfs_create_group failed with result %d\n", sysfs_file, result);
        kobject_put(hello_obj);
        return -ENOMEM;
    }

    printk(KERN_INFO "/sys/kernel/%s/%s created\n", sysfs_dir, sysfs_file);
    return result;
}

void __exit sysfs_exit(void)
{
    kobject_put(hello_obj);
    printk (KERN_INFO "/sys/kernel/%s/%s removed\n", sysfs_dir, sysfs_file);
}

module_init(sysfs_init);
module_exit(sysfs_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Skip Geldens & Stefan Grimminck");
MODULE_DESCRIPTION("sysfs buffer");
