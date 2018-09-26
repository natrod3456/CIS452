#include <pthread.h> 
#include <stdio.h> 
#include <errno.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>
#include <signal.h>

void* dispatchFunc (); /*Asks user for file name and then spawns a worker thread /w file name as param */

void* workerFunc (void* fileName);   /* Waits and then prints the name of the file passed, terminates */

void sigUsrHandler(int sigNum);

/* name of file specified by user */
char fileName[500];
int fileCount;

int main(){
	pthread_t dispatchThread; //might have to declare worker thread here idk
	int status; // int for tracking suc/fail of pthread operations
	signal(SIGINT, sigUsrHandler);
	/* create dispatch thread which calls dispatch func */
	if((status = pthread_create(&dispatchThread, NULL, dispatchFunc, NULL)) != 0){
		/* thread creation fail */
		fprintf(stderr, "thread creation error %d: %s\n", status, strerror(status));
		exit(1);
	}
	
	/* close dispatch thread - might have to improve so that it joins after ctrl-c */
	//graceful shutdown
	if((status = pthread_join(dispatchThread, NULL)) != 0){
		fprintf(stderr, "join error %d: %s\n", status, strerror(status));
		exit(1);
	}
printf("Total number of files requested: %d", fileCount);
return 0;
} 

void* dispatchFunc(){
	int status;
	while(1){
		printf("Enter File Name: ");
		scanf("%s", &fileName);
		fileCount++;
		pthread_t workerThread;
		if((status = pthread_create(&workerThread, NULL, workerFunc, &fileName)) != 0){
			/*thread creation fail */
			fprintf(stderr, "thread creation error %d: %s\n", status, strerror(status));
			exit(1);
		}
		pthread_detach(workerThread);
	}
}

void* workerFunc(void* filename){
	char *file = (char *) filename;
        int num = rand() %10 + 1;
	if(num > 8){
		int sleepTime = rand() % 10 + 7;
		sleep(sleepTime);
		printf("Could not find your file: %s\n", file);
	}
	else if (num < 9){
		sleep(1);
		printf("Found your file here is your file: %s\n", file);
	}
}

void sigUsrHandler(int sigNum){
	if(sigNum == SIGINT){
		printf("Total number of files requested: %d\n", fileCount);
		exit(0);
	}
}
