#include <wiringPi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

//Base Address & Offset
#define BASE_ADDRESS 		0x3F200000 // gpfsel0 base address
#define CAPACITY 		4096 // 4 bit memory size
#define GPFSEL1_OFFSET		0x01
#define GPEDS0_OFFSET 		0x10

#define RED 	8   //GPIO 2 LED RED
#define YELLOW 	9   //GPIO 3 LED YELLOW
#define GREEN 	7   //GPIO 4 LED GREEN

//Global Variables
sem_t mySem;

//thread function needs to be a pointer as well as void *ptr
void *LED1_Thread(void *ptr)
{
	//set up RT-Task
	int ret;
	//setting up the priority	
	struct sched_param sp = { .sched_priority = 53};

	ret = sched_setscheduler(0, SCHED_FIFO, &sp);
	
	//check to see if scheduler failed
	if(ret == -1)
	{
		printf("Failed to do scheduler for Thread #1\n");
	}

	while(1)
	{
		//Lock
		sem_wait(&mySem);

		//Outputs a value (0 or 1) to the given pin		
		digitalWrite(RED, HIGH); // ON, HIGH
		//delays for a number of milliseconds		
		delay(600); //mS
		digitalWrite(RED, LOW); // OFF, LOW

		//Unlock
		sem_post(&mySem);
		usleep(10);
	}
	
}

void *LED2_Thread(void *ptr)
{
	//set up RT-Task
	int ret;
	//setting up the priority	
	struct sched_param sp = { .sched_priority = 52};

	ret = sched_setscheduler(0, SCHED_FIFO, &sp);

	//check to see if scheduler failed
	if (ret == -1)
	{
		printf("Failed to do scheduler for Thread #2\n");
	}

	while (1)
	{
		//Lock
		sem_wait(&mySem);

		digitalWrite(YELLOW, HIGH);
		delay(600);
		digitalWrite(YELLOW, LOW);


		//Unlcok
		sem_post(&mySem);
		usleep(10);
	}
}

//This function applies when the button is pressed
void *Ped_Thread(void *ptr)
{
	//Set up RT-Task
	int ret;
	//setting up the priority	
	struct sched_param sp = { .sched_priority = 52};
	
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
	//set scheduler
	ret = sched_setscheduler(0, SCHED_FIFO, &sp);

	//check to see if scheduler failed
	if (ret == -1)
	{
		printf("Failed to do scheduler for Thread #3\n");
	}
	
	while (1)
	{
		if (*gpeds0 == 0b10000000000000000)
		{
			//Lock
			sem_wait(&mySem);
			
			digitalWrite(GREEN, HIGH);
			delay(600);
			digitalWrite(GREEN, LOW);

			//Clear register gpeds; bit 16 -> 1
			*gpeds0 = 0b10000000000000000;

			//Unlock
			sem_post(&mySem);
			usleep(10);
		}
	}
}


int main(void)
{

	//priority
	struct sched_param sp = { .sched_priority = 50};
	
	int ret;

	ret = sched_setscheduler(0, SCHED_FIFO, &sp);
	
	if (ret == -1)
	{
		printf("Set_Scheduler failed\n");
	}

	//semaphore initialize 
	sem_init(&mySem, 0, 1);

	//Opens the GPIO devices and allows program to access it
	wiringPiSetup();
	pinMode (RED, OUTPUT);
	pinMode (YELLOW, OUTPUT);
	pinMode (GREEN, OUTPUT);
	
	//create pthread
	pthread_t thread[3];

	pthread_create(&thread[0], NULL, LED1_Thread, NULL);
	pthread_create(&thread[1], NULL, LED2_Thread, NULL);
	pthread_create(&thread[2], NULL, Ped_Thread, NULL);

	//join pthread
	for (int i = 0; i < 3; i++)
	{
		pthread_join(thread[i], NULL);
	}
	return 0;
}
