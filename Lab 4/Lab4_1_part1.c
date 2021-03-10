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

//global buffer
char buffer[2];
//delcaring time_stamp
struct timeval time_stamp;

//running ReadBPE at the same time with main function because of pthread
void* ReadBPE()
{
	//open pipe
	int fd;
	char* filename = "/tmp/BP_pipe";
	fd = open(filename, O_RDONLY);
	//initialize buffer for BP_pipe, the buffer is time_stamp that's why its in struct
	struct timeval Xbp;
	struct timeval X1;
	struct timeval X2;
	//initialize size of file
	int bytes_read;

	//initialize interpolation variables
	double Y1, Y2, m, Ybp;

	
	while (1)
	{
		//read from "/tmp/BP_pipe", its updating the buffer by reading it
		bytes_read = read(fd, &Xbp, sizeof(int));
		
		Y1 = buffer[0];
		X1 = time_stamp;
		
		//waiting for buffer to receive the second GPS data for Y2
		while (Y1 == buffer[0])
		{
		}
		//after buffer received the second GPS data, we can initialize Y2 and X2
		Y2 = buffer[0];
		X2 = time_stamp;
		

		//converting struct into integer
		int X2_micro = (X2.tv_sec)*1000000 + (int)X2.tv_usec;
		int X1_micro = (X1.tv_sec)*1000000 + (int)X1.tv_usec;
		int Xbp_micro = (Xbp.tv_sec)*1000000 + (int)Xbp.tv_usec;
		
		//interpolation
		//Find a slope
		m = (Y2 - Y1) / (X2_micro - X1_micro);

		//Find a Y button pressed
		Ybp = m * (Xbp_micro - X1_micro) + Y1;
		
		printf("\n");
		printf("Y1:  %d \n", Y1);
		printf("X1:  %d \n", X1);
		printf("Y2:  %d \n", Y2);
		printf("X2:  %d \n", X2);
		printf("Ybp: %d \n", Ybp);
		printf("Xbp: %d \n", Xbp);
	}
}


int main(void)
{
	//open pipe
	int fd;
	char* filename = "/tmp/N_pipe1";
	//reading only, receiving data
	fd = open(filename, O_RDONLY);
	
	//initialize size of file
	int bytes_read;

	//create pthread for "read bpe" which is for button pressed program
	pthread_t thread;

	pthread_create(&thread, NULL, ReadBPE, NULL);
	
	while (1)
	{
		//read from pipe, store temporaly and read each characters
		bytes_read = read(fd, &buffer, sizeof(char));
		//get time stamp (system time)
		gettimeofday(&time_stamp, NULL);
	}

	
	return 0;
}
