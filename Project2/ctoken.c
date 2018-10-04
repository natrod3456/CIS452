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

int main(int argc, char **argv)
{	
	char input[5000];
	char buffer[5000];
	char final[5000];
	pid_t pid, cid, gid;
	//signal (SIGINT, sigHandler); 
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

	int fd[2][2];
	for(int i = 0; i < 2; i++){
		if(pipe(fd[i]) < 0){
			perror("plumbing problem");
			exit(1);
		}
	}

	if( (pid = fork()) < 0)
	{
		perror ("fork failed"); 
        	exit(1);
	}
	else if(pid == 0){
	//child 1 process
		if((cid = fork()) < 0){
			perror("fork failed");
			exit(1);
		}	
		else if(cid == 0){
			read(fd[0][0], buffer, sizeof(buffer));
			printf("Child received message\n");
			printf("message is: %s \n", buffer);
			close(fd[0][1]);
			write(fd[1][1], buffer, (size_t) strlen (buffer) + 1);
			printf("Child sending message\n");
			printf("message is: %s \n", buffer);
			close(fd[0][0]);
			
			if((gid = fork()) < 0){
				perror("fork failed");
				exit(1);
			}
			else if (gid == 0){
				//grandchild process
				read(fd[1][0], final, sizeof(buffer));
				printf("Grandchild received message\n");
				puts(final);
				close(fd[1][1]);
				close(fd[1][0]);
				exit(0);
			}
			else{
				wait(NULL);
			}
			
		}
		else{
			wait(NULL);		
		
		}

	}
	else{
	//parent 1 process
		write(fd[0][1], input, (size_t) strlen(input) + 1);
		printf("Parent sending message\n");
		printf("Message is: %s \n", input);
          	wait(NULL);
	}
}
