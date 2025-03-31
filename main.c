#include "dependencies/assembly.h"
#include <bits/types/sigevent_t.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

typedef struct {
  side_t side;
  unsigned int position; 
}armId ;


// setting the assembly
assembly_line_t line;
// setting the arm handler thread
pthread_t arm_handler_thread;

void signal_handler(int sigNum){
  if (sigNum == SIGINT){
    printf("Freeing the assembly_line\n");
    pthread_join(arm_handler_thread, NULL);
    free_assembly_line(&line);
    exit(EXIT_SUCCESS);
  }
  if (sigNum == SIGUSR1){
    printf("The assembly stats are :\n");
    print_assembly_stats(line);
  }
}

// the thread used to call the right arm to be triggered
// to join if sigint is received !
static void* arm_handler() {

  int car_position = 0;
  while (1) {
    // call le bras à gauche et droite à car_position dans des nouveaux thread 
    // gérer les erreurs retournée
    // sleep(BELT_Period)
  }
}

int main(int argc, char *argv[])
{ 
  // initialazing the assembly_line
  init_assembly_line(&line); 

  // setting up the signals handling
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);

  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGUSR1, &sa, NULL);

  setup_arm(line, PART_FRAME, LEFT, 1);
  setup_arm(line, PART_ENGINE, LEFT, 2);
  setup_arm(line, PART_WHEELS, RIGHT, 2);
  setup_arm(line, PART_BODY, LEFT, 3);
  setup_arm(line, PART_DOORS, LEFT, 4);
  setup_arm(line, PART_LIGHTS, RIGHT,4);
  setup_arm(line, PART_WINDOWS, LEFT, 5);

  // starting the arm handler thread
  pthread_create(&arm_handler_thread, NULL, &arm_handler, NULL);

  
  run_assembly(line);
}

