#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/io.h>        //含有iomap函数iounmap函数
#include <asm/uaccess.h>//含有copy_from_user函数
#include <linux/device.h>//含有类相关的处理函数

#define LED1_GPIO 25
#define LED2_GPIO 10
#define LED3_GPIO 42

#define TLMM_BASE_ADDR              0x01000000
#define GPIO_CONFIG_ADDR(x)         (TLMM_BASE_ADDR + 0x0000 + (x)*0x1000)
#define GPIO_IN_OUT_ADDR(x)         (TLMM_BASE_ADDR + 0x0004 + (x)*0x1000)
//#define GPIO_OUT_SET_ADDR(x)        (TLMM_BASE_ADDR + 0x200040 + (x/32)*0x04)
//#define GPIO_OUT_CLR_ADDR(x)        (TLMM_BASE_ADDR + 0x200020 + (x/32)*0x04)
//#define GPIO_OUT_VAL(x)             (1 << (x - (x/32)*32))
//#define GPIO_OUT_OE_SET_ADDR(x)     (TLMM_BASE_ADDR + 0x2000C0 + (x/32)*0x04)
//#define GPIO_OUT_OE_VAL(x)          (1 << (x - (x/32)*32))

static struct class *myled_drv_class;//类
static struct class_device *myled_drv_class_dev[4];//类下面的设备
static int Myled_major;

static unsigned long *gp25con = NULL;
static unsigned long *gp25dat = NULL;
static unsigned long *gp10con = NULL;
static unsigned long *gp10dat = NULL;
static unsigned long *gp42con = NULL;
static unsigned long *gp42dat = NULL;

static int myled_open(struct inode *inode, struct file *file)
{
    int minor = MINOR(inode->i_rdev);//取得次设备号，根据次设备号来配置IO端口
    //printk("\n minor = %d\n",minor);
    
    switch(minor)  
        {
            case 0:
               *gp25con &= ~(0x3FF);
               *gp25con |= (0 << 0);
               *gp25con |= (0 << 2);
               *gp25con |= (3 << 6);
               *gp25con |= (1 << 9);
               
               *gp10con &= ~(0x3FF);
               *gp10con |= (0 << 0);
               *gp10con |= (0 << 2);
               *gp10con |= (3 << 6);
               *gp10con |= (1 << 9);
               
               *gp42con &= ~(0x3FF);
               *gp42con |= (0 << 0);
               *gp42con |= (0 << 2);
               *gp42con |= (3 << 6);
               *gp42con |= (1 << 9);
                printk("initialize leds\n");
                break;
            case 1:
               *gp25con &= ~(0x3FF);
               *gp25con |= (0 << 0);
               *gp25con |= (0 << 2);
               *gp25con |= (3 << 6);
               *gp25con |= (1 << 9);
                printk("initialize led1\n");
                break;
            case 2:
               *gp10con &= ~(0x3FF);
               *gp10con |= (0 << 0);
               *gp10con |= (0 << 2);
               *gp10con |= (3 << 6);
               *gp10con |= (1 << 9);
                printk("initialize led2\n");
                break;
            case 3:
               *gp42con &= ~(0x3FF);
               *gp42con |= (0 << 0);
               *gp42con |= (0 << 2);
               *gp42con |= (3 << 6);
               *gp42con |= (1 << 9);
                printk("initialize led3\n");
                break;
            default:break;
        }
    
    //printk("\n hello this is open\n");
    return 0;
}


static ssize_t myled_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
    char val;
    int ret;
    int minor = MINOR(file->f_dentry->d_inode->i_rdev);//根据文件取出次设备号
    
    ret = copy_from_user(&val, buf, count);//ret返回0表示拷贝成功

    if(!ret)
    {
        switch(minor)
        {
            case 0:
                if(val==1)
                {
                    *gp25dat &= ~(0x3<< 0);//bit0-1 清零
                    *gp25dat |= (0x2 << 0);//点灯
                    *gp10dat &= ~(0x3<< 0);//bit0-1 清零
                    *gp10dat |= (0x2 << 0);//点灯
                    *gp42dat &= ~(0x3<< 0);//bit0-1 清零
                    *gp42dat |= (0x2 << 0);//点灯
                     printk("leds on\n");
                }
                else if(val == 0)
                {
                    //*gpxdat |= ((1 << 4) | (1<<5) | (1<<6));//灭灯
                    *gp25dat &= ~(0x3<< 0);//bit0-1 清零
                    *gp10dat &= ~(0x3<< 0);//bit0-1 清零
                    *gp42dat &= ~(0x3<< 0);//bit0-1 清零
                    printk("leds off\n");
                }
                break;
            case 1:
                if(val==1)
                {
                    *gp25dat &= ~(0x3<< 0);//bit0-1 清零
                    *gp25dat |= (0x2 << 0);//点灯
                    printk("gpio_25 led1 on \n");
                }
                else if(val == 0)
                {
                    *gp25dat &= ~(0x3<< 0);//bit0-1 清零
                    printk("gpio_25 led1 off\n");
                }
                break;
            case 2:
                if(val==1)
                {
                    *gp10dat &= ~(0x3<< 0);//bit0-1 清零
                    *gp10dat |= (0x2 << 0);//点灯
                     printk("gpio_10 led2 on\n");
                }
                else if(val == 0)
                {
                    *gp10dat &= ~(0x3<< 0);//bit0-1 清零
                    printk("gpio_10 led2 off\n");
                }
                break;
            case 3:
                if(val==1)
                {
                    *gp42dat &= ~(0x3<< 0);//bit0-1 清零
                    *gp42dat |= (0x2 << 0);//点灯
                     printk("gpio_42 led3 on\n");
                }
                else if(val == 0)
                {
                    //*gpxdat |= ((1<<6));//灭灯
                    *gp42dat &= ~(0x3<< 0);//bit0-1 清零
                    printk("gpio_42 led3 off\n");
                }
                break;
            default:break;
        }
    }
    else
        printk("\n copy from user wrong!!!!%d  %d\n",ret,count);
    
    //printk("\n hello this is write\n");
    
    return 0;
}


static struct file_operations myled_drv_fops = {
    .owner  =   THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open   =   myled_open,     
    .write    =    myled_write,       
};


/*
 * 执行insmod命令时就会调用这个函数 
 */
static int __init myled_init(void)
{
    int minor;//次设备号
    Myled_major = register_chrdev(0, "myled_drv", &myled_drv_fops);//注册first_drv_fops结构体到字符设备驱动表,0表示自动分配主设备号
    if(Myled_major<0)
    {
              printk("\n first_drv can't register major number\n");
              return Myled_major;
        }

    myled_drv_class = class_create(THIS_MODULE, "leds");//创建类 
    
   // myled_drv_class_dev[0] = class_device_create(myled_drv_class, NULL, MKDEV(Myled_major, 0), NULL, "leds");//创建设备节点
    myled_drv_class_dev[0] = device_create(myled_drv_class, NULL, MKDEV(Myled_major, 0), NULL, "leds");//创建设备节点
    if (unlikely(IS_ERR(myled_drv_class_dev[0])))
            return PTR_ERR(myled_drv_class_dev[0]);

    for(minor=1;minor<4;minor++)
    {
        //myled_drv_class_dev[minor] = class_device_create(myled_drv_class, NULL, MKDEV(Myled_major, minor), NULL, "led%d",minor);//创建设备节点
        myled_drv_class_dev[minor] = device_create(myled_drv_class, NULL, MKDEV(Myled_major, minor), NULL, "led%d",minor);//创建设备节点
        if (unlikely(IS_ERR(myled_drv_class_dev[minor])))
            return PTR_ERR(myled_drv_class_dev[minor]);
    }

    gp25con = ioremap(GPIO_CONFIG_ADDR(LED1_GPIO), 16);//重映射，将物理地址变换为虚拟地址,gpio25
    gp25dat = gp25con + 1;
    gp10con = ioremap(GPIO_CONFIG_ADDR(LED2_GPIO), 16);//重映射，将物理地址变换为虚拟地址,gpio10
    gp10dat = gp10con + 1;
    gp42con = ioremap(GPIO_CONFIG_ADDR(LED3_GPIO), 16);//重映射，将物理地址变换为虚拟地址,gpio42
    gp42dat = gp42con + 1;
    
    return 0;
}

/*
 * 执行rmmod命令时就会调用这个函数 
 */
static void __exit myled_exit(void)
{
    int i;
    for(i=0;i<4;i++)
        //class_device_unregister(myled_drv_class_dev[i]);//删除设备节点
        device_unregister(myled_drv_class_dev[i]);//删除设备节点
        
    class_destroy(myled_drv_class);//删除类

    iounmap(gp25con);//删除重映射分配的地址
    iounmap(gp10con);//删除重映射分配的地址
    iounmap(gp42con);//删除重映射分配的地址
    
    unregister_chrdev(Myled_major, "myled_drv");//将rst_drv_fops结构体从字符设备驱动表中删除
    printk("\n myled module rmmod\n");
}


/* 这两行指定驱动程序的初始化函数和卸载函数 */
module_init(myled_init);
module_exit(myled_exit);


MODULE_LICENSE("GPL");//不加的话加载会有错误提醒
MODULE_AUTHOR("puck.shen");//作者
MODULE_VERSION("v02");//版本
MODULE_DESCRIPTION("MyLED Driver Test on MDM9x07");//简单的描述

