#include "ndp_dev_a_file.h"
#include <linux/init.h>       /* module_init, module_exit */
#include <linux/module.h>     /* version info, MODULE_LICENSE, MODULE_AUTHOR, printk() */
#include <asm/io.h>           /* address translation */

extern char *ndp_dev_a_w, *ndp_dev_a_r;

MODULE_DESCRIPTION("NDPDevA Driver Template");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joao Vieira");

/* =============================================================================================== */
static int ndp_dev_a_driver_init(void)
{
    int result = 0;
    printk( KERN_NOTICE "NDPDevA Driver: Initialization started\n" );

    ndp_dev_a_w = ioremap(NDP_DEV_A_AWRI, NDP_DEV_A_SWRI);
    ndp_dev_a_r = ioremap(NDP_DEV_A_AREA, NDP_DEV_A_SREA);

    result = register_device();
    return result;
}

/* =============================================================================================== */
static void ndp_dev_a_driver_exit(void)
{
    printk( KERN_NOTICE "NDPDevA Driver: Exiting\n" );
    unregister_device();
}

/* =============================================================================================== */
module_init(ndp_dev_a_driver_init);
module_exit(ndp_dev_a_driver_exit);
