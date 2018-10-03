#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define READ 0
#define WRITE 1

struct Token{
	int id;
	char message[5000];
};

struct Token token;
int  numMachines;
int destination;
int numPipes;
char input[5000];

void sigHandler(int);

int main(int argc, char **argv)
{	
	signal (SIGINT, sigHandler); 
	if(argc != 3){
		printf("Please enter the commands as followd: /\a.out [NUMMACHINES] [DESTINATION] \n");
		exit(1);
	
	}

	numMachines = strtol(argv[1], NULL, 10);
	destination = strtol(argv[2], NULL, 10);

	printf("Enter message you want to send: ");
	fgets(input, 5000,stdin);
	size_t length = strlen(input)-1;
	if(input[length] == '\n')
		input[length] = '\0';

	if(numMachines != 2){
		 numPipes = numMachines;
	}
	else
		 numPipes = 1;

	int fd[2*numPipes];

	for(int i = 0; i < numPipes; i++){
		if(pipe(fd + i*2) < 0){
			perror("plumbing problem");
			exit(1);
		}
	}
	pid_t pids[numMachines];
	//create the processes now in a while loop
	//first process will copy down variables into struct then pass it down the first pipe
	for(int j = 0; j < numMachines; j++){
		if((pids[j] = fork()) <  0 ){
			perror ("fork failed"); 
        		exit(1); 
		}
		else if (pids[j] == 0){
			//child process - first process will copy data to struct and write to pipe
			//next processes will read from that pipe and write to next pipe
			exit(0);
		}
	}
	
	//is this right who knows
	int status;
	for(int x = 0; x < numMachines; x++){
		wait(&status);
	}



//	printf("token id is: %d\n", token.id);
//	printf("token message is: %s\n", token.message);
//	printf("num of machines is : %d\n", numMachines);
	
	return 0;
}

void sigHandler(int sigNum){
	//blahlbhablahblah
	printf("Exiting program...\n");
	exit(0);

}
