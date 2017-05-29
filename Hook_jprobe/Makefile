#change with your own kernel path
KERNEL := 
#change with your own toolchain
TOOLCHAIN := 

SRC :=hook_jprobe.c
hook-objs := hook_jprobe.o
obj-m := hook.o

all:
	make ARCH=arm CROSS_COMPILE=$(TOOLCHAIN)bin/arm-linux-androideabi- -C $(KERNEL) M=$(shell pwd) CFLAGS_MODULE=-fno-pic modules
clean:
	make -C $(KERNEL) M=$(shell pwd) clean

