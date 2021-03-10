#include <wiringPi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

//Base Address & Offset
#define BASE_ADDRESS 		0x3F200000 // gpfsel0 base address
#define CAPACITY 		4096 // 4 bit memory size
#define GPFSEL1_OFFSET		0x01
#define GPEDS0_OFFSET 		0x10


#define RED 	8   //GPIO 2 LED RED
#define YELLOW 	9   //GPIO 3 LED YELLOW
#define GREEN 	7   //GPIO 4 LED GREEN
#define BCM16 	27 //GPIO 16 Button

int main(void)
{
    int mem_fd;


    //Opening a file    
    if ((mem_fd = open ("/dev/mem", O_RDWR|O_SYNC) ) < 0)
    {
        printf("can't open /dev/mem \n");
        exit(-1);
    }
    unsigned long *gpio_map;
    //mmap GPIO
    gpio_map = mmap(NULL, CAPACITY, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, BASE_ADDRESS);
    unsigned long *gpeds0 =  gpio_map + 0x40/4;

	//Opens the GPIO devices and allows program to access it
	wiringPiSetup();
	pinMode (RED, OUTPUT);
	pinMode (YELLOW, OUTPUT);
	pinMode (GREEN, OUTPUT);
    	pinMode (BCM16, INPUT);
	
	printf ("%x \n", gpio_map);
	printf ("%d \n", *gpio_map);
	printf ("%x \n", gpeds0);
	printf ("%d \n", *gpeds0);
	

	// infinite loop
	for(;;)
	{
		//Outputs a value (0 or 1) to the given pin		
		digitalWrite(RED, 1); // ON, HIGH
		//delays for a number of milliseconds		
		delay (600); //mS
		digitalWrite(RED, 0); // OFF, LOW
		delay (400);
		digitalWrite(YELLOW, 1); 
		delay (600);
		digitalWrite(YELLOW, 0);
		delay (400);
	
		// If button has pressed down, this is not a interrupter use GPEDS
		if(*gpeds0 == 0b10000000000000000)
		{    
		    printf("Been here\n");
		    digitalWrite(GREEN, 1);
		    delay (600);
	            digitalWrite(GREEN, 0);
	            delay (400);
		    
		    //Clear register gpeds; bit 16 -> 1
		    *gpeds0 = 0b10000000000000000;
		}
	}

	
	

	return 0;
}
