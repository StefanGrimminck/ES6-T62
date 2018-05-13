obj-m += gpio.o 

.PHONY: clean

all:
	make ARCH=arm CROSS_COMPILE=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-linux- -C /home/student/felabs/sysdev/tinysystem/linux-2.6.34 M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean