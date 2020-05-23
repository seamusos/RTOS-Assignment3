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

#include <pthread.h> /* pthread functions and data structures for pipe */
#include <unistd.h>	 /* for POSIX API */
#include <stdlib.h>	 /* for exit() function */
#include <stdio.h>	 /* standard I/O routines */
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

#define DEFAULT_WRITE_FILE "output.txt"

/* Initializes data and utilities used in thread params */
void initializeData(ThreadParams *params);

typedef struct fifo
{
	int *fd;
	int *items;
	int eSize;
} fifo_t;

typedef struct proccess_data
{
	int pid;
	int arrive_t;
	int burst_t;
	int remain_t;
} process_data_t;

// Struct Holding Thread Parameters
typedef struct SRTF_Params
{
	//add your variables here
	//sem_t sem1, sem2;
	//process_data_t *processData;
	fifo_t *fifo;
	char write_file[100];
} Process_Params;

sem_t sem_SRTF;

process_data_t processData[9];

void initializeData(ProcessParams *params);

/* This Thread runs the SRTF program to calculate scheduled times*/
void *worker1(void *params);

void *worker2(void *params);


/* this main function creates named pipe and threads */
int main(void)
{
	pthread_t tid1, tid2; //Thread ID
	pthread_attr_t attr;

	Process_Params params;

	if (argc != 2)
	{
		printf("No User Defined Output File Name, resulting to default file names \n");
		strcpy(params.write_file, DEFAULT_WRITE_FILE);
	}
	else
	{
		printf("User Defined Write File = %s \n", argv[1]);
		//Copy Text File Names from Arguements
		strcpy(params.write_file, argv[1]);
	}

	/* creating a named pipe(FIFO) with read/write permission */
	// add your code

	char *myfifo = "./myfifo";
	int fd;

	if (mkfifo(myfifo, 0666) == -1)
	{
		perror("Error creating FIFO");
	}

	fd = open(myfifo, O_RDWR);
	if (fd == -1)
	{
		perror("Error opening FIFO");
	}

	/* initialize the parameters */
	// add your code

	/* create threads */
	// add your code
	// Initialization
	initializeData(&params);
	pthread_attr_init(&attr);

	if (pthread_create(&tid1, &attr, worker1, (void *)(&params)) != 0)
	{
		perror("Error Creating Thread");
		exit(-1);
	}

	if (pthread_create(&tid2, &attr, worker2, (void *)(&params)) != 0)
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

/* this function calculates CPU SRTF scheduling, writes waiting time and turn-around time to th FIFO */
void *worker1(void *params)
{
	// add your code here
	Process_Params *worker1_params = (Process_Params *)(params);

	int endTime, time, remaining = 0;

	int shortestIndex = 0;

	float wait_t, turn_t, avgWait_t, avgTurn_t

	for (time = 0; remaining != NUM_PROCESSES; time++)
	{

		//Scan all proccess for shortest remaining time
		for(int i = 0; i < NUM_PROCESSES; i++)
		{
			if (processData[i].arrive_t <= time && processData.remain_t < processData[shortestIndex].remain_t && processData[i].remain_t > 0)
			{
				/* code */
				shortestIndex = i;
			}
		}

		processData[shortestIndex].remain_t--;	//Simulate decresing process time

		if(processData[shortestIndex].remain_t == 0)
		{
			remaining++;

			endTime = time + 1;

			turn_t = endTime - processData[shortestIndex].arrive_t;
			wait_t = endTime - processData[shortestIndex].burst_t - processData[shortestIndex].arrive_t;

			avgWait_t += wait_t;
			avgTurn_t += turn_t;
		}

	}

	avgWait_t /= NUM_PROCESSES;
	avgTurn_t /= NUM_PROCESSES;

	char buffer[100];

	buffer = "Average Wait Time: ";
	strcat(buffer, avgWait_t);

	if(write(worker1_params->fifo->fd, buffer, sizeof(buffer)) == -1)
	{
		perror("Failed to Write to FIFO");
		exit(1);
	}

	buffer = "Average Turnaround Time: ";
	strcat(buffer, avgTurn_t);

	if(write(worker1_params->fifo->fd, buffer, sizeof(buffer)) == -1)
	{
		perror("Failed to Write to FIFO");
		exit(1);
	}
	sem_post(sem_SRTF);

	return 0;
}

/* reads the waiting time and turn-around time through the FIFO and writes to text file */
void *worker2()
{
	// add your code here
	Process_Params *worker2_params = (Process_Params *)(params);

	char buffer[100];

	printf("Opening Write File\n");
	FILE* writeFile = fopen(worker2_params->write_file, "w");
	if (!writeFile)
	{
		perror("Invalid File\n");
		exit(0);
	}

	sem_wait(sem_SRTF); //Wait for semaphore

	for(int i = 0; i <= worker2_params->fifo->items; i++)
	{
		if(read(worker1_params->fifo->fd, buffer, sizeof(buffer)) == -1)
		{
			perror("Failed to Read from FIFO")
			exit(1);
		}

		fputs(buffer, writeFile);		
	}

	if(fclose(writeFile) == -1)
	{
		perror("Error Closing File");
		exit(1);
	}

	pthread_cancel(tidA);
	pthread_cancel(tidB);

	return 0;
}

void initializeData(ThreadParams *params)
{

	// Initialize Sempahores
	if (sem_init(&sem_SRTF, 0, 0))
	{
		perror("Error initalising semaphore");
		exit(0);
	}


	processData[0].pid = 1; processData[0].arrive_t = 8;	processData[0].burst_t = 10;
	processData[1].pid = 2; processData[1].arrive_t = 10;	processData[1].burst_t = 3;
	processData[2].pid = 3; processData[2].arrive_t = 14;	processData[2].burst_t = 7;
	processData[3].pid = 4; processData[3].arrive_t = 9;	processData[3].burst_t = 5;
	processData[4].pid = 5; processData[4].arrive_t = 16;	processData[4].burst_t = 4;
	processData[5].pid = 6; processData[5].arrive_t = 21;	processData[5].burst_t = 6;
	processData[6].pid = 7; processData[6].arrive_t = 26;	processData[6].burst_t = 2;
	//processData[7].pid = 8; processData[7].arrive_t = 30; processData[7].burst_t = 13;
	
	//Initialise remaining time to be same as burst time
	for (i = 0; i < PROCESSNUM; i++) {
		processData[i].remain_t = processData[i].burst_t;
	}
}