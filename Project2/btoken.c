#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

struct Token{
	int id;
	char message[5000];
};

struct Token token;
int  numMachines;
int destination;

void sigHandler(int);

int main(int argc, char **argv)
{	
	char input[5000];
	char buffer[5000];
	
	signal (SIGINT, sigHandler); 
	if(argc != 3){
		printf("Please enter the commands as followd: /\a.out [NUMMACHINES] [DESTINATION] \n");
		exit(1);
	
	}

	numMachines = strtol(argv[1], NULL, 10);
	destination = strtol(argv[2], NULL, 10);

	pid_t pids[numMachines];

	printf("Enter message you want to send: ");
	fgets(input, 5000,stdin);
	size_t length = strlen(input)-1;
	if(input[length] == '\n')
		input[length] = '\0';

	int fd[numMachines][2];
	for(int i = 0; i < numMachines; i++){
		if(pipe(fd[i]) < 0){
			perror("plumbing problem");
			exit(1);
		}
	}

	if( (pids[0] = fork()) < 0)
	{
		perror ("fork failed"); 
        	exit(1);
	}
	else if(pids[0] == 0){
	//child 1 process
		for(int x = 1; x < numMachines; x++){
			if((pids[x] = fork()) < 0){
				perror("fork failed");
				exit(1);
			}	
			else if(pids[x] == 0){
				read(fd[x-1][0], buffer, sizeof(buffer));
				printf("Child: %d  received message\n", x);
			//	printf("message is: %s \n", buffer);
				if(x == (numMachines - 1)){
					printf("Message is: %s\n", buffer);
					for(int y = 0; y < numMachines; y++){
						close(fd[y][0]);
						close(fd[y][1]);
					}
					printf("All pipes closed..goodbye!\n");
					exit(0);
				}
				write(fd[x][1], buffer, (size_t) strlen (buffer) + 1);
				printf("Child %d sending message\n", x);
			//	printf("message is: %s \n", buffer);
			}
			else{
				wait(NULL);
			}
		
		}

	}
	else{
	//parent 1 process
		write(fd[0][1], input, (size_t) strlen(input) + 1);
		printf("Parent1 sending message\n");
	//	printf("Message is: %s \n", input);
          	wait(NULL);
	}

}

void sigHandler(int sigNum)
{
	exit(0);
}
