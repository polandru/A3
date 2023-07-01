/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Dan Walkes
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations
#include "aesdchar.h"
int aesd_major =   0; // use dynamic major
int aesd_minor =   0;
#define BUFFSIZE 25000
MODULE_AUTHOR("Your Name Here"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_dev aesd_device;

int aesd_open(struct inode *inode, struct file *filp)
{
    PDEBUG("open");
    /**
     * TODO: handle open
     */
    //from scull
    // struct scull_dev *dev; /* device information */
	// dev = container_of(inode->i_cdev, struct scull_dev, cdev);
	// filp->private_data = dev; /* for other methods */

    aesd_device = *container_of(inode->i_cdev, struct aesd_dev,cdev);
    return 0;
}

int aesd_release(struct inode *inode, struct file *filp)
{
    PDEBUG("release");
    /**
     * TODO: handle release
     */
    return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    PDEBUG("read %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle read
     */

    if(mutex_lock_interruptible(&aesd_device.lock)){
       int len = strlen(aesd_device.buffer[aesd_device.r_pos]);
       if(len  > 0 && strlen(aesd_device.buffer[aesd_device.r_pos])>0){
        retval = len;
        copy_to_user(buf,aesd_device.buffer[aesd_device.r_pos],len);
        aesd_device.r_pos = (aesd_device.r_pos+1)%10;
       }
    }
    mutex_unlock(&aesd_device.lock);
    return retval;
}

ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = -ENOMEM;
    PDEBUG("write %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle write
     */
    // if(mutex_lock_interruptible(&aesd_device.lock)){
        char * tmp =  kmalloc(count, GFP_KERNEL);
        copy_from_user(tmp,buf,count);
        if(count > 0){
            strcat(aesd_device.temp, tmp);

            if(tmp[count -1 ] == '\n'){
                strcpy(aesd_device.buffer[aesd_device.w_pos], aesd_device.temp);
                memset(aesd_device.temp,0,BUFFSIZE);
                retval = count;
            }
               
            
        }
        kfree(tmp);
    // }
    
    return retval;
}
struct file_operations aesd_fops = {
    .owner =    THIS_MODULE,
    .read =     aesd_read,
    .write =    aesd_write,
    .open =     aesd_open,
    .release =  aesd_release,
};

static int aesd_setup_cdev(struct aesd_dev *dev)
{
    int err, devno = MKDEV(aesd_major, aesd_minor);

    cdev_init(&dev->cdev, &aesd_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &aesd_fops;
    err = cdev_add (&dev->cdev, devno, 1);
    if (err) {
        printk(KERN_ERR "Error %d adding aesd cdev", err);
    }
    return err;
}



int aesd_init_module(void)
{
    dev_t dev = 0;
    int result;
    result = alloc_chrdev_region(&dev, aesd_minor, 1,
            "aesdchar");
    aesd_major = MAJOR(dev);
    if (result < 0) {
        printk(KERN_WARNING "Can't get major %d\n", aesd_major);
        return result;
    }
    memset(&aesd_device,0,sizeof(struct aesd_dev));

    /**
     * TODO: initialize the AESD specific portion of the device
     */

    aesd_device.r_pos=0;
    aesd_device.w_pos=0;
    for(int i =0; i < 10; i++){
        aesd_device.buffer[i] = kmalloc(BUFFSIZE,GFP_KERNEL);
        memset(aesd_device.buffer[i],0,BUFFSIZE);
    }
    mutex_init(&aesd_device.lock);
    aesd_device.temp = kmalloc(BUFFSIZE,GFP_KERNEL);
    memset(aesd_device.temp,0,BUFFSIZE);


    result = aesd_setup_cdev(&aesd_device); 

    if( result ) {
        unregister_chrdev_region(dev, 1);
    }
    return result;

}

void aesd_cleanup_module(void)
{
    dev_t devno = MKDEV(aesd_major, aesd_minor);

    cdev_del(&aesd_device.cdev);

    /**
     * TODO: cleanup AESD specific poritions here as necessary
     */

    unregister_chrdev_region(devno, 1);
}



module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
