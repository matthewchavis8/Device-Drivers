#ifndef __DEFERRED_H__
#define __DEFERRED_H__

#include <asm/ioctl.h>
typedef unsigned long ul;


#define MY_IOCTL_TIMER_SET    _IOW('k', 1, ul)
#define MY_IOCTL_TIMER_CANCEL _IO('k', 2)
#define MY_IOCTL_TIMER_ALLOC  _IO('k', 3)

#endif /* __DEFERRED_H__ */
