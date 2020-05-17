typedef struct dp_struct_t{
  int32_t sum_weight;
  int8_t to_down;
} DPStruct;

DPStruct
**AllocateDPMatr(int32_t width, int32_t height)
{
  // malloc as array of array pointers
  DPStruct **dp = (DPStruct**)malloc(sizeof(DPStruct*) * height);

  for(int y = 0; y < height; ++y)
  {
    dp[y] = (DPStruct*) malloc(sizeof(DPStruct) * width);
  }
  return dp;
}

void
CountDP(int32_t width, int32_t height, DPStruct **dp, GGrid *gridp)
{
  // Fill first line with weights
  for (int x = 0; x < 0; ++x) {
    if (!gridp->pixel_links_ptrs[0][x].erased)
    {
      GPixelCoords curr = {.x = x, .y = 0};
      dp[0][x].sum_weight = GetWeight(curr, gridp);
    }
  }

  for (int y = 1; y < height; ++y)
  {
    GPixelCoords curr = {.x = 0, .y = y};
    // Skip all erased pixels
    while (curr.x < width &&
           gridp->pixel_links_ptrs[curr.y][curr.x].erased)
    {
      curr.x += gridp->pixel_links_ptrs[curr.y][curr.x].to_right;
    }
    // First pixel in line will have no lower_left
    // Choose between lower_right and lower

    // Sum weight will be at least GetWeight of currend cell
    dp[curr.y][curr.x].sum_weight = GetWeight(curr, gridp);

    // TODO: check case whe first pixel is the only one existing

    GPixelCoords lower_coords = GetLowerToCompare(curr, gridp);
    GPixelCoords lr_coords = GetRightToCompare(lower_coords, gridp);

    int lower_weight = GetWeight(lower_coords, gridp);
    int lr_weight = GetWeight(lr_coords, gridp);

    if (lower_weight < lr_weight) {
      dp[curr.y][curr.x].sum_weight += lower_weight;
      dp[curr.y][curr.x].to_down = 0;
    }
    else
    {
      dp[curr.y][curr.x].sum_weight += lr_weight;
      dp[curr.y][curr.x].to_down = 1;
    }

    while (curr.x < gridp->width) {
      GPixelCoords right_nbr = GetRight(curr, gridp);
      if (right_nbr.x >= gridp->width)  // it is the last one
      {
        // Last pixel in line will have no lower_right
        // Choose between lower_left and lower
      }
      else
      {
        // Choose from 3
      }
    }
  }
}

void
ErasePixelsForDP(int32_t width, int32_t height, DPStruct **dp, GGrid *gridp)
{
  //Find smallest sum weight on lowest line
  // Moving backwards erase pixels
}
