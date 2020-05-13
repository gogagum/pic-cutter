#include <stdlib.h>
#include <stdint.h>

struct PixelLinks{
  uint32_t to_upper;
  uint32_t to_lower;
  uint32_t to_left;
  uint32_t to_right;
};

struct PixelLinks
**AllocatePixelLinks(int width, int height)
{
  struct PixelLinks **row_pointers =
    (struct PixelLinks**)malloc(sizeof(struct PixelLinks*) * height);

  for(uint32_t y = 0; y < height; y++)
  {
    row_pointers[y] =
      (struct PixelLinks*)malloc(sizeof(struct PixelLinks) * width);
  }
  return row_pointers;
}

void
FillPixelLinksByDefault(struct PixelLinks **row_pointers, int width, int height)
{
  struct PixelLinks default_pixel_links = {
    .to_upper = 0,
    .to_lower = 0,
    .to_left = 1,
    .to_right = 1
  };

  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      row_pointers[y][x] = default_pixel_links;
    }
  }
}
