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

ssize_t device_file_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    printk(KERN_NOTICE "uhf_interposer: device file is being read at offset %i, read bytes count = %u\n", (int)*position, (unsigned int) count);

    // Check if the position has moved beyond the end of the file.
    if(*position >= test_string_size){
        return 0;
    }

    // If more is requested that available, send all available.
    if(*position + count > test_string_size){
        count = test_string_size - *position;
    }

    if(copy_to_user(user_buffer, test_string + *position, count) != 0){
        return -EFAULT;
    }

    *position += count;
    return count;
}

// WIP, still not sure about this one.
static int ipsr_open (struct inode *inode, struct file *file){
    struct uhf_device_data *data;
    data = container_of(inode->i_cdev, struct uhf_device_data, cdev);
    file->private_data = data;
}