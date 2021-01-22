#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/kdev_t.h>

#include "data.h"

static dev_t ppg_dev;

struct cdev ppg_cdev;

static int m_ref_count;

static int ret_bytes;

ssize_t read_ppg(struct file *file_ptr, char __user *buffer, size_t count, loff_t *f_pos)
{
	printk(KERN_INFO "Reading from ppg driver\n");
	ret_bytes = copy_to_user(buffer, &ppg[m_ref_count], sizeof(int));
	++m_ref_count;
	if(m_ref_count >= sizeof(ppg)/sizeof(int))
	{
		m_ref_count = 0;
	}
	return (sizeof(int) - ret_bytes);
}

struct file_operations file_ops = {
	.owner = THIS_MODULE,
	.read = read_ppg
};

static int __init ppg_module_init(void)
{
	printk(KERN_INFO "Loading ppg driver\n");

	alloc_chrdev_region(&ppg_dev, 0, 1, "ppg_device");
	cdev_init(&ppg_cdev, &file_ops);
	ppg_cdev.owner = THIS_MODULE;
	cdev_add(&ppg_cdev, ppg_dev, 1);

	m_ref_count = 0;
	return 0;
}

static void __exit ppg_clean(void)
{
	printk(KERN_INFO "Unloading ppg driver\n");
	cdev_del(&ppg_cdev);
	unregister_chrdev_region(ppg_dev, 1);
	printk(KERN_INFO "Unloaded ppg driver\n");
}

module_init(ppg_module_init);
module_exit(ppg_clean);

MODULE_AUTHOR("Samuele Carboni");
MODULE_LICENSE("GPL");
