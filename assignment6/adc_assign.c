#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <mach/hardware.h>
#include <mach/platform.h>
#include <mach/irqs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>


#define DEVICE_NAME 		"adc"
#define ADC_NUMCHANNELS		3
#define BUF_SIZE 100 /* maximum size of message buffer */

// adc registers
#define	ADCLK_CTRL			io_p2v(0x400040B4)
#define	ADCLK_CTRL1			io_p2v(0x40004060)
#define	ADC_SELECT			io_p2v(0x40048004)
#define	ADC_CTRL			io_p2v(0x40048008)
#define ADC_VALUE           io_p2v(0x40048048)
#define SIC2_ATR            io_p2v(0x40010010)
#define AD_PDN_STROBE 		(1 << 1)		/* G, waarom niet gewoon twee? Dus niet */
#define ADC_VALUE_MASK		0x3FF
#define EINT0_LOC			(1 << 23)
#define AD_PDN_CTRL			(1 << 2)
#define READ_REG(a)         (*(volatile unsigned int *)(a))
#define WRITE_REG(b,a)      (*(volatile unsigned int *)(a) = (b))

DECLARE_WAIT_QUEUE_HEAD(adc_interrupt_event);
static bool 			adc_interrupt_flag = false;
static bool 			gpi_interrupt_print = false;

typedef struct DriverInformation{
	char msg[BUF_SIZE];
	int minor;
}DriverInfo;


static unsigned char    adc_channel = 0;
static int              adc_values[ADC_NUMCHANNELS] = {0, 0, 0};

static irqreturn_t      adc_interrupt (int irq, void * dev_id);
static irqreturn_t      gp_interrupt (int irq, void * dev_id);


static void adc_init (void)
{
	unsigned long data;

	// set 32 KHz RTC clock
    data = READ_REG (ADCLK_CTRL);
    data |= 0x1;
    WRITE_REG (data, ADCLK_CTRL);

	// rtc clock ADC & Display = from PERIPH_CLK
    data = READ_REG (ADCLK_CTRL1);
    data &= ~0x01ff;
    WRITE_REG (data, ADCLK_CTRL1);

	// negatief & positieve referentie
    data = READ_REG(ADC_SELECT);
    data &= ~0x03c0;
    data |=  0x0280;
    WRITE_REG (data, ADC_SELECT);

    
    /* Bit 2 in register ADC_CTRL set => the ADC is powered up and reset */
    data = READ_REG(ADC_CTRL);
	data |= AD_PDN_CTRL; 				//AD_PDN_CTRL
	WRITE_REG (data, ADC_CTRL);
	
	/* Edge triggering op GPI_1 Table 5.4.7 */
	data = READ_REG(SIC2_ATR);
	data |= EINT0_LOC;
	WRITE_REG (data, SIC2_ATR);
	
	//IRQ init - Interrupt Enable Register for Sub Interrupt Controller 1
	// 7th bit in register = 0x40 in hex => TS_IRQ(ADC_INT) Touch screen irq interrupt
    if (request_irq (IRQ_LPC32XX_TS_IRQ, adc_interrupt, IRQF_DISABLED, "IRQ_ADC_INT_INTERRUPT", NULL) != 0)
    {
        printk(KERN_ALERT "ADC IRQ request failed\n");
    }
    
    /* 7 op J3, LPC3250_OEM_Board_Users_Guide_Rev_B Page 35 */
    if (request_irq (IRQ_LPC32XX_GPI_01, gp_interrupt, IRQF_DISABLED, "IRQ_GPI_01_INTERRUPT", NULL) != 0)
    {
        printk (KERN_ALERT "GP IRQ request failed\n");
    }
}


static void adc_start (unsigned char channel)
{	
	unsigned long data;

	if (channel >= ADC_NUMCHANNELS)
    {
        channel = 0;
    }

	data = READ_REG (ADC_SELECT);
	//selecteer het kanaal, eerst uitlezen, kanaalbits negeren en dan alleen de kanaalbits veranderen (0x0030)
	WRITE_REG((data & ~0x0030) | ((channel << 4) & 0x0030), ADC_SELECT);

	//nu ook globaal zetten zodat we de interrupt kunnen herkennen
	adc_channel = channel;
	
	*(unsigned int*)(LPC32XX_CLKPWR_LCDCLK_CTRL) = 0x00;
	*(unsigned int*)(io_p2v(0x40028044)) = (1 << 1); 
	
	/* Start*/
    data  = READ_REG(ADC_CTRL);
    data |= AD_PDN_STROBE;
    WRITE_REG(data, ADC_CTRL);
}

static irqreturn_t adc_interrupt (int irq, void * dev_id)
{	
	
    adc_values[adc_channel] = READ_REG(ADC_VALUE) & ADC_VALUE_MASK;
    

    if (gpi_interrupt_print)
	{
		printk(KERN_WARNING "ADC(%d)=%d\n", adc_channel, adc_values[adc_channel]);
		adc_channel++;
		if (adc_channel < ADC_NUMCHANNELS)
		{
			adc_start (adc_channel);
		}
	}else{
		adc_interrupt_flag = true;
		wake_up_interruptible(&adc_interrupt_event);
	}
	
    return (IRQ_HANDLED);
}

static irqreturn_t gp_interrupt(int irq, void * dev_id)
{	
	gpi_interrupt_print = true;
    adc_start (0);

    return (IRQ_HANDLED);
}


static void adc_exit (void)
{
	free_irq (IRQ_LPC32XX_GPI_01, NULL);
	free_irq (IRQ_LPC32XX_TS_IRQ, NULL);
}


static ssize_t device_read (struct file * file, char __user * buf, size_t length, loff_t * f_pos)
{
	DriverInfo* info = (DriverInfo*)file->private_data;
	int channel = info->minor;
	
	int bytes_remaining, msg_length = 0;
	if (*f_pos != 0) { return 0; }
	
    printk (KERN_WARNING DEVICE_NAME ":device_read(%d)\n", channel);

    if (channel < 0 || channel >= ADC_NUMCHANNELS)
    {
		return -EFAULT;
    }
	
	adc_interrupt_flag = false;
	
	
    adc_start (channel);
   
    wait_event_interruptible(adc_interrupt_event, adc_interrupt_flag ); 
    
	msg_length = sprintf(info->msg, "%d", adc_values[adc_channel]);
	
    bytes_remaining = copy_to_user(buf, info->msg, msg_length);
    *f_pos += msg_length - bytes_remaining;
    
    return msg_length - bytes_remaining;
}



static int device_open (struct inode * inode, struct file * file)
{
    DriverInfo* info;
    file->private_data = kmalloc(sizeof(DriverInfo), GFP_KERNEL);
    info = (DriverInfo*)file->private_data;
    info->minor = MINOR(inode->i_rdev);
    try_module_get(THIS_MODULE);
    
    return 0;
}


static int device_release (struct inode * inode, struct file * file)
{
    kfree(file->private_data);

    module_put(THIS_MODULE);
	return 0;
}


static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .read = device_read,
    .open = device_open,
    .release = device_release
};


static struct chardev
{
    dev_t       dev;
    struct cdev cdev;
} adcdev;


int adcdev_init (void)
{
    // try to get a dynamically allocated major number
	int error = alloc_chrdev_region(&adcdev.dev, 0, ADC_NUMCHANNELS, DEVICE_NAME);;

	if(error < 0)
	{
		// failed to get major number for our device.
		printk(KERN_WARNING DEVICE_NAME ": dynamic allocation of major number failed, error=%d\n", error);
		return error;
	}

	printk(KERN_INFO DEVICE_NAME ": major number=%d\n", MAJOR(adcdev.dev));

	cdev_init(&adcdev.cdev, &fops);
	adcdev.cdev.owner = THIS_MODULE;
	adcdev.cdev.ops = &fops;

	error = cdev_add(&adcdev.cdev, adcdev.dev, ADC_NUMCHANNELS);
	if(error < 0)
	{
		// failed to add our character device to the system
		printk(KERN_WARNING DEVICE_NAME ": unable to add device, error=%d\n", error);
		return error;
	}

	adc_init();
	 
	return 0;
}


/*
 * Cleanup - unregister the appropriate file from /dev
 */
void cleanup_module()
{
	cdev_del(&adcdev.cdev);
	unregister_chrdev_region(adcdev.dev, ADC_NUMCHANNELS);

	adc_exit();
}


module_init(adcdev_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Skip Geldens & Stefan Grimminck");
MODULE_DESCRIPTION("ADC Driver");

