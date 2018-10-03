/* This a reader program that accesses a shared memory segment and reads from it
 */

/*Header Files */
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAXSEGMENTSIZE 4096
#define MAXINPUTSIZE 200

int shmId;
char *shmPtr, *shmSeg, *writeFlag, *readFlag, *turnFlag;
char input[200];

void sigHandler(int);


/*Main Program*/
int main(){
 
	struct sigaction sa;         //create sig action struct
	sa.sa_handler = &sigHandler; //set handler
	sa.sa_flags = SA_RESTART;    //default?
	
	//init signal
	if (sigfillset(&sa.sa_mask) <0) {
		perror("Error with bitmask");
		exit(1);
	}
	
	//init signal handler
	if (sigaction(SIGINT, &sa, NULL) <0){
		perror("Error with handler init");
		exit(1);
	}

	key_t key;                 //key to shared mem
	int projectId = 50;         //not sure if abritrary or what
	//create IPC key
	if ((key = ftok("keyFile.txt",projectId )) == (key_t) -1) {
		perror("ftok key error");
		exit(1);
	}

	//create get mem segment
	if ((shmId = shmget(key, MAXSEGMENTSIZE, S_IRUSR|S_IWUSR)) <0){
		perror("Error shmget failure");
		exit(1);
	}

	//attach segment
	if ((shmPtr = shmat(shmId, 0, 0)) == (void*) -1){
		perror("Error attach failure");
		exit(1);
	}
	
	//set the segment to be the pointer created by attach
	shmSeg = shmPtr;

	writeFlag  = shmSeg;       //T or F, first addr of the seg
	readFlag   = shmSeg++;     //T or F, second addr of the seg
        turnFlag   = shmSeg++;     //R or W, third addr of the seg
	shmSeg++; 		   //move past flagsin the segment space

	char *output;
	while(1){

		//set read flag
		*readFlag = 'T';

		//set turn flag
		*turnFlag = 'W';

		//blocking guard
		while(*writeFlag == 'T' && *turnFlag == 'W');
		
		//critical section
		output = (char*) malloc((MAXINPUTSIZE+1)*sizeof(char));
		memcpy(output, shmSeg, MAXINPUTSIZE);
		printf("%s", output);
		free(output);
		*readFlag = 'F';
	}

	return 0;

}

void sigHandler (int sigNum){
	if (sigNum == SIGINT){
		if (shmdt(shmPtr) <0){
			perror("Error cant detatch");
			exit(1);
		} 
		printf("\nTermination complete\n");
		exit(0);
	}

}












