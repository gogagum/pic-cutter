#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

//#define PNG_DEBUG 3
#include <png.h>

#include "pixel_linker.h"
#include "gpixel.h"

// Checks if file is png
void
CheckIfPNG(FILE* fp)
{
  uint8_t first_byte;
  size_t num_of_bytes = 1;
  fread(&first_byte, 1, num_of_bytes, fp);

  if (png_sig_cmp(&first_byte, 0, num_of_bytes))
  {
    perror("Not png error");
    exit(1);
  }
  // Go back to the beginning of file
  rewind(fp);
}

void
GetImageParams(int *width, int *height,
               png_byte* color_type, png_byte *bit_depth
               png_structp image_pointer, png_infop info_pointer)
{
  *width  = png_get_image_width(image_pointer, info_pointer);
  *height = png_get_image_height(image_pointer, info_pointer);
  *color_type = png_get_color_type(image_pointer, info_pointer);
  *bit_depth  = png_get_bit_depth(image_pointer, info_pointer);
}

void
CheckWidthToCut(int width, int width_to_cut)
{
  if (width < width_to_cut) {
    printf("%s\n", "Uncorrect input.");
    // TODO: exit more accurate, or make shure it is ok.
    exit(0);
  }
}

void
CheckBitDepth(png_byte bit_depth)
{
  // For now support only 8 bits depth
  if (8 != bit_depth)
  {
    printf("%s\n", "This bit depth is not supported yet.");
    exit(0);
  }

  /*
   possibly this is the best way:
   void png_set_strip_16(png_structp png_ptr);
   */
}

void
SetColorPalette (png_byte color_type, png_structp image_pointer)
{
  if(color_type == PNG_COLOR_TYPE_PALETTE)
  {
    png_set_palette_to_rgb(image_pointer);
  }
}

// Allocating and default filling png_struct
png_structp
PNGCreateReadStructWithCheck()
{
  png_structp image_pointer = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                     NULL,
                                                     NULL,
                                                     NULL);
  if(NULL == image_pointer)
  {
    perror("Error while png_create_read_struct(...)");
    exit(1);
  }
  return image_pointer;
}

// Allocating and default filling png_info
png_infop
PNGCreateInfoStructWithCheck(png_structp image_pointer)
{
  png_infop info_pointer = png_create_info_struct(image_pointer);
  if(NULL == info_pointer)
  {
    perror("Error while png_create_info_struct(...)");
    exit(1);
  }
  return info_pointer;
}

// Allocating matrix
png_bytep
*AllocateAndFillPNGMatrix(int width, int height,
                          png_structp image_pointer,
                          png_infop info_pointer)
{
  if (width * height > 10000000)
  {
    printf("Pic size is too big.");
    exit(0);
  }
  png_bytep *row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
  for(uint32_t y = 0; y < height; y++)
  {
    row_pointers[y] =
        (png_byte*)malloc(png_get_rowbytes(image_pointer, info_pointer));
  }
  return row_pointers;
}

int
main(int argc, char* argv[])
{
  // Get args
  char* file_name = argv[1];
  size_t width_to_cut = atoi(argv[2]);

  // Open file
  FILE *fp = fopen(file_name, "rb");
  CheckIfPNG(fp);

  // Allocating by pointers to image and info structs
  png_structp image_pointer = PNGCreateReadStructWithCheck();
  png_infop info_pointer = PNGCreateInfoStructWithCheck(image_pointer);

  // Save stack context/enviroenment into png_jmpbuf(image_pointer)
  // as it is recommendet to do like this in manual.
  if(setjmp(png_jmpbuf(image_pointer)))
  {
    perror("setjmp");
    abort();
  }

  // Initialize the default input/output functions for the PNG file
  png_init_io(image_pointer, fp);
  png_read_info(image_pointer, info_pointer);

  // Image params
  int width, height;
  png_byte color_type;
  png_byte bit_depth;

  // Get and check image parameters
  GetImageParams(width, height, color_type, bit_depth,
                 image_pointer, info_pointer);
  CheckWidthToCut(width, width_to_cut);
  CheckBitDepth(bit_depth);
  SetColorPalette(color_type, image_pointer);

  // Allocating memory
  png_bytep *row_pointers =
      AllocateAndFillPNGMatrix(width, height, image_pointer, info_pointer);

  // Reading to allocated memory
  png_read_image(image_pointer, row_pointers);




  return 0;
}
