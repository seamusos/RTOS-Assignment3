/**********************************************************************************

           *************NOTE**************
This is a template for the subject of RTOS in University of Technology Sydney(UTS)
Please complete the code based on the assignment requirement.

Assignment 3 Program_2 template

**********************************************************************************/
/*
  To compile prog_1 ensure that gcc is installed and run the following command:
  gcc -Wall -O2 program_1.c -o prog_1 -lpthread -lrt

*/

#include <pthread.h> 	/* pthread functions and data structures for pipe */
#include <unistd.h> 	/* for POSIX API */
#include <stdlib.h> 	/* for exit() function */
#include <stdio.h>	/* standard I/O routines */
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define NUM_PROCESSES 8

/* Initializes data and utilities used in thread params */
void initializeData(ThreadParams *params);

typedef struct SRTF_Params {
  //add your variables here
  sem_t sem1, sem2;
  int arriveTime;
  int burst;
  int remBurst;

  char write_file[100];
  
} Process_Params;



/* this function calculates CPU SRTF scheduling, writes waiting time and turn-around time to th FIFO */
void *worker1(void *params)
{
	// add your code here
	Process_Params *worker1_params = (Process_Params *)(params);

	int endTime, time, shortest, remain = 0;

	for(time = 0; remain != NUM_PROCESSES; time++)
	{
		if (/* condition */)
		{
			/* code */
		}
		
	} 

	sem_post();

	return 0;

}

/* reads the waiting time and turn-around time through the FIFO and writes to text file */
void *worker2()
{
   // add your code here
}

/* this main function creates named pipe and threads */
int main(void)
{
	pthread_t tid1, tid2; //Thread ID
  	pthread_attr_t attr;

	Process_Params params;


	/* creating a named pipe(FIFO) with read/write permission */
	// add your code 


	/* initialize the parameters */
	// add your code 
	
	/* create threads */
	// add your code
	// Initialization
	initializeData(&params);
	pthread_attr_init(&attr);

	if(pthread_create(&tid1, &attr, worker1, (void *)(&params)) != 0)
	{
		perror("Error Creating Thread");
		exit(-1);
	}

	if(pthread_create(&tid2, &attr, worker2, (void *)(&params)) != 0)
	{
		perror("Error Creating Thread");
		exit(-1);
	}
	
	/* wait for the thread to exit */
	//add your code
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);	
	return 0;
}

void initializeData(ThreadParams *params)
{

  // Initialize Sempahores
  if(sem_init(&(params->sem1), 0, 1))
  {
    perror("Error initalising thread");
    exit(0);
  }
  if(sem_init(&(params->sem2), 0, 0))
  {
    perror("Error initalising thread");
    exit(0);
  }
  return;
}
