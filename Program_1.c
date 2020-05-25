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

#define NUM_PROCESSES 7

#define DEFAULT_WRITE_FILE "output.txt"

typedef struct fifo
{
	int *fd;
	int items;
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
	process_data_t *processData;
	fifo_t *fifo;
	char write_file[100];
} Process_Params;

sem_t sem_SRTF;

pthread_t tid1, tid2; //Thread ID
pthread_attr_t attr;

/* Initializes data and utilities used in thread params */
void initializeData(process_data_t *processData);

/* This Thread runs the SRTF program to calculate scheduled times*/
void *worker1(void *params);

void *worker2(void *params);

void pushToFIFO(fifo_t *fifo, char *string, float value);

/* this main function creates named pipe and threads */
int main(int argc, char const *argv[])
{

	Process_Params params;
	int items = 0;	


	if (argc != 2)
	{
		printf("No User Defined Output File Name, resulting to default file name \n");
		strcpy(params.write_file, DEFAULT_WRITE_FILE);
	}
	else
	{
		printf("User Defined Write File = %s \n", argv[1]);
		//Copy Text File Names from Arguements
		strcpy(params.write_file, argv[1]);
	}

	char *myfifo = "./myfifo";	//Declare FIFO Path
	int fd;

	/* Configuring FIFO */
	if (mkfifo(myfifo, 0666) == -1)
	{
		perror("Error creating FIFO");
	}

	fd = open(myfifo, O_RDWR);
	if (fd == -1)
	{
		perror("Error opening FIFO");
	}

	//Add FIFO Variables
	fifo_t fifo;
	fifo.fd = &fd;
	fifo.items = items;
	fifo.eSize = 100;

	//Allocate array for process Data
	process_data_t *processData;
	processData = malloc(sizeof(process_data_t) * NUM_PROCESSES);
	initializeData(processData);

	params.fifo = &fifo;
	params.processData = processData;

	pthread_attr_init(&attr);

	//Create Threads
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

	/* Waiting for Thread To Exit*/
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	//Close link to FIFO
    if(close(fd) == -1)
    {
        perror("Error closing FIFO.\n"); exit(1);
    }
	//Remove FIFO 
	if(unlink(myfifo) == -1)
    {
        perror("Error removing FIFO\n"); exit(1);
    }

	free(processData);

	return 0;
}

/* this function calculates CPU SRTF scheduling, writes waiting time and turn-around time to th FIFO */
void *worker1(void *params)
{
	Process_Params *worker1_params = (Process_Params *)(params);
	process_data_t *process = worker1_params->processData;

	int endTime, remaining = 0;

	float wait_t, turn_t; 
	float avgWait_t = 0;
	float avgTurn_t = 0;

	for (int time = 0; remaining != NUM_PROCESSES; time++)
	{

		int shortestIndex = 0;

		//Scan all proccess for shortest remaining time
		for (int i = 0; i <= NUM_PROCESSES; i++)
		{
			if (process[i].arrive_t <= time && process[i].remain_t > 0)
			{
				if (process[i].remain_t < process[shortestIndex].remain_t)
				{
					shortestIndex = i;
				}
			}
		}

		if (process[shortestIndex].arrive_t <= time)
		{
			process[shortestIndex].remain_t--; //Simulate decreasing process time

			if (process[shortestIndex].remain_t == 0)
			{
				remaining++;
				endTime = time + 1;

				turn_t = endTime - process[shortestIndex].arrive_t;
				wait_t = endTime - process[shortestIndex].burst_t - process[shortestIndex].arrive_t;

				avgWait_t += wait_t;
				avgTurn_t += turn_t;
			}
		}
	}

	avgWait_t /= NUM_PROCESSES;
	avgTurn_t /= NUM_PROCESSES;

	fprintf(stdout, "Average Wait Time = %f \n", avgWait_t);
	fprintf(stdout, "Average Turnaround Time = %f \n", avgTurn_t);

	pushToFIFO(worker1_params->fifo, "Average Wait Time: ", avgWait_t);
	pushToFIFO(worker1_params->fifo, "Average Turnaround Time: ", avgTurn_t);

	sem_post(&sem_SRTF);

	return 0;
}

/* reads the waiting time and turn-around time through the FIFO and writes to text file */
void *worker2(void *params)
{
	Process_Params *worker2_params = params;

	char buffer[100];

	printf("Opening Write File\n");
	FILE *writeFile = fopen(worker2_params->write_file, "w");
	if (!writeFile)
	{
		perror("Invalid File\n");
		exit(0);
	}

	sem_wait(&sem_SRTF); //Wait for semaphore

	for (int i = 0; i < worker2_params->fifo->items; i++)
	{
		printf("Reading From FIFO");
		if (read(*worker2_params->fifo->fd, buffer, sizeof(buffer)) < 0)
		{
			perror("Failed to Read from FIFO");
			exit(1);
		}

		printf("Writing to File: %s\n", buffer);

		if (fputs(buffer, writeFile) == EOF)
		{
			perror("error writing to file");
			exit(0);
		}
	}

	if (fclose(writeFile) == -1)
	{
		perror("Error Closing File");
		exit(1);
	}

	pthread_cancel(tid1);
	pthread_cancel(tid2);

	printf("Program Finished \n");

	return 0;
}

void initializeData(process_data_t *processData)
{
	printf("Init Params \n");

	// Initialize Sempahores
	if (sem_init(&sem_SRTF, 0, 0))
	{
		perror("Error initalising semaphore");
		exit(0);
	}

	processData[0].pid = 1;	processData[0].arrive_t = 8;	processData[0].burst_t = 10;
	processData[1].pid = 2;	processData[1].arrive_t = 10;	processData[1].burst_t = 3;
	processData[2].pid = 3;	processData[2].arrive_t = 14;	processData[2].burst_t = 7;
	processData[3].pid = 4;	processData[3].arrive_t = 9;	processData[3].burst_t = 5;
	processData[4].pid = 5;	processData[4].arrive_t = 16;	processData[4].burst_t = 4;
	processData[5].pid = 6;	processData[5].arrive_t = 21;	processData[5].burst_t = 6;
	processData[6].pid = 7;	processData[6].arrive_t = 26;	processData[6].burst_t = 2;

	//Initialise remaining time to be same as burst time
	for (int i = 0; i < 7; i++)
	{
		processData[i].remain_t = processData[i].burst_t;
	}
}

void pushToFIFO(fifo_t *fifo, char *string, float value)
{
	char buffer[100];
	snprintf(buffer, sizeof(buffer), "%s%f \n", string, value);
	printf("Writing to FIFO: %s \n", buffer);
	if (write(*fifo->fd, buffer, sizeof(buffer)) == -1)
	{
		perror("Failed to Write to FIFO");
		exit(1);
	}
	fifo->items++; 	//Increases amount of items in FIFO
}