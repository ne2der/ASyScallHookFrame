//put your target function defination here
//
#define ARM64
asmlinkage long (*real_sys_openat)(unsigned int, const char __user*, int, umode_t);
#ifdef ARM64
asmlinkage long (*real_compat_sys_openat)(unsigned int, const char __user*, int, umode_t);
#endif

