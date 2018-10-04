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
struct Token token2;
int  numMachines;
int destination;

/*************************************************************************************************
 * A program that uses communicating multiple processes to emulate a form of network communication.
 * B-Level.
 * @author: Natalie Rodriguez
 * @version: Fall 2018
 *************************************************************************************************/
int main(int argc, char **argv)
{	
	char input[5000];
	
	if(argc != 3){
		printf("Please enter the commands as followed: /\a.out [NUMMACHINES] [DESTINATION] \n");
		exit(1);
	
	}

	numMachines = strtol(argv[1], NULL, 10);
	destination = strtol(argv[2], NULL, 10);

	if(destination > numMachines){
		printf("Please enter valid destination\n");
		exit(0);
	}

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
	/* child processes */
		for(int x = 1; x < numMachines; x++){
			if((pids[x] = fork()) < 0){
				perror("fork failed");
				exit(1);
			}	
			else if(pids[x] == 0){
				read(fd[x-1][0], &token2, sizeof(struct Token));
				printf("Child: %d  received message\n", x);
				if(token2.id == x){
					printf("This is my message...printing message..\n");
					printf("Message: %s\n", token2.message);
					token2.id = 0;
					strcpy(token2.message, "");
					printf("Setting id to 0 and message to empty...\n");
					write(fd[x][1], &token2, sizeof(struct Token));
					printf("Child %d sending message\n", x);
				}
				else{
					printf("This is not my message\n");
					write(fd[x][1], &token2, sizeof(struct Token));
					printf("Child %d sending message\n", x);
				}

				if(x == (numMachines - 1)){
					printf("Finished message is: %s\n", buffer);
					for(int y = 0; y < numMachines; y++){
						close(fd[y][0]);
						close(fd[y][1]);
					}
					printf("All pipes closed..goodbye!\n");
					exit(0);
				}
			}
			else{
				wait(NULL);
			}
		
		}

	}
	else{
	/* parent 1 process */
		token.id = destination;
		strcpy(token.message, input);
		write(fd[0][1], &token, sizeof(struct Token));
		printf("Parent1 sending message\n");
          	wait(NULL);
	}

}
