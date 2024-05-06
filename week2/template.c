#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kernel.h>

// #include <asm/system.h>
// #include <asm-generic/iomap.h>
#include <asm/io.h>

MODULE_LICENSE("Dual BSD/GPL");

#define GPIO0_ADDR 0x4804c000
#define PIN 13

// 0x134
#define GPIO_OE 0x4d

//0x194
#define GPIO_SETDATAOUT 0x65

//0x190
#define GPIO_CLEARDATAOUT 0x64

static struct cdev* device;

static const int major  = 420;
static const int minor = 0;

static const int amount = 1;

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
	uint32_t* gpio0 = ioremap(GPIO0_ADDR, 0x198);
	barrier();

	iowrite32((1<<PIN), gpio0 + GPIO_CLEARDATAOUT); wmb();

	iounmap(gpio0);

	printk(KERN_ALERT "read %i bytes\n", size);
	return 0;
}

// ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
static ssize_t write(struct file* fp, const char __user* buffer, size_t size, loff_t* loff)
{
	// for now, make write set the led on
	uint32_t* gpio0 = ioremap(GPIO0_ADDR, 0x198);
	barrier();

	iowrite32((1<<PIN), gpio0 + GPIO_SETDATAOUT); wmb();

	iounmap(gpio0);

	printk(KERN_ALERT "written %i bytes\n", size);
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

	printk(KERN_ALERT "Hello driver added successfully\n");
	printk(KERN_ALERT "Started driver with A: %i and B: %i\n", intParamA, intParamB);

	// address of control module pin 13
	uint32_t* unmapped_cm_gdm_ad9 = (uint32_t*)(0x44e10824);

	// remap IO
	uint32_t* cm_ad9 = ioremap(unmapped_cm_gdm_ad9, sizeof(*unmapped_cm_gdm_ad9));
	uint32_t* gpio0 = ioremap(GPIO0_ADDR, 0x198 * sizeof(uint32_t));

	if (cm_ad9 == NULL) return 0;
	if (gpio0 == NULL) return 0;

	uint32_t oe;

	barrier();

	// set mux to gpio mode
	uint32_t val = ioread32(cm_ad9); rmb();
	val &= (~0x7);
	val |= 0x7;
	iowrite32(val, cm_ad9); wmb();

	// set pin 13 as output pin
	oe = ioread32(gpio0 + GPIO_OE); rmb();
	oe &= ~(1<<PIN);
	iowrite32(oe, gpio0); wmb();

	iounmap(cm_ad9);
	iounmap(gpio0);

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
