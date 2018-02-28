#include <linux/kernel.h>    /* We're doing kernel work */
#include <linux/module.h>    /* Specifically, a module */
#include <linux/kobject.h>   /* Necessary because we use sysfs */
#include <linux/device.h>
#include <mach/hardware.h>

#define sysfs_dir  "es6"
#define sysfs_file "hw"

#define sysfs_max_data_size 1024 /* due to limitations of sysfs, you mustn't go above PAGE_SIZE, 1k is already a *lot* of information for sysfs! */
static char sysfs_buffer[sysfs_max_data_size+1] = "HelloWorld!\n"; /* an extra byte for the '\0' terminator */
static ssize_t used_buffer_size = 0;

static ssize_t
sysfs_show(struct device *dev,
           struct device_attribute *attr,
           char *buffer)
{
    printk(KERN_INFO "sysfile_read (/sys/kernel/%s/%s) called\n", sysfs_dir, sysfs_file);
    
    /*
     * The only change here is that we now return sysfs_buffer, rather than a fixed HelloWorld string.
     */
    return sprintf(buffer, "%s", sysfs_buffer);
}

static ssize_t
sysfs_store(struct device *dev,
            struct device_attribute *attr,
            const char *buffer,
            size_t count)
{   
    char io = 'r';
    uint32_t addr = 0;
    int value = 0;
    sscanf(buffer, "%c 0x%x %d", &io, &addr, &value);
	printk(KERN_INFO "Read or Write: %c\n", io);
	printk(KERN_INFO "Register: 0x%x\n", addr);
	printk(KERN_INFO "Value: %d\n", value);
	uint32_t *regval = io_p2v(addr);
	
	
	switch(io)
	{
	case 'r':	
		printk(KERN_INFO "Hardware Address: %d\n", addr); 
		printk(KERN_INFO "Value of Address: %d\n", *(uint32_t*)regval);			
		break;
	case 'w':
		*(uint32_t*)regval = value;
		break;
	default:	
		printk(KERN_INFO "Wrong Value");
		break;
		
	}

    return count;
}


/* 
 * This line is now changed: in the previous example, the last parameter to DEVICE_ATTR
 * was NULL, now we add a store function as well. We must also add writing rights to the file:
 */
static DEVICE_ATTR(data, S_IWUGO | S_IRUGO, sysfs_show, sysfs_store);


/*
 * This is identical to previous example.
 */
static struct attribute *attrs[] = {
    &dev_attr_data.attr,
    NULL   /* need to NULL terminate the list of attributes */
};
static struct attribute_group attr_group = {
    .attrs = attrs,
};
static struct kobject *hello_obj = NULL;


int __init sysfs_init(void)
{
    int result = 0;

    /*
     * This is identical to previous example.
     */
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
