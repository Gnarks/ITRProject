#include "files/assembly.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{ 
  struct assembly_line *line;
  init_assembly_line(line); 
  

  free_assembly_line(line);
  return EXIT_SUCCESS;
}
