#include "dependencies/assembly.h"
#include <bits/types/sigevent_t.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef struct {
  side_t side;
  unsigned int position; 
}armId ;


static void timer_handler(union sigval sigev_value) {
  armId* id = sigev_value.sival_ptr;

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

  run_assembly(line);

  //Setup du timer
  timer_t timefunc;
  struct sigevent funev;
  struct itimerspec funspec;

  funev.sigev_notify = SIGEV_THREAD;

  //Fonction a trigger 
  funev.sigev_notify_function = timer_handler;

  //Todo gérer plusieurs attributs de la fonction trigger_arm
  funev.sigev_notify_attributes = NULL;

  funspec.it_value.tv_nsec=0;
  //Intervalle
  funspec.it_interval.tv_nsec=BELT_PERIOD*1000000;

  timer_create(CLOCK_REALTIME, &funev, &timefunc);
  timer_settime(timefunc, 0, &funspec, NULL);

  free_assembly_line(&line);
  return EXIT_SUCCESS;
}

