#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

//#define PNG_DEBUG 3
#include <png.h>

#include "gpixel.h"
#include "ggrid.h"
#include "gpixel_linker.h"
#include "rename.h"
#include "png_io.h"
#include "dp.h"

void
CheckWidthToCut(int width, int width_to_cut)
{
  if (width < width_to_cut) {
    printf("%s\n", "Uncorrect input.");
    // TODO: exit more accurate, or make shure it is ok.
    exit(0);
  }
}

int
main(int argc, char* argv[])
{
  // Get args
  char* file_name = argv[1];
  size_t width_to_cut = atoi(argv[2]);

  GGrid working_grid;
  ReadPNGIntoGrid(file_name, &working_grid);
  CheckWidthToCut(working_grid.width, width_to_cut);

  DPStruct** dp_matr =
      AllocateDPMatr(working_grid.width, working_grid.height);

  for (int i = 0; i < working_grid.width - width_to_cut; ++i)
  {
    CountDP(working_grid.width, working_grid.height, dp_matr, &working_grid);
    ErasePixelsAccordingToDP(dp_matr, &working_grid);
  }

  ApplyErasedPixelsFromGrid(width_to_cut, &working_grid);

  char new_file_name[sizeof(char) * (strlen(file_name) + 10)];
  Rename(new_file_name, file_name);
  WritePNGFromGrid(new_file_name, &working_grid);
  ClearDp(&working_grid, );
  ClearGrid(&working_grid);


  return 0;
}
