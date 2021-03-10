#include <wiringPi.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <time.h>

//Base Address & OFFSET
#define BASE_ADDRESS		0x3F200000 //gpfsel0 base address
#define CAPACITY			4096 //4 bit memory size
#define GPFSEL1_OFFSET		0x01

int main(void)
{
	//initialize priority, prioritize pressing the button
	struct sched_param sp = { .sched_priority = 50 };
	
	//initialize opening file
	int mem_fd;

	//Initialize RT-TASK
	int ret;
	
	//Opening a file
	if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
	{
		printf("Can't open /dev/mem \n");
		exit(-1);
	}
	
	//mmap GPIO
	unsigned long* gpio_map;
	gpio_map = mmap(NULL, CAPACITY, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, BASE_ADDRESS);
	unsigned long* gpeds0 = gpio_map + 0x40 / 4; // GPEDS0_OFFSET IS 0x10

	//creating a pipe
	mkfifo("/tmp/BP_Pipe", 777);

	//opening a pipe
	int fd;
	char* filename = "/tmp/BP_Pipe";
	int bytes_read;
	//writing into a pipe
	fd = open(filename, O_WRONLY);

	//Set scheduler
	ret = sched_setscheduler(0, SCHED_FIFO, &sp);

	if (ret = -1)
	{
		printf("Set_Scheduler failed \n");
	}
	//get the current time
	struct timespec next_period;
	clock_gettime(CLOCK_MONOTONIC, &next_period);

	//set the period (60ms)
	next_period.tv_nsec = 60000000;
	
	//delcaring time_stamp
	struct timeval time_stamp;
	
	while (1)
	{
		//if button has presssed down
		if (gpeds0 == 0b10000000000000000)
		{
			//get detect time
			gettimeofday(&time_stamp, NULL);
			//writing into the pipe
			bytes_read = write(fd, &time_stamp, sizeof(struct timeval));

			//clear register gpeds; bit -> 1
			*gpeds0 = 0b10000000000000000;
		}
		//wait until next-period , 120ms
		next_period.tv_nsec = next_period.tv_nsec + 60000000;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_period, NULL);
	}
}
