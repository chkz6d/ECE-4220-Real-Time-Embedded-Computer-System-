#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <sys/timerfd.h>

#define BUFFER_MAX 300

struct period
{
    struct timespec next_period;
    long period_ns;
    char *p;
};

struct sched_param param;

char buffer[BUFFER_MAX];

void *Read(void *ptr)
{
    struct period *read = (struct period*)ptr;
    clock_gettime(CLOCK_MONOTONIC, &(read->next_period));
    read->next_period.tv_nsec = read->next_period.tv_nsec + 1500000;

    FILE *fileopen;
    fileopen = fopen(read->p, "r");

    while(fgets(buffer, BUFFER_MAX, fileopen) != NULL)
    {
        puts(buffer);
        read->next_period.tv_nsec = read->next_period.tv_nsec + 1500000;
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &read->next_period, NULL);
    }

    fclose(fileopen);
    return NULL; //returns nothing, it's always a good practice to return NULL
}

void *Write(void *ptr)
{
    struct period *write = (struct period*)ptr;
    clock_gettime(CLOCK_MONOTONIC, &(write->next_period));
    write->next_period.tv_nsec = write->next_period.tv_nsec + 1000000;

    for (int i = 0; i < 20; i++)
    {
        printf("%s", buffer);
        write->next_period.tv_nsec = write->next_period.tv_nsec + 1000000;
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &write->next_period,NULL);
    }

    return NULL;
}
int main()
{
    int priority = 51; //kernel is priority 50
    struct period *ptr = malloc(sizeof(struct period) * 3);
    ptr[0].p = "first.txt";
    ptr[1].p = "second.txt";
    
    pthread_t thread[3];
    
    param.sched_priority = priority;

    pthread_create(&thread[0], NULL, Read, &ptr[0]);
    pthread_create(&thread[1], NULL, Read, &ptr[1]);
    pthread_create(&thread[2], NULL, Write, &ptr[2]);

    //for loop if the pthread is in array
    for (int i = 0; i < 3; i++)
    {
        pthread_join(thread[i], NULL);
    }
    return 0;
}
