#include "dependencies/assembly.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

typedef struct {
  side_t side;
  part_t part;
  unsigned int position; 
}armId ;


// setting the assembly
assembly_line_t line;

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
  printf("setup du bras : pos :%u side:%u part: %u\n", id->position, id->side,id->part);
  setup_arm(line, id->part, id->side, id->position);
  
  while(1){
    // sleep le BELT_Period * id.position 
    // essayer de arm_trigger avec id comme param
    // sleep jusqu'à ce que la line se restart (jusque position finale donc posfinale - pos)
  }
}


int main(int argc, char *argv[]){ 
  // initialazing the assembly_line
  init_assembly_line(&line); 

  armId depList[NUM_PARTS] =  {
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

  for (int i = 0; i < NUM_PARTS; i++) {
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

