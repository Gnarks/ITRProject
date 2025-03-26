#include "dependencies/assembly.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>


// setting the assembly
assembly_line_t line;

void signal_handler(int sigNum){
  if (sigNum == SIGINT){
    printf("Freeing the assembly_line\n");
    free_assembly_line(&line);
    exit(0);
  }
  if (sigNum == SIGUSR1){
    printf("Here print assembly stats\n");
    print_assembly_stats(line);
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
  

  run_assembly(line);

  free_assembly_line(&line);
  return EXIT_SUCCESS;
}
