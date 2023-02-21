#include "ndp_device_file.h"
#include <linux/fs.h> 	    /* file stuff */
#include <linux/kernel.h>   /* printk() */
#include <linux/errno.h>    /* error codes */
#include <linux/module.h>   /* THIS_MODULE */
#include <linux/cdev.h>     /* char device stuff */
#include <linux/uaccess.h>  /* copy_to_user(), copy_from_user() */

char *ndp_device_ptr;

/*===============================================================================================*/
static ssize_t ndp_device_file_write(
    struct file *file_ptr
    , const char __user *user_buffer
    , size_t count
    , loff_t *position)
{
    printk( KERN_NOTICE "NDP Device Driver [W]: %s (%ld bytes)\n", user_buffer, count );

    copy_from_user(ndp_device_ptr, user_buffer, count);

    return count;
}

/*===============================================================================================*/
static ssize_t ndp_device_file_read(
    struct file *file_ptr
    , char __user *user_buffer
    , size_t count
    , loff_t *position)
{
    printk( KERN_NOTICE "NDP Driver [R]: %s\n", ndp_device_ptr );

    copy_to_user(user_buffer, ndp_device_ptr, count);

    return count;
}


/*===============================================================================================*/
static struct file_operations simple_driver_fops =
{
    .owner = THIS_MODULE,
    .read = ndp_device_file_read,
    .write = ndp_device_file_write,
};

static int device_file_major_number = 0;
static const char device_name[] = "ndp_device_driver";

/*===============================================================================================*/
int register_device(void)
{
    int result = 0;

    printk( KERN_NOTICE "NDP Device Driver: register_device() is called.\n" );

    result = register_chrdev( 0, device_name, &simple_driver_fops );
    if( result < 0 )
    {
        printk( KERN_WARNING "NDP Device Driver: cannot register character device with errorcode = %i\n", result );
        return result;
    }

    device_file_major_number = result;
    printk( KERN_NOTICE "NDP Device Driver: registered character device with major number = %i and minor numbers 0...255\n"
        , device_file_major_number );

    return 0;
}

/*===============================================================================================*/
void unregister_device(void)
{
    printk( KERN_NOTICE "NDP Device Driver: unregister_device() is called\n" );
    if(device_file_major_number != 0)
    {
        unregister_chrdev(device_file_major_number, device_name);
    }
}
