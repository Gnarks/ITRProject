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
  setup_arm(line, id->part, id->side, id->position);
  long triggerTime = BELT_PERIOD * id->position;
  long waitForRestart = BELT_PERIOD * (7 - id->position);

  struct timespec time;
  // first sleep a bit to not wake up to early
  clock_gettime(CLOCK_REALTIME, &time);
  time.tv_nsec += 8000000;
  clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &time, NULL);

  while(1){
    // tell the arm to sleep until triggerTime
    clock_gettime(CLOCK_REALTIME, &time);
    time.tv_sec += triggerTime / 1000;
    time.tv_nsec += triggerTime % 1000;
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &time, NULL);

    printf("suis le bras à la pos : %u side : %u je me trigger\n", id->position, id->side);
    error_t er = trigger_arm(line, id->side, id->position);

    printError(er);

    clock_gettime(CLOCK_REALTIME, &time);
    time.tv_sec += waitForRestart / 1000;
    time.tv_nsec += waitForRestart % 1000;
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &time, NULL);

    // sleep le BELT_Period * id.position 
    // essayer de arm_trigger avec id comme param
    // sleep jusqu'à ce que la line se restart (jusque position finale donc posfinale - pos)
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


