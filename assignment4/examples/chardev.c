/*
 *  chardev.c: Creates a read-only char device that says
 *             how many times you've opened it.
 */

#include <asm/uaccess.h> /* for copy_to_user */
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>

#define SUCCESS 0
#define DEVICE_NAME "chardev" /* Dev name as it appears in /proc/devices  */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergiusz Pawlowicz");
MODULE_DESCRIPTION("Open counter");

/*
 * Global variables are declared as static, so are global within the file.
 */
static int major_number; /* major number assigned to our device driver */
static bool device_opened = false; /* prevents simultaneous access to device */
static int how_often_opened = 0; /* count how often we have been opened */

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/chardev"
 */
static int device_open(struct inode *inode, struct file *file)
{
    if (device_opened) {
        return -EBUSY;
    }
    device_opened = true;
    how_often_opened++;

    /*
     * Increment the usage count, else the module could be removed
     * while a userspace process is using it!
     */
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
    device_opened = false; /* We're now ready for our next caller */

    /*
     * Decrement the usage count, or else once you opened the file
     * the kernel won't allow you to remove this module
     */
    module_put(THIS_MODULE);

    return 0;
}

/*
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp, /* see include/linux/fs.h  */
               char *buffer,      /* buffer to fill with data */
               size_t length,     /* length of the buffer  */
               loff_t *offset)    /* offset is zero on first call */
{
    const int MaxSize =
        length < 50 ? length
            : 50; /* make sure we copy at most length bytes */
    char msg[MaxSize];
    int msgLength = 0;
    int bytesRemaining = 0;

    if (*offset != 0) {
        /* as we write all data in one go, we have no more data */
        return 0;
    }

    msgLength = snprintf(msg, MaxSize, "I have been opened %d times!\n",
                 how_often_opened);
    bytesRemaining = copy_to_user(buffer, msg, msgLength);

    /* update how far I am with my data */
    *offset += msgLength - bytesRemaining;
    return msgLength - bytesRemaining;
}

/*
 * Called when a process writes to dev file: echo "hi" > /dev/chardev
 */
static ssize_t device_write(struct file *filp, const char *buff, size_t len,
                loff_t *off)
{
    printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
    return -EINVAL;
}

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/*
 * This function is called when the module is loaded
 */
int init_module(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n",
               major_number);
        return major_number;
    }

    printk(KERN_INFO "I was assigned major number %d. To talk to\n",
           major_number);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME,
           major_number);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{
    /*
     * Unregister the device
     */
    unregister_chrdev(major_number, DEVICE_NAME);

    /*
     * Please note: in Kernel versions up to 2.6.xx, unregister_chrdev
     * returns int, in those versions: please use the following code:
     *
     * int result = unregister_chrdev(major_number, DEVICE_NAME);
     * if (result < 0)
     * {
     *     printk(KERN_ALERT "Error in unregister_chrdev: %d\n", result);
     * }
     */
}
