#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

void sigHandler (int);
void sigHandlerUsr (int);

int main() 
{ 
    signal (SIGINT, sigHandler); 
    signal (SIGUSR1, sigHandlerUsr);
    signal (SIGUSR2, sigHandlerUsr);
    
    pid_t pid;
    if ((pid = fork()) <0){
	    perror("fork failed");
	    exit(1);
    }
    else if (!pid){
	    while(1){
		int num = rand() %5;
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
	    while(1){
		signal(SIGINT, sigHandler);
    		signal(SIGUSR1, sigHandlerUsr);
		signal(SIGUSR2, sigHandlerUsr);
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
	if( usrInterrupt == SIGUSR1){
	printf("Received SIGUSR1 interrupt\n");
	
	}
	else if (usrInterrupt == SIGUSR2){
	printf("received SIGUSR2 interrupt\n");
	
	}

}
