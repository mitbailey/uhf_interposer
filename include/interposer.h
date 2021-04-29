/**
 * @file interposer.h
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Acts as a go-between for the terminal and UHF transceiver.
 * @version 0.1
 * @date 2021-04-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

// https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html
// https://www.apriorit.com/dev-blog/195-simple-driver-for-linux-os

// Should include the following operations:
// open
// close
// flush
// read
// write

//#include <linux/init.h> 
// https://stackoverflow.com/questions/28423477/linux-init-h-no-such-file-or-directory
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

// File ops contains pointers to various functions for the driver.
struct file_operations {
    struct module *owner;
    ssize_t (*read) (struct file *, char __user *, size_t loff_t *);
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
    int (*open) (struct inode *, struct file *);
    int (*flush) (struct file *, fl_owner_t id);
    int (*release) (struct inode *, struct file *);
} __ipsr_ops;

// Contains our device's data.
// A character device must have the cdev type.
struct uhf_device_data {
    struct cdev cdev;
    // Data below ...


}

// Function used to register the device.
// On failure: Prints, returns error value.
// On success: Prints, returns 0.
int register_device(void);

// Function used to unregister the device.
void unregister_device(void);

// Reads characters from the device.
// filep: pointer to the file structure. 
// buffer: the data we've read is allocated in the user space addressed by this.
// len: number of bytes to be read
// offset: where to begin reading bytes from
// Returns: the number of bytes successfully read.
/*
"To work with information from the device file, the user allocates a special
buffer in the user-mode address space. Then, the read function copies the 
information to this buffer. The address to which a pointer from the user
space points and the address in the kernel address space may have different
values."
*/
// Copies from kernel buffer to buffer in the user space.
// long copy_to_user(void __user *to, const void * from, unsigned long n)
//                  ptr to buffer   ptr to data source  number of bytes to copy
ssize_t device_file_read(struct file *filep, char *buffer, size_t len, loff_t *offset);

// WIP, still not sure about this one.
static int ipsr_open (struct inode *inode, struct file *file);