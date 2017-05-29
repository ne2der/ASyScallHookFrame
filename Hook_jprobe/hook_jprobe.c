#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/kprobes.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/uaccess.h>


asmlinkage int jsys_open(const char *filename,int flags,mode_t mode){

	printk(KERN_INFO "HOOKED ,this is jsys_open \n");

	pid_t current_pid = current_thread_info()->task->tgid;
	char s[1024];

	sprintf(s,"[OPEN]PID:%d  ,flags :%d,mode :%x ,filename:%s \n",current_pid,flags,mode,filename);

        printk(s);
	jprobe_return();
	return 0;

}

asmlinkage int jsys_openat(int dfd,const char *filename,int flags,mode_t mode){

	printk(KERN_INFO "HOOKED ,this is jsys_openat \n");

	pid_t current_pid = current_thread_info()->task->tgid;
	char s[1024];

	sprintf(s,"[OPEN]PID:%d  dfd :%d,flags :%d,mode :%x,filename:%s \n",current_pid,dfd,flags,mode,filename);
        printk(s);
	jprobe_return();
	return 0;

}

static struct jprobe open_jprobe = {
	.entry	=jsys_open,
	.kp = {
		.symbol_name	="sys_open",
	},
};

static struct jprobe openat_jprobe= {
	.entry	=jsys_openat,
	.kp = {
		.symbol_name	="sys_openat",
	},
};



static struct jprobe *my_jprobe[] = {
	&open_jprobe,
	&openat_jprobe
};
int init_module(void){

	printk(KERN_INFO "init hook ,ready to go \n");
  	int ret = register_jprobes(&my_jprobe,sizeof(my_jprobe)/sizeof(my_jprobe[0]));
	printk("my_jprobe %d,my_jprobe[0]:%d size = %d \n",sizeof(my_jprobe),sizeof(my_jprobe[0]),sizeof(my_jprobe)/sizeof(my_jprobe[0]));
	if(ret < 0){
	printk(KERN_INFO "register_jprobes failed, return %d \n",ret);
	return -1;
	
	
	}
	return 0;



}

 void cleanup_module(void){

	unregister_jprobes(&my_jprobe,sizeof(my_jprobe)/sizeof(my_jprobe[0]));
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ne2der");
