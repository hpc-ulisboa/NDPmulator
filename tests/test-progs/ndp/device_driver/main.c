#include "ndp_device_file.h"
#include <linux/init.h>       /* module_init, module_exit */
#include <linux/module.h>     /* version info, MODULE_LICENSE, MODULE_AUTHOR, printk() */
#include <asm/io.h>           /* address translation */

extern char *ndp_device_ptr;

MODULE_DESCRIPTION("NDP Device Driver Template");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joao Vieira");

/* =============================================================================================== */
static int ndp_device_driver_init(void)
{
    int result = 0;
    printk( KERN_NOTICE "NDP Device Driver: Initialization started\n" );

    ndp_device_ptr = ioremap(NDP_DEVICE_ADDR, NDP_DEVICE_SIZE);

    result = register_device();
    return result;
}

/* =============================================================================================== */
static void ndp_device_driver_exit(void)
{
    printk( KERN_NOTICE "NDP Device Driver: Exiting\n" );
    unregister_device();
}

/* =============================================================================================== */
module_init(ndp_device_driver_init);
module_exit(ndp_device_driver_exit);
