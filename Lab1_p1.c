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

#define BASE_ADDRESS 0x3F200000 // gpfsel0 base address

//Turning on LED function
int turn_on_LED(void) //Kernel sometimes accept the value so for safety I include void
{
    
    unsigned long *gpfsel0 = (unsigned long*)ioremap(BASE_ADDRESS, 4096); // 4096 is a memory size
    
    //we add gpfsel0 with 0x07 (OFFSET) to GET the address of gpset0 (which is 0x3F20001C)
    unsigned long *gpset0 = gpfsel0 + 0x07; // this equivalent to unsigned long *gpset0 = (unsigned long*)ioremap(0x3F20001C, 4096); but it uses more mem
    
    //printing address to see if I have the right address
    printk("%X", gpfsel0);
    printk("%X", gpset0);
    
    //Shift 1 bit by three times to right so 000000001 ---> 00001000 because the number of pin for red is 3
    iowrite32(1 << 3,gpfsel0); // function iowrite32 is writing to set the pin mode (input/output) which is 3(LED RED)
    iowrite32(1 << 3,gpset0); // Writing into GPFSET0 with turn on the corresponding LED 

    return 0;
}

//Turning off LED function, remember you are coding on function so you have to assign all the gpfsel,gpset,and gpclr again
void turn_off_LED(void)
{
    unsigned long *gpfsel0 = (unsigned long*)ioremap(BASE_ADDRESS, 4096);
    unsigned long *gpset0 = gpfsel0 + 0x07;
    unsigned long *gpclr0 = gpfsel0 + 0x0A;

    iowrite32(1 << 6,gpsel0); // Writing into GPCLR0 will turn off the LED
    iowrite32(1 << 3, gpclr0);
    iounmap(gpfsel10);
}


module_init(turn_on_LED);
module_init(turn_off_LED);