#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/scatterlist.h>
#include <linux/mutex.h>
#include <linux/syscalls.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include "hook_fuc.h"


void **sys_call_table;
#ifdef ARM64
void **compat_sys_call_table;
#endif
void **sys_call_table_find;

static unsigned long **find_sys_call_table(void);

asmlinkage long _hook_sys_openat(unsigned int dfd, const char __user *filename, int flags, umode_t mode)
{


	printk(KERN_ERR "into openat syscall hook\n");

	return real_sys_openat(dfd,filename,flags,mode);

}



#ifdef ARM64
asmlinkage long _hook_compat_sys_openat(unsigned int dfd, const char __user *filename, int flags, umode_t mode)
{


	printk(KERN_ERR "into compat openat syscall  hook\n");

	return real_compat_sys_openat(dfd,filename,flags,mode);

}


#endif


// find sys_call_table through sys_close address
static unsigned long **find_sys_call_table() {
    unsigned long offset;
    unsigned long **sct;
    
    #ifdef ARM64
    int flag=1;
    #else
    int flag=0;
    #endif

    //sys call num maybe different
    //check in unistd.h
    //__NR_close will use 64bit version unistd.h by default when build LKM
    for(offset = PAGE_OFFSET; offset < ULLONG_MAX; offset += sizeof(void *)) {
	sct = (unsigned long **) offset;
       
	if(sct[__NR_close] == (unsigned long *) sys_close)
	      if( flag==0){
  	   	 printk(KERN_ERR "find sys call table :%llx ",sct);
	   	 return sct;
        	}else{
  	    	printk(KERN_ERR "find first  sys call table :%llx ",sct);
	    	flag--;

	}

    }

    /*
     * Given the loop limit, it's somewhat unlikely we'll get here. I don't
     * even know if we can attempt to fetch such high addresses from memory,
     * and even if you can, it will take a while!
     */
    return NULL;
}

#ifdef ARM64
//compat_sys_call_table for 32 bit version call
static unsigned long **find_compat_sys_call_table() {
    unsigned long offset;
    unsigned long **sct;

    //sys call num maybe different
    //check in asm-generic/unistd.h
    //LKM module will use 64bit value of __NR_close by default
    //To get correct offset of sys_close  in compat_sys_call_table ,NR_close need to be set as 6
    int NR_close = 6;
    for(offset = PAGE_OFFSET; offset < ULLONG_MAX; offset += sizeof(void *)) {
	sct = (unsigned long **) offset;
       
	if(sct[NR_close] == (unsigned long *) sys_close){
  	printk(KERN_ERR "find compat sys call table :%llx ",sct);
	    return sct;
	}
    }

    /*
     * Given the loop limit, it's somewhat unlikely we'll get here. I don't
     * even know if we can attempt to fetch such high addresses from memory,
     * and even if you can, it will take a while!
     */
    return NULL;
}
#endif







//auto run when load module , hook target fuc here
int init_module() {

  sys_call_table = find_sys_call_table();
 
  printk(KERN_ERR "find sys call table :%llx ",sys_call_table);
  //sys_call_table = (void*)0xffffffc00008c000;
  real_sys_openat = (void*)(sys_call_table[__NR_openat]);
  printk(KERN_ERR "real_openat addr :%llx ",real_sys_openat);
  printk(KERN_ERR "_NR_openat:%d ",__NR_openat);
  printk(KERN_ERR "hook_openat addr :%llx ",_hook_sys_openat);
 
 
  sys_call_table[__NR_openat]=&_hook_sys_openat;

  #ifdef ARM64
  int NR_openat =  322;
  compat_sys_call_table = find_compat_sys_call_table();
  real_compat_sys_openat = (void*)(compat_sys_call_table[NR_openat]);
  printk(KERN_ERR "real_compat_openat addr :%llx ",real_compat_sys_openat);
  compat_sys_call_table[NR_openat]=&_hook_compat_sys_openat;

  printk(KERN_ERR "hook_compat_sys_openat addr :%llx ",_hook_compat_sys_openat);
  #endif


  return 0;
}

//can not rmmod module without this fuc
void cleanup_module()
{
 sys_call_table[__NR_openat]=real_sys_openat;
#ifdef ARM64
 int NR_openat =  322;
 compat_sys_call_table[NR_openat]=real_compat_sys_openat;

#endif
return ;
}


//for  hash fuc
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Test Module");
MODULE_AUTHOR("user<user@gmail.com>");
MODULE_ALIAS("test");
