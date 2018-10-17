#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

#define SIZE 16

void sem_lock(int sem_id);
void sem_unlock(int sem_id);

int main (int argc, char **argv)
{
   int status;
   long int i, loop, temp, *shmPtr;
   int shmId;
   pid_t pid;
   int semId;

      // get value of loop variable (from command-line argument)
   if (argc != 2){
	   printf("you forgot loop variable");
	   exit(1);
   }
   loop = strtol(argv[1], NULL, 10);

   if ((shmId = shmget (IPC_PRIVATE, SIZE, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) {
      perror ("i can't get no..\n");
      exit (1);
   }
   if ((shmPtr = shmat (shmId, 0, 0)) == (void*) -1) {
      perror ("can't attach\n");
      exit (1);
   }
   
   // sem create
   semId = semget(IPC_PRIVATE, 1, 00600);
   
   //sem init
   semctl(semId, 0, SETVAL, 1); 

   shmPtr[0] = 0;
   shmPtr[1] = 1;

 

   if (!(pid = fork())) {
      for (i=0; i<loop; i++) {
               // swap the contents of shmPtr[0] and shmPtr[1]
	       sem_lock(semId);
	       temp = shmPtr[0];
       	       shmPtr[0] = shmPtr[1];
	       shmPtr[1] = temp;	       
	       sem_unlock(semId);
      }
      if (shmdt (shmPtr) < 0) {
         perror ("just can't let go\n");
         exit (1);
      }
      exit(0);
   }
   else
      for (i=0; i<loop; i++) {
               // swap the contents of shmPtr[1] and shmPtr[0]
	       sem_lock(semId);
	       temp = shmPtr[1];
	       shmPtr[1] = shmPtr[0];
	       shmPtr[0] = temp;
	       sem_unlock(semId);
      }

   wait (&status);
   printf ("values: %li\t%li\n", shmPtr[0], shmPtr[1]);

   if (shmdt (shmPtr) < 0) {
      perror ("just can't let go\n");
      exit (1);
   }
   if (shmctl (shmId, IPC_RMID, 0) < 0) {
      perror ("can't deallocate\n");
      exit(1);
   }

   // remove semaphore
   semctl (semId, 0, IPC_RMID);

   return 0;
}


/* locks semaphore providing exlusive access to a resource */
void sem_lock (int sem_id) {
	struct sembuf sem_op;

	/*wait -> decrement val */
	sem_op.sem_num  = 0;
	sem_op.sem_op   = -1;
	sem_op.sem_flg = 0;

	semop(sem_id, &sem_op, 1);
}

/* unlocks semaphore */
void sem_unlock (int sem_id) {
	struct sembuf sem_op;

	/* signal -> increase val */
	sem_op.sem_num = 0;
	sem_op.sem_op = 1;
	sem_op.sem_flg = 0;

	semop(sem_id, &sem_op, 1);
}



