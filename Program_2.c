/**********************************************************************************

           *************NOTE**************
This is a template for the subject of RTOS in University of Technology Sydney(UTS)
Please complete the code based on the assignment requirement.

Assignment 3 Program_2 template

**********************************************************************************/
/*
  To compile prog_1 ensure that gcc is installed and run the following command:
  gcc -Wall -O2 program_2.c -o prog_2 -lpthread -lrt

*/
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

//Number of pagefaults in the program
int pageFaults = 0;

//Function declaration
void SignalHandler(int signal);

/**
 Main routine for the program. In charge of setting up threads and the FIFO.

 @param argc Number of arguments passed to the program.
 @param argv array of values passed to the program.
 @return returns 0 upon completion.
 */
int main(int argc, char* argv[])
{
	//Register Ctrl+c(SIGINT) signal and call the signal handler for the function.
	//add your code here

	signal(SIGINT, SignalHandler);

	if (argc != 2)
	{
		printf("Error: No User Input for Frame Size\n");
		exit(1);
	}
	
	int i;
	// reference number
	int REFERENCESTRINGLENGTH = 24;
	//Argument from the user on the frame size, such as 4 frames in the document
	int frameSize = atoi(argv[1]);
	//Frame where we will be storing the references. -1 is equivalent to an empty value
	uint frame[REFERENCESTRINGLENGTH];
	//Reference string from the assignment outline
	int referenceString[24] = {7,0,1,2,0,3,0,4,2,3,0,3,0,3,2,1,2,0,1,7,0,1,7,5};
	//Next position to write a new value to.
	int nextWritePosition = 0;
	//Boolean value for whether there is a match or not.
	bool match = false;
	//Current value of the reference string.
	int currentValue;

	//Initialise the empty frame with -1 to simulate empty values.
	for(i = 0; i < frameSize; i++)
	{
		frame[i] = -1;
	}

	//Loop through the reference string values.
	for(i = 0; i < REFERENCESTRINGLENGTH; i++)
	{
		match = false;
		currentValue = referenceString[i];
		
		printf("_______________________________\n\n");
		printf("Scanning Frame..\n");

		//Scan frame for exisiting page
		for (int j = 0; j < frameSize; j++)
		{
			/* Check if current Value is already in frame */
			if (currentValue == frame[j])
			{
				printf("Match, Already in Frame\n");
				match = true;
				break;	//Break For Loop, value already found
			}						
		}
		if (match == false) //No Exisiting Matching Values
		{
			pageFaults++;	//Incremenet Page Fault
			printf("Page Fault, Current Count = %d\n", pageFaults);
			frame[nextWritePosition] = currentValue; //Write into oldest position
			nextWritePosition++;	//Shift write position to next oldest point

			/* Wrap around to begining */
			if (nextWritePosition >= frameSize)
			{
				nextWritePosition = 0;
			}			
		}

		//Print current Fram
		printf("\nCurrent Frame: \n");
		printf("Reference \n%d \n--\n", currentValue);
		for(int j = 0; j < frameSize; j++)
		{
			printf("%d\n", frame[j]);	
		}
		printf("Page Frame\n\n");
	}

	//Sit here until the ctrl+c signal is given by the user.
	printf("Algorithum Finished, Waiting for User Signal (Ctrl + C)\n");
	while(1)
	{
		sleep(1);
	}

	return 0;
}

/**
 Performs the final print when the signal is received by the program.

 @param signal An integer values for the signal passed to the function.
 */
void SignalHandler(int signal)
{
	if(signal == SIGINT)
	{
		printf("\nTotal page faults: %d\n", pageFaults);
	}
	exit(0);
}