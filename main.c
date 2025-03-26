#include "dependencies/assembly.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
  funev.sigev_notify_function = trigger_arm;

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

