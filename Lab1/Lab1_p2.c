#ifndef MODULE
#define MODULE
#endif
#ifndef KERNEL
#define KERNEL
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

//Base Address & Offset
#define BASE_ADDRESS 0x3F200000 // gpfsel0 base address
#define CAPACITY 4096
#define GPFSEL1_OFFSET 0x01
#define GPAREN0_OFFSET 0x1F
#define GPEDS0_OFFSET 0x10
#define GPPUD_OFFSET 0x25
#define GPPUDCLK0_OFFSET 0x26

//To identify the handler
int mydev_id;

//Interrupt handler, this is the function when it gets interrupted the function is activated
static irqreturn_t button_isr(int irq, void *dev_id)
{
    //halt(disable) the interrupt
    disable_irq_nosync(79);
    //ASSIGN GPIO address
    unsigned long *gpfsel0 = (unsigned long*)ioremap(BASE_ADDRESS, CAPACITY);
    unsigned long *gpeds0 = gpfsel0 + GPEDS0_OFFSET;
    

    unsigned long button = ioread32(GPEDS0);

    iowrite32(0b00000000000011111000000000000000, gpeds0);
    //to check if the button has been pushed
    printk("Interrupt successfully handled");
    printk("");

    return IRQ_HANDLED;
    

}
//initial module installation 
int my_init(void)
{
    int dummy = 0;
    //ASSIGN GPIO address
    unsigned long *gpfsel0 = (unsigned long*)ioremap(BASE_ADDRESS,CAPACITY);
    unsigned long *gparen0 = gpfsel0 + GPAREN0_OFFSET;
	unsigned long *gppud = gpfsel0 + GPPUD_OFFSET;
	unsigned long *gppudclk0 = gpfsel0 + GPPUDCLK0_OFFSET;
	
	//set input bcm16
	iowrite32(0x0, gpfsel0);
	
	//Triggering interrupt, GPPUD configuration (PULLDOWN)
	iowrite32(0b01, gppud);
	udelay(100);
	iowrite32(0b00000000000011111000000000000000,gppudclk0);
	udelay(100);
	
	//set gparen0
	iowrite32(0b00000000000011111000000000000000,gparen0);
	

	//call ISR
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);
	
	//enable(79)
	enable_irq(79);

	printk("Button Detection enabled.\n");
	return 0;
}

void my_cleanup(void)
{
	//free_irq
	free_irq(79, &mydev_id);
	
	printk("All are cleanup.\n");
}

module_init(my_init);
module_exit(my_cleanup);