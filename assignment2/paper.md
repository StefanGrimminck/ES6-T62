# Peek and poke
Stefan Grimminck & Skip Geldens
T62
## Part 1 

We set out to make an application that could read registers from user-space. We wanted to see what the results were, because we believe that this is not possible. 

https://www.embedded.com/design/programming-languages-and-tools/4432746/Device-registers-in-C
https://www.infradead.org/~mchehab/kernel_docs/unsorted/rtc.html



The program above is what we used to read the data on a register. The register that we need for the RTC on the LPC is: 0x40024000. This information was in the LPC3250 datasheet, on page 34. 

This program gives a Segmentation fault on the Lubuntu image, but gives out a seemingly random number on the LPC. However, this number is fixed, even if we reboot the LPC. The reason that this is happening is that we do have some access to the registers that are outside of our address space. But if we change the address on this line:
uint32_t* info = (uint32_t*)0x40024000;
To 400240000 (so without 0x in front) we do get a segmentation fault. So the LPC3250 clearly has a MMU, because we are restricted when we try to read at this address. In the LPC datasheet there is also a short mention of the MMU, but that does not give a answer on why we can read from an address that isn’t ours. 

## Part 2
After we'd gathered the basic knowledge of kernel modules by read LKMPG we created a kernel module to read and write to the proc filesystem the correct way. 

Before we started writing our module we first needed to know more about what a module is and how it is used in Linux. … .. ...
< Basic understanding of kernel module structure >

The basic structure of a kernel module is practically always the same, these are the things that should be in every module:

-Includes:
linux/module.h
linux/kernel.h

-An initializing function
	This runs when the module is inserted into the running system with insmod.
-A cleanup function
	This function runs when the module is removed from the running system with rmmod.

There are some specific functions and macros you use when using the /sys filesystem, we will explain them here.

sysfs_store()  (this can be any name, as long as you register it)

This function is called when a user writes something to the file you have defined and made in the /sys filesystem. So when a user or application wants to contact your kernel module, this is going to be done via the /sys filesystem in our case. There are also different ways of talking to the kernel, for example via the /proc filesystem or via /dev. 

In this function the kernel module should handle the input of the user, and do something with it. In our case it reads or writes some user specified registers.

sysfs_show() (this can be any name, as long as you register it)

This function will return something to the /sys file when a user wants to read from it. In our case it just prints a buffer that we filled in the sysfs_store function.


This line of code above is a macro for sysfs, this populates a struct with the following parameters:
-name
-mode
-the show function (sysfs_show in our case)
-the store function (sysfs_store in our case)

This snippet shows a struct that contains multiple of the before mentioned device attributes. This is done so that one kernel module can have multiple files in the /sys filesystem. 


This code in the init function of the module eventually creates the files in the /sys filesystem. You can see two functions, the kobject_create_and_add() and sysfs_create_group(). 

The kobject_create_and_add() function makes a kobject struct, and registers it with the sysfs. The name (first argument) is what gives us a directory in the sysfs where we can create different files in. 
(https://www.kernel.org/doc/html/latest/driver-api/basics.html?highlight=kobject_create#c.kobject_create_and_add)


The sysfs_create_group() function takes the kernel object we just created, and fills it the attr_group struct. This looks to be the same as using sysfs_create_file(), but for multiple files at once! 

After we have initialised the sysfs the module can do its work!

(https://www.kernel.org/pub/linux/kernel/people/mochel/doc/papers/ols-2005/mochel.pdf) 


< Snippet reading the registers >
< Description reading registers >


< Snippet writing to the registers >
< Description reading registers >

< How we tested our module (screens) >








https://www.kernel.org/doc/Documentation/filesystems/sysfs.txt

