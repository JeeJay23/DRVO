#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

ssize_t scull_read(struct file *filp, char __user *buff,
                   size_t count, loff_t *offp);
ssize_t scull_write(struct file *filp, const char __user *buff,
                    size_t count, loff_t *offp);
int scull_release(struct inode *inode, struct file *filp);
int scull_open(struct inode *inode, struct file *filp);

static const int major = 420;
static const int minor = 0;
static const int amount = 1;
static const char driver_name[] = "hello_driver";
dev_t device_number;

static struct cdev *device;

struct file_operations ops = {
    .read = scull_read,
    .release = scull_release,
    .open = scull_open,
    .write = scull_write};

static int openCount = 0;
static char *msg = NULL;
struct file f;

/* PIN */
#define PIN 19
/* base address */
#define GPIO1_ADDR 0x4804c000
#define MUX_A3 0x44e10820
/* register offsets in uint32_t sizes */
#define GPIO_OE 0x4D           // 0x134
#define GPIO_DATAIN 0x4E       // 0x138
#define GPIO_CLEARDATAOUT 0x64 // 0x190
#define GPIO_SETDATAOUT 0x65   // 0x194
/* max size in bytes */
#define GPIO_MAX 0x198
uint32_t *gpio1;
uint32_t oe;

static int basic_init(void)
{
    int result;
    result = alloc_chrdev_region(&device_number, minor, amount, driver_name);
    device = cdev_alloc();
    cdev_init(device, &ops);
    result = cdev_add(device, device_number, amount);

    /* output instellen */
    gpio1 = ioremap(MUX_A3, GPIO_MAX * sizeof(uint32_t));
    barrier();
    oe = ioread32(gpio1 + GPIO_OE);
    rmb();
    iowrite32((oe & (~(1 << PIN))), gpio1 + GPIO_OE);
    wmb(); // write memory barrier
    iounmap(gpio1);

    printk(KERN_INFO "Init ran of opgave65\n Major: %i, Minor: %i\n", device_number, minor);
    return result;
}

static void basic_exit(void)
{
    openCount = 0;

    printk(KERN_INFO "Exiting opgave65\n");
    cdev_del(device);
    unregister_chrdev_region(device_number, amount);
}

int scull_open(struct inode *inode, struct file *filp)
{
    openCount++;
    printk(KERN_INFO "Module voor de %i keer geopend", openCount);
    printk(KERN_INFO "open function\n");
    return 0;
}

int scull_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "release function\n");
    return 0;
}

ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                   loff_t *f_pos)
{

    return NULL;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos)
{
    memset(msg, 0, 32);
    count = copy_from_user(msg, buf, count);

    gpio1 = ioremap(GPIO1_ADDR, GPIO_MAX);
    barrier();

    if (msg[0] == '1')
    {
        iowrite32((1 << PIN), gpio1 + GPIO_SETDATAOUT); // Pin 19 aan
    }
    else
    {
        iowrite32((1 << PIN), gpio1 + GPIO_CLEARDATAOUT); // Pin 19 uit
    }
    wmb();
    iounmap(gpio1);

    return 0;
}

module_init(basic_init);
module_exit(basic_exit);