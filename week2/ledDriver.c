#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>

#include <asm/io.h>

MODULE_LICENSE("Dual BSD/GPL");

#define GPIO1_ADDR 0x4804c000
#define GPIO0_ADDR 0x44e07000

#define CM_GCM_AD9 0x44e10824
#define CM_GCM_A3 0x44e1084c
#define CM_GPMC_AD13 0x44e10834
#define CM_GPMC_A2 0x44e10848

// #define PIN 11
// #define PIN 13
#define PIN 18
// #define PIN 16

#define GPIO_OE 0x4d
// #define GPIO_OE 0x134

#define GPIO_SETDATAOUT 0x65
// #define GPIO_SETDATAOUT 0x194

#define GPIO_CLEARDATAOUT 0x64
// #define GPIO_CLEARDATAOUT 0x190

#define GPIO_MAX 0x198

static struct cdev* device;

static const int major  = 420;
static const int minor = 0;

static const int amount = 1;
static char* msg = NULL;

static const char driver_name[] = "Hello World Driver";

static int intParamA = 0;
static int intParamB = 0;

module_param(intParamA, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
module_param(intParamB, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
module_param_named(readableNameA, intParamA, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
module_param_named(readableNameB, intParamB, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

static int open(struct inode* node, struct file* fp)
{
	printk(KERN_ALERT "Opened hello\n");
	return 0;
}
static int release(struct inode* node, struct file* fp)
{
	printk(KERN_ALERT "released hello\n");
	return 0;
}

// ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
static ssize_t read(struct file* fp, char __user* buffer, size_t size, loff_t* loff)
{
	uint32_t* gpio = ioremap(GPIO1_ADDR, GPIO_MAX);
	barrier();

	iowrite32((1<<PIN), gpio + GPIO_CLEARDATAOUT); wmb();

	iounmap(gpio);

	printk(KERN_ALERT "turn led off\n", size);
	return 0;
}

// ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
static ssize_t write(struct file* fp, const char __user* buffer, size_t size, loff_t* loff)
{
	uint32_t* gpio = ioremap(GPIO1_ADDR, GPIO_MAX);
    barrier();
    iowrite32((1 << PIN), gpio + GPIO_SETDATAOUT); wmb();

    // memset(msg, 0, 32);

    // int count = copy_from_user(msg, buffer, size);
	// // for now, make write set the led on
	// barrier();

    // if (msg[0] == '1'){
    //     iowrite32((1 << PIN), gpio + GPIO_SETDATAOUT);
    // }
    // else{
    //     iowrite32((1 << PIN), gpio + GPIO_CLEARDATAOUT);
    // }

	iounmap(gpio);

	printk(KERN_ALERT "turn led on\n", size);
	return size;
}

static struct file_operations fops = {
	.read = read,
	.write = write,
	.open = open,
	.release = release
};

static int hello_init(void)
{
	int res = 0;
	dev_t device_number = MKDEV(major, minor);
	device = cdev_alloc();
	res = register_chrdev_region(device_number, amount, driver_name);

	cdev_init(device, &fops);

	if (res <  0)
	{
		printk(KERN_ALERT "Failed to register device\n");
		return res;
	}

	printk(KERN_ALERT "registered device\n");

	res = cdev_add(device, device_number, amount);
	if (res < 0){
		unregister_chrdev_region(device_number, amount);
		printk(KERN_ALERT "Failed to add device\n");
		return 0;
	}

	uint32_t* cm = ioremap(CM_GPMC_A2, sizeof(uint32_t*));
	uint32_t* gpio = ioremap(GPIO1_ADDR, GPIO_MAX * sizeof(uint32_t));

	barrier();

	// set mux to gpio mode
	uint32_t val = ioread32(cm); rmb();
	val &= (~0x7);
	val |= 0x7;
	iowrite32(val, cm); wmb();
	
	// set pin as output pin
	uint32_t oe = ioread32(gpio + GPIO_OE); rmb();
	iowrite32((oe & (~(1<<PIN))), gpio + GPIO_OE); wmb();

	iounmap(cm);
	iounmap(gpio);

	printk(KERN_ALERT "Hello driver added successfully\n");
	printk(KERN_ALERT "Started driver with A: %i and B: %i\n", intParamA, intParamB);

	return 0;
}

static void hello_exit(void)
{
	dev_t device_number;
	device_number = MKDEV(major, minor);
	printk(KERN_ALERT "Driver has been succesfully unregistered\n");
	unregister_chrdev_region(device_number, amount);
}

module_init(hello_init);
module_exit(hello_exit);
