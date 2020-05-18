#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Gpixel                                                                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef GPIXEL_H
#define GPIXEL_H

typedef struct gpixel_t{
  uint8_t R;
  uint8_t G;
  uint8_t B;
  uint8_t alpha;
} __attribute__ ((packed)) GPixel;

typedef struct gpixel_coords_t{
  int32_t x;
  int32_t y;
} GPixelCoords;

int32_t
ColorDistance(GPixel p1, GPixel p2)
{
  // Count difference for every dimension
  int32_t r_difference = p1.R - p2.R;
  int32_t g_difference = p1.G - p2.G;
  int32_t b_difference = p1.B - p2.B;

  int32_t distance = r_difference * r_difference +
                     g_difference * g_difference +
                     b_difference * b_difference;
  return distance;
}

#endif // GPIXEL_H
