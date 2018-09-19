#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

void sigHandler (int);
void sigHandlerUsr (int);
/*************************************************************************
 * A program that demonstrates the use of signals for simple communication.
 * @author Natalie Rodriguez, Jake Geers
 * @version Fall 2018
 *************************************************************************/
int main() 
{  
   signal (SIGUSR1, sigHandlerUsr);
   signal (SIGUSR2, sigHandlerUsr);
    
    pid_t pid, pid2;

    if ((pid = fork()) <0){
	    perror("fork failed");
	    exit(1);
    }
    else if (!pid){
	    //child 1 process
	    while(1){
		int num = rand() %5 + 1;
		int num2 = rand() %2 + 1;
		sleep(num);
		if(num2 == 1){
			raise(SIGUSR1);
		}
		else if (num2 == 2){
			raise(SIGUSR2);
		}
	    }
	    exit(0);
    }
    else{
	    printf("spawned child PID# %d\n", pid);
	    pid2 = fork();
	    if(!pid2){
	    //child 2 process
		while(1){
			int number = rand() %5 + 1;
			int number2 = rand() %2 + 1;
			sleep(number);
			if(number2 == 1){
				raise(SIGUSR1);
			}	
			else if (number2 == 2){
				raise(SIGUSR2);
			}
	    	}	
	    	exit(0);
	    }
	    else{
		//parent process
		printf("spawned child PID# %d\n", pid2);
		struct sigaction sa;
		struct sigaction sa2;
		sa.sa_handler = sigHandlerUsr;
		sa2.sa_handler = sigHandler;

	    	while(1){
			sigaction(SIGINT, &sa2, NULL);
			sigaction(SIGUSR1, &sa, NULL);
			sigaction(SIGUSR2, &sa, NULL);
	    	}
	    }
    
    }

    return 0; 
}

void sigHandler (int sigNum){ 
    printf (" received an interrupt.\n");  
    sleep (1); 
    printf ("outta here.\n"); 
    exit(0); 
}

void sigHandlerUsr (int usrInterrupt){
	pid_t pid;
	pid = getpid();
	if( usrInterrupt == SIGUSR1){
		printf("waiting... Received SIGUSR1 interrupt from pid# %d\n", pid);
	}
	else if (usrInterrupt == SIGUSR2){
		printf("waiting... Received SIGUSR2 interrupt from pid# %d\n", pid);
	}
}
