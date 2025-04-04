#include "dependencies/assembly.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  side_t side;
  part_t part;
  unsigned int position; 
}armId ;


// setting the assembly
assembly_line_t line;

void printError(error_t er){
    switch (er) {
    case OK:
      printf("ok\n");
      break;
    case INSTALL_REQUIREMENTS:
      printf("INSTALL_REQUIREMENTS\n");
      break;
    case LINE_STARTED:
      printf("LINE_STARTED\n");
      break;
    case INCORRECT_POSITION:
      printf("INCORRECT_POSITION\n");
      break;
    case INCORRECT_BELT_POSITION:
      printf("INCORRECT_BELT_POSITION\n");
      break;
    case NON_EMPTY_POSITION:
      printf("NON_EMPTY_POSITION\n");
      break;
    case TIME_ERROR:
      printf("TIME_ERROR\n");
      break;
    case SEM_ERROR:
      printf("SEM_ERROR\n");
      break;
    case MALLOC_ERROR:
      printf("MALLOC_ERROR\n");
      break;
    case LINE_STOPPED:
      printf("LINE_STOPPED\n");
      break;
    case INVALID_POINTER:
      printf("INVALID_POINTER\n");
      break;
    }
}

void signal_handler(int sigNum){
  if (sigNum == SIGINT){
    printf("Freeing the assembly_line\n");
    print_assembly_stats(line);
    free_assembly_line(&line);
    exit(EXIT_SUCCESS);
  }
  if (sigNum == SIGUSR1){
    printf("The assembly stats are :\n");
    print_assembly_stats(line);
  }
}

void* arm_handler(void *armid){
  armId *id = armid;
  long triggerTime = BELT_PERIOD * id->position;
  long waitForRestart = BELT_PERIOD * (MAX_POSITION-2 - id->position);
  // set the overshot inititaly to 0
  // it's the difference between `abs time to wake up ` and `real time when we
  // woke up`
  struct timespec overshot;
  overshot.tv_sec = 0;
  overshot.tv_nsec = 0;
  struct timespec now;
  struct timespec sleepTo;

  // first sleep a bit to not wake up to early
  clock_gettime(CLOCK_REALTIME, &sleepTo);
  sleepTo.tv_nsec += 9000000;
  clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepTo, NULL);


  unsigned long wakeupTimer_sec[] =
  { triggerTime/1000+sleepTo.tv_sec,
    (waitForRestart + triggerTime*2)/1000 +sleepTo.tv_sec,
    (waitForRestart*2 + triggerTime*3)/1000+sleepTo.tv_sec,
    (waitForRestart*3 + triggerTime*4)/1000+sleepTo.tv_sec,
    (waitForRestart*4 + triggerTime*5)/1000+sleepTo.tv_sec,
  };

  unsigned long wakeupTimer_nsec[] =
  { triggerTime%1000+sleepTo.tv_nsec,
    (waitForRestart + triggerTime*2)%1000 +sleepTo.tv_nsec,
    (waitForRestart*2 + triggerTime*3)%1000+sleepTo.tv_nsec,
    (waitForRestart*3 + triggerTime*4)%1000+sleepTo.tv_nsec,
    (waitForRestart*4 + triggerTime*5)%1000+sleepTo.tv_nsec,
  };

  int count = 0;

  while(1){

    // tell the arm to sleep until triggerTime
    clock_gettime(CLOCK_REALTIME, &now);
    sleepTo.tv_sec = now.tv_sec + triggerTime / 1000 ;
    sleepTo.tv_nsec = now.tv_nsec + triggerTime % 1000 ;
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepTo, NULL);

    // calcule à quel point l'voershot est grand
    clock_gettime(CLOCK_REALTIME, &now);
    overshot.tv_nsec = (now.tv_nsec - sleepTo.tv_nsec );
    overshot.tv_sec = (now.tv_sec - sleepTo.tv_sec );

    // printf("devait dormir jusque : %lu sec et %lu\n", wakeupTimer_sec[count], wakeupTimer_nsec[count]);
    // printf("voulait dormir jusque : %lu sec et %lu\n", sleepTo.tv_sec, sleepTo.tv_nsec);
    // printf("à dormit jusque : %lu sec et %lu\n", now.tv_sec, now.tv_nsec);
    // printf("overshot is calc at : %lu\n", overshot.tv_nsec);
    // printf("alors que ça devrait être : %lu \n", -wakeupTimer_nsec[count] + now.tv_nsec);

    sleepTo.tv_sec = now.tv_sec + waitForRestart / 1000 ;
    sleepTo.tv_nsec = now.tv_nsec + waitForRestart % 1000 ;
    // printf("suis le bras à la pos : %u side : %u je me trigger\n", id->position, id->side);
    error_t er = trigger_arm(line, id->side, id->position);
    printError(er);

    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepTo, NULL);

    clock_gettime(CLOCK_REALTIME, &now);
    overshot.tv_nsec = (now.tv_nsec - sleepTo.tv_nsec);
    overshot.tv_sec = (now.tv_sec - sleepTo.tv_sec);

    count++;

  }
}


int main(int argc, char *argv[]){ 
  // initialazing the assembly_line
  init_assembly_line(&line); 

  armId depList[NUM_PARTS- 1] =  {
    {LEFT, PART_FRAME, 1}, 
    {LEFT, PART_ENGINE, 2},
    {RIGHT, PART_WHEELS, 2},
    {LEFT, PART_BODY, 3 },
    {LEFT, PART_DOORS, 4},
    {RIGHT,PART_LIGHTS,  4},
    {LEFT, PART_WINDOWS, 5},
  }; 

  //may be put in global
  pthread_t* arm_threads = malloc(sizeof(pthread_t) * NUM_PARTS);

  for (int i = 0; i < NUM_PARTS - 1; i++) {
    // setting the arm handler thread
    setup_arm(line, depList[i].part, depList[i].side, depList[i].position);
    pthread_create(&arm_threads[i], NULL, arm_handler, &depList[i]);
  }

  // setting up the signals handling
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);

  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGUSR1, &sa, NULL);
  
  run_assembly(line);
}


