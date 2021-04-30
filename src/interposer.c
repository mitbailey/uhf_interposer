/**
 * @file interposer.c
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Acts as a go-between for the terminal and UHF transceiver.
 * @version 0.1
 * @date 2021-04-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include "include/interposer.h"

static int device_file_major_number = 0;
static const char device_name[] = "uhf_interposer";

static const char test_string[] = "Hello from kernel world!";
static const ssize_t test_string_size = sizeof(test_string);

// Registers the device.
int register_device(void){
    int result = 0;

    printk(KERN_NOTICE "uhf_interposer: register_device() has been called.\n");

    result = register_chrdev(0, device_name, &__ipsr_ops);

    if (result < 0){
        printk(KERN_WARNING "uhf_interposer: cannot register character device.\n
                            ERR %i\n", result);

        return result;
    }    

    device_file_major_number = result;

    printk(KERN_NOTICE "uhf_interposer: registered character device.\n
                        Major Number %i\n", device_file_major_number);

    return 0;
} 

// Function used to unregister the device.
void unregister_device(void){
    printk(KERN_NOTICE "uhf_interposer: unregister_device() has been called.\n");

    if (device_file_major_number != 0){
        unregister_chrdev(device_file_major_number, device_name);
    }

    printk(KERN_WARN "uhf_interposer: this device could not be unregistered because it was not registered.\n");
}

ssize_t ipsr_read(struct file *file, char __user *userBuffer, size_t size, loff_t *offset){
    printk(KERN_NOTICE "uhf_interposer: device file is being read at offset %i, read bytes count = %u\n", (int)*position, (unsigned int) count);
    
    struct uhf_device_data *data = (struct uhf_device_data*) file->private_data;
    
    ssize_t len = min(data->size - *offset, size);

    if (len <= 0){
        return 0;
    }

    if(copy_to_user(user_buffer, data->buffer + *offset, len) != 0){
        return -EFAULT;
    }

    *offset += len;
    return len;
}

// write
ssize_t ipsr_write (struct file * file, const char __user *userBuffer, size_t size, loff_t *offset){
    printk(KERN_NOTICE "uhf_transposer: device file is being written at offset %i, written bytes count = %u\n", (int)*offset, (unsigned int) size);

    struct uhf_device_data *data = (struct uhf_device_data*) file->private_data;
    
    ssize_t len = min(data->size - *offset, size);

    if (len <= 0){
        return 0;
    }

    if(copy_from_user(data->buffer + *offset, userBuffer, len)){
        return -EFAULT;
    }

    *offset += len;
    return len;
}

// flush
int ipsr_flush (struct file *, fl_owner_t id){

}

// open
// Initializes a device.
static int ipsr_open (struct inode *inode, struct file *file){
    struct uhf_device_data *data;
    data = container_of(inode->i_cdev, struct uhf_device_data, cdev);
    
    // Validate access
    file->private_data = data;

    /* initialize the device */

    return 0;
}

// release
int ipsr_release (struct inode *, struct file *){

}