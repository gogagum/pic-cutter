#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Gpixel                                                                     //
////////////////////////////////////////////////////////////////////////////////

typedef struct gpixel_t{
  uint8_t R,
  uint8_t G,
  uint8_t B,
  uint8_t alpha
} GPixel;

typedef struct gpixel_coords_t{
  int32_t x,
  int32_t y
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

////////////////////////////////////////////////////////////////////////////////
// Grid                                                                       //
////////////////////////////////////////////////////////////////////////////////

typedef struct grid_t{
  int32_t width,
  int32_t height,
  png_byte **row_ptrs,
  GPixelLinks **pixel_links_ptrs
} GGrid;

// Sets coords for pixel left neighbour
// If there is no neighbour, sets current coords
void
GetLeftToCompare(GPixelCoords *left_to_cmp_coords_ptr,
                 GPixelCoords curr, GGrid *gridp)
{
  (*left_to_cmp_coords_ptr).y = curr.y;
  (*left_to_cmp_coords_ptr).x =
      curr.x - (*gridp).pixel_links_ptrs[y][x].to_left;
  if ((*left_to_cmp_coords_ptr).x < 0)
    (*left_to_cmp_coords_ptr).x = 0;
}

// Sets coords for pixel right neighbour
// If there is no neighbour, sets current coords
void
GetRightToCompare(GPixelCoords *right_to_cmp_coords_ptr,
                  GPixelCoords curr, GGrid *gridp)
{
  (*right_to_cmp_coords_ptr).y = curr.y;
  (*right_to_cmp_coords_ptr).x =
      curr.x + (*gridp).pixel_links_ptrs[y][x].to_right;
  if ((*right_to_cmp_coords_ptr).x >= (*gridp).width)
    (*right_to_cmp_coords_ptr).x = (*gridp).width - 1;
}

// Sets coords for pixel upper neighbour
// If there is no neighbour, sets current coords
void
GetUpperToCompare(GPixelCoords *upper_to_cmp_coords_ptr,
                  GPixelCoords curr, GGrid *gridp)
{
  (*upper_to_cmp_coords_ptr).x =
      curr.x + (*gridp).pixel_links_ptrs[y][x].to_upper;
  (*upper_to_cmp_coords_ptr).y = curr.y + 1;
  if ((*upper_to_cmp_coords_ptr) == (*gridp).height)
    (*upper_to_cmp_coords_ptr).y--;
}

// Sets coords for pixel lower neighbour
// If there is no neighbour, sets current coords
void
GetLowerToCompare(GPixelCoords *lower_to_cmp_coords_ptr,
                  GPixelCoords curr, GGrid *gridp)
{
  (*lower_to_cmp_coords_ptr).x =
      curr.x + (*gridp).pixel_links_ptrs[y][x].to_lower;
  (*lower_to_cmp_coords_ptr).y = curr.y - 1;
  if ((*lower_to_cmp_coords_ptr).y == -1)
    (*lower_to_cmp_coords_ptr).y++;
}

int32_t
GetWeight(GPixelCoords curr, GGrid *gridp)
{
  GPixelCoords lower_to_cmp_coords;
  GPixelCoords upper_to_cmp_coords;
  GPixelCoords left_to_cmp_coords;
  GPixelCoords right_to_cmp_coords;
  GetLowerToCompare(&lower_to_cmp_coords, curr, gridp);
  GetUpperToCompare(&upper_to_cmp_coords, curr, gridp);
  GetLeftToCompare(&left_to_cmp_coords, curr, gridp);
  GetRightToCompare(&right_to_cmp_coords, curr, gridp);
  GPixel curr_pixel = (*gridp).row_ptrs[y][x];  // To check
  GPixel lower_pixel = (*gridp).row_ptrs[lower_to_cmp_coords.y][lower_to_cmp_coords.x];
  GPixel upper_pixel = (*gridp).row_ptrs[upper_to_cmp_coords.y][upper_to_cmp_coords.x];
  GPixel left_pixel  = (*gridp).row_ptrs[left_to_cmp_coords.y][left_to_cmp_coords.x];
  GPixel right_pixel = (*gridp).row_ptrs[right_to_cmp_coords.y][right_to_cmp_coords.x];
  int32_t weight = ColorDistance(curr_pixel, lower_pixel) +
                   ColorDistance(curr_pixel, upper_pixel) +
                   ColorDistance(curr_pixel, left_pixel) +
                   ColorDistance(curr_pixel, right_pixel);
  return weight;
}
