#ifndef _DEVICE_FILE_H_
#define _DEVICE_FILE_H_

#include <linux/compiler.h>             /* __must_check */

__must_check int register_device(void); /* 0 if ok*/
void unregister_device(void);

#define NDP_DEV_A_CADR 0x40000000
#define NDP_DEV_A_CSZE 0x1000

#define NDP_DEV_A_AWRI NDP_DEV_A_CADR
#define NDP_DEV_A_SWRI 0x28				// 5 8-byte registers to write

#define NDP_DEV_A_AREA (NDP_DEV_A_CADR + NDP_DEV_A_SWRI)
#define NDP_DEV_A_SREA 0x10				// 2 8-byte registers to read

#endif // _DEVICE_FILE_H_
