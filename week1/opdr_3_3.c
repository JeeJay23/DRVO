#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

MODULE_LICENSE("Dual BSD/GPL");

// ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
// ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
// int (*open) (struct inode *, struct file *);
// int (*release) (struct inode *, struct file *);


static int hello_init(void)
{
	printk(KERN_ALERT "Hello, world\n");
	return 0;
}
static void hello_exit(void)
{
	printk(KERN_ALERT "Goodbye, world\n");
}
static int open(struct inode* node, struct file* fp)
{
	printk(KERN_ALERT "Goodbye, world\n");
	return 0;
}
static int release(struct inode* node, struct file* fp)
{
	printk(KERN_ALERT "Goodbye, world\n");
	return 0;
}

// ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
static ssize_t read(struct file* fp, char __user* buffer, size_t size, loff_t* loff)
{
	printk(KERN_ALERT "Goodbye, world\n");
	return 0;
}

// ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
static ssize_t write(struct file* fp, const char __user* buffer, size_t size, loff_t* loff)
{
	printk(KERN_ALERT "Goodbye, world\n");
	return 0;
}

static struct file_operations fops = {
	.read = read,
	.write = write,
	.open = open,
	.release = release
};

module_init(hello_init);
module_exit(hello_exit);
