#include "dependencies/assembly.h"
#include <bits/types/sigevent_t.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
  assembly_line_t line;
  unsigned int position;
}timerargs ;


static void timer_handler(union sigval sv) {
  timerargs* id = sv.sival_ptr;

  // here call the trigger_arm
  error_t leftarm = trigger_arm(id->line,LEFT, id->position);
  error_t rightarm = trigger_arm(id->line,RIGHT, id->position);
  // todo en fct de la sortie error_t faire des trucs
  id->position++;
  printf("lefterror : %d \n", leftarm);
  printf("righterror: %d \n", rightarm);
  printf("position is %d \n", id->position);
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

  //Setup du timer
  timer_t timefunc;
  struct sigevent funev;
  struct itimerspec funspec;

  //Arguments initiaux du timer_handle
  timerargs args = {line, 0};

  //Config du timer
  funev.sigev_notify = SIGEV_THREAD;
  funev.sigev_notify_function = timer_handler;
  funev.sigev_notify_attributes = NULL;
  funev.sigev_value.sival_ptr = &args;
  
  funspec.it_value.tv_sec=0;
  funspec.it_value.tv_nsec=1;
  funspec.it_interval.tv_nsec=(BELT_PERIOD%1000)*1000000;
  funspec.it_interval.tv_sec=BELT_PERIOD/1000;

  timer_create(CLOCK_REALTIME, &funev, &timefunc);

  //Lancement du timer
  timer_settime(timefunc, 0, &funspec, NULL);

  run_assembly(line);
  free_assembly_line(&line);
  return EXIT_SUCCESS;
}

