#ifndef _DEVICE_FILE_H_
#define _DEVICE_FILE_H_

#include <linux/compiler.h>             /* __must_check */

__must_check int register_device(void); /* 0 if ok*/
void unregister_device(void);

#define NDP_DEVICE_ADDR 0x620000000
#define NDP_DEVICE_SIZE 0x40000000

#endif // _DEVICE_FILE_H_
