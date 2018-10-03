/* This a writer program that creates a shared memory segment and writes
 * to it based on user input
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
	if ((shmId = shmget(key, MAXSEGMENTSIZE, IPC_CREAT|S_IRUSR|
					S_IWUSR)) <0){
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
	shmSeg++;	           //move past flagsin the segment space
	
	char *input;
	while(1){

		*writeFlag = 'T';
		*turnFlag = 'R';
		//get user input
		input = (char*) malloc((MAXINPUTSIZE+1)*sizeof(char));
		fgets(input, MAXINPUTSIZE, stdin);
		printf("input was: %s",input); 

		//set writer flag
		//*writeFlag = 'T';

		//set turn flag
	//	*turnFlag = 'R';

		//blocking guard
		while(*readFlag == 'T' && *turnFlag == 'R');
		
		//critical section
		//sprintf(shmSeg, "%s", input);
		memcpy(shmSeg, input, MAXINPUTSIZE);
		free(input);
		*writeFlag = 'F';
	}

	return 0;

}

void sigHandler (int sigNum){
	if (sigNum == SIGINT){
		if (shmdt(shmPtr) <0){
			perror("Error cant detatch");
			exit(1);
		} 
		if (shmctl(shmId, IPC_RMID, 0) < 0){
			perror("Error cant deallocate");
			exit(1);
		}
		printf("\nTermination complete\n");
		exit(0);
	}

}












