#include "files/assembly.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{ 

  struct assembly_line line;
  init_assembly_line(line); 

  setup_arm(line, PART_FRAME, LEFT, 1);
  setup_arm(line, PART_ENGINE, LEFT, 2);
  setup_arm(line, PART_WHEELS, RIGHT, 2);
  setup_arm(line, PART_BODY, LEFT, 3);
  setup_arm(line, PART_DOORS, LEFT, 4);
  setup_arm(line, PART_LIGHTS, RIGHT,4);
  setup_arm(line, PART_WINDOWS, LEFT, 5);

  free_assembly_line(line);
  return EXIT_SUCCESS;
}
