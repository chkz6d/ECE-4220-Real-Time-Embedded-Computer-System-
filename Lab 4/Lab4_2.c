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
#include <semaphore.h>

//Semaphores
sem_t mySem;
//global buffer
char buffer[2];
//delcaring time_stamp
struct timeval time_stamp;

struct data
{
	struct timeval x_data[3];
	int y_data[3];			
};

//read the print_data from the pipe and print them all
void* print_function()
{
	//intializing struct for print_data
	struct data print_data;
	
	
	//opening a pipe
	int fd;
	fd = open("/tmp/Print_pipe", O_RDONLY);
	//reading the pipe data
	read(fd, &print_data, sizeof(struct data));
	//Lock
	sem_wait(&mySem);
	
	//printing the data
	printf("Y1:  %d", print_data.y_data[0]);
	printf("X1:  %d", print_data.x_data[0]);
	printf("Y2:  %d", print_data.y_data[1]);
	printf("X2:  %d", print_data.x_data[1]);
	printf("Ybp:  %d", print_data.y_data[2]);
	printf("Xbp:  %d", print_data.x_data[2]);
	
	//Unlock
	sem_post(&mySem); 
}

//Calculating interpolation and sending the print_data to the pipe
void* interpolation(void* ptr)
{
	//convert void pointer ptr into struct timeval
	struct timeval *Xbp = (struct timeval*)ptr;
	//intializing struct for print_data, Y1,Y2,Ybp, X1,X2,Xbp
	struct data print_data;
	
	print_data.x_data[2] = *Xbp;
	//initialize interpolation variable
	int m;

	print_data.y_data[0] = buffer[0];
	print_data.x_data[0] = time_stamp;

	//waiting for buffer to receive the second GPS data for Y2
	while (print_data.y_data[0] == buffer[0])
	{
	}
	//after buffer received the second GPS data, we can initialize Y2 and X2
	print_data.y_data[1] = buffer[0];
	print_data.x_data[1] = time_stamp;
	
	//converting struct into integer
	int X2_micro = (print_data.x_data[1].tv_sec)*1000000 + print_data.x_data[1].tv_usec;
	int X1_micro = (print_data.x_data[0].tv_sec)*1000000 + print_data.x_data[0].tv_usec;
	int Xbp_micro = (print_data.x_data[2].tv_sec)*1000000 + print_data.x_data[2].tv_usec;

	//interpolation
	//Find a slope
	m = (print_data.y_data[1] - print_data.y_data[0]) / (X2_micro - X1_micro);

	//Find a Y button pressed
	print_data.y_data[2] = m * (Xbp_micro - X1_micro) + print_data.y_data[0];
	
	//opening a pipe
	int fd;
	fd = open("/tmp/Print_pipe", O_WRONLY);
	//creating print_thread
	pthread_t print_thread;
	pthread_create(&print_thread, NULL, print_function, NULL);
	//sending the data to the pipe
	write(fd, &print_data, sizeof(struct data));
}

//running ReadBPE at the same time with main function because of pthread
void* ReadBPE()
{
	//initialize Button pressed
	struct timeval *Xbp = malloc(sizeof(struct timeval) *1 );
	//open pipe
	int fd;
	char* filename = "/tmp/BP_pipe";
	fd = open(filename, O_RDONLY);
	//initialize size of file
	int bytes_read;

	while (1)
	{
		//read from "/tmp/BP_pipe", its updating the buffer by reading it
		bytes_read = read(fd, Xbp, sizeof(struct timeval));
		//creating 
		pthread_t thread;
		//
		//1st argument: passing the thread
		//2nd argument: passing the thread argument type
		//3rd argument: passing the function
		//4th argument: passing the function data
		pthread_create(&thread, NULL, interpolation, &Xbp);
	}
}


int main(void)
{
	//Semaphore initialize
	sem_init (&mySem, 0, 1);
	//creating pipe for Print_pipe
	mkfifo("/tmp/Print_pipe", 777);
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
		//read from pipe, store temporaly and read each characters, buffer is GPS data
		bytes_read = read(fd, &buffer, sizeof(char));
		//get time stamp (system time)
		gettimeofday(&time_stamp, NULL);
	}


	return 0;
}
