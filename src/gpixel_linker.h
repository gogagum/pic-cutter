#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef GPIXEL_LINKER_H
#define GPIXEL_LINKER_H

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
FillPixelLinksByDefault(GPixelLinks **links_ptrs, int width, int height)
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
      links_ptrs[y][x] = default_pixel_links;
    }
  }
}

void
FreePixelLinks(int height, GPixelLinks **links_ptrs)
{
  for (int y = 0; y < height; ++y)
  {
    free(links_ptrs[y]);
  }
  free(links_ptrs);
}

#endif    // GPIXEL_LINKER_H
