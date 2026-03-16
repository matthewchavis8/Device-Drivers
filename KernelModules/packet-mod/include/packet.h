#ifndef __PACKET_H__
#define __PACKET_H__

#include <asm/ioctl.h>
typedef unsigned int u32;

#define MY_IOCTL_FILTER_ADDRESS  _IOW('k', 1, u32)

#endif // !__PACKET_H__
