#change with your own kernel path
KERNEL = /kernel/goldfish
#change with yout own toolchain path 
TOOLCHAIN = /androidsource/android-7.1.2_r3/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/



SRCS :=kernel_hook.c hook_fuc.c
hook-objs = kernel_hook.o 
obj-m :=hook.o
all:
	make ARCH=arm64 CROSS_COMPILE=$(TOOLCHAIN)bin/aarch64-linux-android- -C $(KERNEL) M=$(shell pwd)   CFLAGS_MODULE=-fno-pic modules
clean:
	make -C $(KERNEL) M=$(shell pwd) clean
