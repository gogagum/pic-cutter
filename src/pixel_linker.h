#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct gpixel_links_t{
  int32_t to_upper;  // upper.x = current.x + to_upper
  int32_t to_lower;  // lower.x = current.x + to_lower
  uint32_t to_left;
  uint32_t to_right;
  bool erased;
} GPixelLinks;

GPixelLinks
**AllocatePixelLinks(int width, int height)
{
  GPixelLinks **row_pointers =
      (GPixelLinks**)malloc(sizeof(GPixelLinks*) * height);

  for(uint32_t y = 0; y < height; y++)
  {
    row_pointers[y] = (GPixelLinks*)malloc(sizeof(GPixelLinks) * width);
  }
  return row_pointers;
}

void
FillPixelLinksByDefault(GPixelLinks **row_pointers, int width, int height)
{
  GPixelLinks default_pixel_links = {
    .to_upper = 0,
    .to_lower = 0,
    .to_left = 1,
    .to_right = 1,
    .erased = false
  };

  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      row_pointers[y][x] = default_pixel_links;
    }
  }
}
