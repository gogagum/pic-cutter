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
               png_byte* color_type, png_byte *bit_depth,
               png_structp image_pointer, png_infop info_pointer)
{
  *width  = png_get_image_width(image_pointer, info_pointer);
  *height = png_get_image_height(image_pointer, info_pointer);
  *color_type = png_get_color_type(image_pointer, info_pointer);
  *bit_depth  = png_get_bit_depth(image_pointer, info_pointer);
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
png_bytepp
AllocateAndFillPNGMatrix(int width, int height,
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

void
ReadPNGIntoGrid(char* input_file_name, GGrid* gridp)
{
  // Open file
  FILE *input_fp = fopen(input_file_name, "rb");
  CheckIfPNG(input_fp);

  // TODO: check fp

  // Allocating by pointers to image and info structs
  png_structp image_ptr = PNGCreateReadStructWithCheck();
  png_infop info_ptr = PNGCreateInfoStructWithCheck(image_ptr);

  // Save stack context/enviroenment into png_jmpbuf(image_pointer)
  // as it is recommendet to do like this in manual.
  if(setjmp(png_jmpbuf(image_ptr)))
  {
    perror("setjmp on input\n");
    abort();
  }

  // Initialize the default input/output functions for the PNG file
  png_init_io(image_ptr, input_fp);
  png_read_info(image_ptr, info_ptr);

  // Image params
  png_byte color_type;
  png_byte bit_depth;

  // Get and check image parameters
  GetImageParams(&(gridp->width), &(gridp->height),
                 &color_type, &bit_depth,
                 image_ptr, info_ptr);

  CheckBitDepth(bit_depth);
  SetColorPalette(color_type, image_ptr); // ???

  // Allocating memory
  png_bytep *row_ptrs = AllocateAndFillPNGMatrix(gridp->width, gridp->height,
                                                 image_ptr, info_ptr);
  gridp->pixels_ptr = (GPixel**)row_ptrs;
  gridp->pixel_links_ptrs = AllocatePixelLinks(gridp->width,
                                               gridp->height);
  FillPixelLinksByDefault(gridp->pixel_links_ptrs, gridp->width, gridp->height);


  // Reading to allocated memory
  png_read_image(image_ptr, row_ptrs);

  // Close file
  fclose(input_fp);

  // Clean memory which is no longer needed
  png_destroy_read_struct(&image_ptr, &info_ptr, NULL);
}

void
WritePNGFromGrid(char* output_file_name, GGrid* gridp)
{
  // Open new file for changing write or creating it
  FILE *output_fp = fopen(output_file_name, "w+");
  if(!output_fp) {
    perror("Error while opening result file.\n");
    exit(1);
  }

  png_structp image_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!image_ptr)
  {
    abort();
  }

  png_infop info_ptr = png_create_info_struct(image_ptr);
  if (!info_ptr)
  {
    abort();
  }

  if (setjmp(png_jmpbuf(image_ptr)))
  {
    abort();
  }

  png_init_io(image_ptr, output_fp);

  png_set_IHDR(
    image_ptr,
    info_ptr,
    gridp->width, gridp->height,  // TODO to change to taking from grid
    8,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(image_ptr, info_ptr);

  png_write_image(image_ptr, (png_bytepp)(gridp->pixels_ptr));
  png_write_end(image_ptr, NULL);

  fclose(output_fp);

  png_destroy_write_struct(&image_ptr, &info_ptr);
}


