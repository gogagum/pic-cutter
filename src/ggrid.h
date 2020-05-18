#include "gpixel.h"
#include "gpixel_linker.h"

#ifndef GGRID_H
#define GGRID_H

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

GPixelCoords
GetUpper(GPixelCoords curr, GGrid *gridp)
{
  GPixelCoords left_coords = {
    .x = curr.x + gridp->pixel_links_ptrs[curr.y][curr.x].to_upper,
    .y = curr.y + 1
  };
  return left_coords;
}

bool
IsRealPixel(GPixelCoords curr, GGrid *gridp)
{
  return curr.x < gridp->width && curr.x >= 0 &&
         curr.y < gridp->height && curr.y >= 0;
}

void
CntDistWithCheck(GPixelCoords curr_coords, GPixelCoords to_cmp_coords,
                 int32_t* weight_p, int32_t* to_cmp_cnt_p, GGrid *gridp)
{
  GPixel curr = gridp->pixels_ptr[curr_coords.y][curr_coords.x];
  if (IsRealPixel(to_cmp_coords, gridp))
  {
    GPixel to_cmp = gridp->pixels_ptr[to_cmp_coords.y][to_cmp_coords.x];
    (*weight_p) += ColorDistance(curr, to_cmp);
  }
  else
  {
    (*to_cmp_cnt_p)--;
  }
}

int32_t
GetWeight(GPixelCoords curr, GGrid *gridp)
{
  GPixelCoords lower = GetLower(curr, gridp);
  GPixelCoords upper = GetUpper(curr, gridp);;
  GPixelCoords left  = GetLeft(curr, gridp);
  GPixelCoords right = GetRight(curr, gridp);
  int32_t to_cmp_cnt = 4;
  int32_t weight = 0;
  CntDistWithCheck(curr, lower, &weight, &to_cmp_cnt, gridp);
  CntDistWithCheck(curr, upper, &weight, &to_cmp_cnt, gridp);
  CntDistWithCheck(curr, left, &weight, &to_cmp_cnt, gridp);
  CntDistWithCheck(curr, right, &weight, &to_cmp_cnt, gridp);
  weight *= 12;  // So weight is always divided by to_cmp_cnt

  return weight / to_cmp_cnt;
}

void
ApplyErasedPixelsFromGrid(int width_to_cut, GGrid *gridp)
{
  for (int y = 0; y < (*gridp).height; ++y)
  {
    // Create new line
    GPixel *new_line = malloc(sizeof(GPixel) * width_to_cut);
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
      curr = GetRight(curr, gridp);
    }

    // Erase old line
    free(gridp->pixels_ptr[y]);
    // Change ptr
    gridp->pixels_ptr[y] = new_line;
  }
  gridp->width = width_to_cut;
}

void
FreeGrid(GGrid *gridp)
{
  //FreePixelLinks(gridp->height, gridp->pixel_links_ptrs);
  for (int y = 0; y < gridp->height; ++y)
  {
    free(gridp->pixels_ptr[y]);
  }
  free(gridp->pixels_ptr);
}

#endif    // GGRID_H
