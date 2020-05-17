#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Gpixel                                                                     //
////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////
// Grid                                                                       //
////////////////////////////////////////////////////////////////////////////////

typedef struct grid_t{
  int32_t width;
  int32_t height;
  GPixel **pixels_ptr;
  GPixelLinks **pixel_links_ptrs;
} GGrid;

// Returns right neighbour coords if exists.
// In other case returns (gridp->width, curr.y).
GPixelCoords
GetRight(GPixelCoords curr, GGrid *gridp)
{
  GPixelCoords right_coords = {
    .x = curr.x + gridp->pixel_links_ptrs[curr.y][curr.x].to_right,
    .y = curr.y
  };
  return right_coords;
}

// Returns left neighbour coords if exists.
// In other case returns (-1, curr.y).
GPixelCoords
GetLeft(GPixelCoords curr, GGrid *gridp)
{
  GPixelCoords left_coords = {
    .x = curr.x - gridp->pixel_links_ptrs[curr.y][curr.x].to_left,
    .y = curr.y
  };
  return left_coords;
}

// Returns lower neighbour coords if exist.
// In other case returns (x1, 0) with random x1
GPixelCoords
GetLower(GPixelCoords curr, GGrid *gridp)
{
  GPixelCoords left_coords = {
    .x = curr.x + gridp->pixel_links_ptrs[curr.y][curr.x].to_lower,
    .y = curr.y - 1
  };
  return left_coords;
}

// Returns coords for pixel left neighbour
// If there is no neighbour, returns current coords
GPixelCoords
GetLeftToCompare(GPixelCoords curr, GGrid *gridp)
{
  GPixelCoords left_to_cmp_coords = {
    .x = curr.x - gridp->pixel_links_ptrs[curr.y][curr.x].to_left,
    .y = curr.y
  };
  if (left_to_cmp_coords.x < 0) { left_to_cmp_coords.x = 0; }
  return left_to_cmp_coords;
}

// Sets coords for pixel right neighbour
// If there is no neighbour, sets current coords
GPixelCoords
GetRightToCompare(GPixelCoords curr, GGrid *gridp)
{
  GPixelCoords right_to_cmp_coords = {
    .x = curr.x + gridp->pixel_links_ptrs[curr.y][curr.x].to_right,
    .y = curr.y
  };
  if (right_to_cmp_coords.x >= gridp->width) {
    right_to_cmp_coords.x = gridp->width - 1;
  }
  return right_to_cmp_coords;
}

// Sets coords for pixel upper neighbour
// If there is no neighbour, sets current coords
GPixelCoords
GetUpperToCompare(GPixelCoords curr, GGrid *gridp)
{
  GPixelCoords upper_to_cmp_coords = {
    .x = gridp->pixel_links_ptrs[curr.y][curr.x].to_upper,
    .y = curr.y + 1
  };
  if (upper_to_cmp_coords.y == gridp->height)
  {
    upper_to_cmp_coords.y = gridp->height - 1;
  }
  return upper_to_cmp_coords;
}

// Sets coords for pixel lower neighbour
// If there is no neighbour, sets current coords
GPixelCoords
GetLowerToCompare(GPixelCoords curr, GGrid *gridp)
{
  GPixelCoords lower_to_cmp_coords = {
    .x = curr.x + gridp->pixel_links_ptrs[curr.y][curr.x].to_lower,
    .y = curr.y - 1
  };
  if (lower_to_cmp_coords.y == -1) { lower_to_cmp_coords.y=0; }
  return lower_to_cmp_coords;
}

int32_t
GetWeight(GPixelCoords curr, GGrid *gridp)
{
  GPixelCoords lower_to_cmp_coords = GetLowerToCompare(curr, gridp);
  GPixelCoords upper_to_cmp_coords = GetUpperToCompare(curr, gridp);;
  GPixelCoords left_to_cmp_coords = GetLeftToCompare(curr, gridp);
  GPixelCoords right_to_cmp_coords = GetRightToCompare(curr, gridp);
  GPixel curr_pixel =  gridp->pixels_ptr[curr.y][curr.x];  // To check
  GPixel lower_pixel = gridp->pixels_ptr[lower_to_cmp_coords.y][lower_to_cmp_coords.x];
  GPixel upper_pixel = gridp->pixels_ptr[upper_to_cmp_coords.y][upper_to_cmp_coords.x];
  GPixel left_pixel  = gridp->pixels_ptr[left_to_cmp_coords.y][left_to_cmp_coords.x];
  GPixel right_pixel = gridp->pixels_ptr[right_to_cmp_coords.y][right_to_cmp_coords.x];
  int32_t weight = ColorDistance(curr_pixel, lower_pixel) +
                   ColorDistance(curr_pixel, upper_pixel) +
                   ColorDistance(curr_pixel, left_pixel) +
                   ColorDistance(curr_pixel, right_pixel);
  return weight;
}

void
ApplyErasedPixelsFromGrid(int width_to_cut, GGrid *gridp)
{
  for (int y = 0; y < (*gridp).height; ++y)
  {
    // Create new line
    GPixel *new_line = malloc(sizeof(GPixel) * gridp->width - width_to_cut);
    int x =  0; // new_line iterator
    GPixelCoords curr = {
      .x = 0,
      .y = y
    };
    // Fill line moving through list

    // Skip all erased in the beginning of line
    while(curr.x < gridp->width)
    {
      while(gridp->pixel_links_ptrs[curr.y][curr.x].erased)
      {
        curr = GetRight(curr, gridp);
      }
      new_line[x] = gridp->pixels_ptr[curr.y][curr.x];
      ++x;
    }

    // Erase old line
    free(gridp->pixel_links_ptrs[y]);
    // Change ptr
    gridp->pixels_ptr[y] = new_line;
  }
  gridp->width -= width_to_cut;
}
