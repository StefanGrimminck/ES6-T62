/*
 * proc-reader-1.c 
 * 
 * ES6 Peek & Poke assignment
 * Made by Stefan Grimminck & Skip Geldens
 * 
 * This is a kernel module that will read a file
 * on the /proc filesystem, and will return the 
 * contents of the registers that were asked for.
 * 
 * The protocol is as follows:
 * To read 8 registers from address 0x400a8014:
 * r 400a8014 8
 * 
 * To write the value 0x3ff to register 0x400a8014:
 * w 400a8014 3ff
 * 
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

// This is used to give a license to the module, and to
// not taint the kernel with this module.
MODULE_AUTHOR("Stefan Grimminck & Skip Geldens");
MODULE_DESCRIPTION("A driver for reading registers");
MODULE_LICENSE("MIT");

// The name of the proc node
#define procfs_name "regread"
#define procfs_max_size 1024

// The structure for the proc file
struct proc_dir_entry *proc_file;
static unsigned long procfs_buffer_size = 0;
static char procfs_buffer[procfs_max_size];

// This function is called when the /proc file is read from.
int procfile_read(char *buffer,
                char **buffer_location,
                off_t offset, int buffer_length, 
                int *eof, void *data)
                {
                    int ret;

                    printk(KERN_INFO "function procfile_read on /proc/%s \n", procfs_name);

                    if(offset > 0){
                        // Read finished
                        ret = 0;
                    }
                    else {
                        // TODO
                        // fill the buffer with the data the user wants
                        ret = sprintf(buffer, "Test data! \n");
                    }
                    return ret;
                }

// This function is called when the /proc file is written to.
int procfile_write(struct file *file, const char *buffer, unsigned long count,
                    void *data){
                        printk(KERN_INFO "Function procfile_write on /proc/%s \n", procfs_name);
                        // Get size of the /proc buffer
                        procfs_buffer_size = count;
                        if(procfs_buffer_size > procfs_max_size){
                            procfs_buffer_size = procfs_max_size;
                        }

                        printk(KERN_INFO, "Size of procfile buffer: %l \n", procfs_buffer_size);
                        return procfs_buffer_size;
                    }

static int __init proc_reader_1_init(void){
    proc_file = create_proc_entry(procfs_name, 0644, NULL);

    if(proc_file == NULL){
        remove_proc_entry(procfs_name, NULL);
        printk(KERN_ALERT "Error: could not start the proc file /proc/%s \n", procfs_name);
        return -1;
    }

    proc_file->read_proc    = procfile_read;
    proc_file->write_proc   = procfile_write;
    //proc_file->owner        = THIS_MODULE;
    proc_file->mode         = S_IFREG | S_IRUGO;
    proc_file->uid          = 0;
    proc_file->gid          = 0;
    proc_file->size         = 37;

    printk(KERN_INFO "Procfs module created. \n");
    // Return 0 on success
    return 0;
}

static void __exit proc_reader_1_exit(void){
    remove_proc_entry(procfs_name, NULL);
    printk(KERN_INFO "Procfs module unloaded. \n");
}

module_init(proc_reader_1_init);
module_exit(proc_reader_1_exit);