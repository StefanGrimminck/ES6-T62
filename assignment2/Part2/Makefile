obj-m += sys-reader-1.o


.PHONY: clean

all: clean arm

x86:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

arm:
	make ARCH=arm CROSS_COMPILE=arm-linux- -C /home/student/felabs/sysdev/tinysystem/linux-2.6.34 M=$(PWD) modules

clean:
	make -C /home/student/felabs/sysdev/tinysystem/linux-2.6.34 M=$(PWD) clean

