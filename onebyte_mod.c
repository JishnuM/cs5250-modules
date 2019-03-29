#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define MAJOR_NUMBER 61

/* forward declaration. */
int onebyte_open(struct inode *inode, struct file *filep);
int onebyte_release(struct inode *inode, struct file *filep);

ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos);


static void onebyte_exit(void);

/** definition of file operation structure. */
struct file_operations onebyte_fops = {
	read: onebyte_read,
	write: onebyte_write,
	open: onebyte_open,
	release: onebyte_release
};

char *onebyte_data = NULL;

int onebyte_open(struct inode *inode, struct file *filep) {
	return 0; // always successful.
}

int onebyte_release(struct inode *inode, struct file *filep) {
	return 0; // always successful.
}

// TODO: Implement read.
ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos) {
	static int ended = 0;

	if (ended) {
		// There is nothing more to read.
		ended = 0;
		return 0;
	}

	ended = 1;

	if (put_user(*onebyte_data, buf) ) {
		return -EFAULT;
	}


	return 1;
}

// TODO: Implement write.
ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos) {
	get_user(*onebyte_data, buf);	
	return 1;
}

static int onebyte_init(void) {
	int result;
	result = register_chrdev(MAJOR_NUMBER, "onebyte", &onebyte_fops);
	
	if (result < 0)
		return result;

	onebyte_data = kmalloc(sizeof(char), GFP_KERNEL);
	if (!onebyte_data) {
		onebyte_exit();
		return -ENOMEM;
	}

	*onebyte_data = 'X';
	printk(KERN_ALERT "This is a onebyte device module\n");
	return 0;
}

static void onebyte_exit(void) {
	if (onebyte_data) {
		kfree(onebyte_data);
		onebyte_data = NULL;
	}

	unregister_chrdev(MAJOR_NUMBER, "onebyte");
	printk(KERN_ALERT "Onebyte device module is unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(onebyte_init);
module_exit(onebyte_exit);
