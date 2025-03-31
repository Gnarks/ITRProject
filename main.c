#include "dependencies/assembly.h"
#include <bits/types/sigevent_t.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef struct {
  side_t side;
  unsigned int position; 
}armId ;


// the thread used to call the right arm to be triggered
// to join if sigint is received !
static void* arm_handler() {

  // here call the trigger_arm 
  // todo en fct de la sortie error_t faire des trucs

}

int main(int argc, char *argv[])
{ 
  assembly_line_t line;
  init_assembly_line(&line);

  setup_arm(line, PART_FRAME, LEFT, 1);
  setup_arm(line, PART_ENGINE, LEFT, 2);
  setup_arm(line, PART_WHEELS, RIGHT, 2);
  setup_arm(line, PART_BODY, LEFT, 3);
  setup_arm(line, PART_DOORS, LEFT, 4);
  setup_arm(line, PART_LIGHTS, RIGHT,4);
  setup_arm(line, PART_WINDOWS, LEFT, 5);


  pthread_t arm_handler_thread;
  pthread_create(&arm_handler_thread, NULL, &arm_handler, NULL);

  run_assembly(line);
  free_assembly_line(&line);


  return EXIT_SUCCESS;
}

