#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

#include <png.h>

#include "gpixel.h"
#include "ggrid.h"
#include "gpixel_linker.h"
#include "png_io.h"
#include "dp.h"
#include "progress.h"

void
CheckWidthToCut(int width, int new_width)
{
  if (width < new_width && new_width >= 1) {
    printf("%s\n", "Uncorrect input.");
    exit(0);
  }
}

int
main(int argc, char* argv[])
{
  // Get args
  char* file_name = argv[1];
  char* new_file_name = argv[2];
  size_t new_width = atoi(argv[3]);

  GGrid working_grid;
  ReadPNGIntoGrid(file_name, &working_grid);
  CheckWidthToCut(working_grid.width, new_width);

  DPStruct** dp_matr =
      AllocateDPMatr(working_grid.width, working_grid.height);

  for (int i = 0; i < working_grid.width - new_width; ++i)
  {
    CountDP(dp_matr, &working_grid);
    ErasePixelsAccordingToDP(dp_matr, &working_grid);
    print_progress((size_t)i, (size_t)working_grid.width - new_width);
  }

  ApplyErasedPixelsFromGrid(new_width, &working_grid);

  WritePNGFromGrid(new_file_name, &working_grid);
  FreeDPMatr(working_grid.height, dp_matr);
  FreeGrid(&working_grid);

  return 0;
}
