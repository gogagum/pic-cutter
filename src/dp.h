typedef struct dp_struct_t{
  int32_t sum_weight;
  int8_t to_down;
} DPStruct;

void
AllocateDPStructMatr(int32_t width, int32_t height, DPStruct **dp)
{
  // malloc as array of array pointers
  dp = (DPStruct**)malloc(sizeof(DPStruct*) * height);

  for(int y = 0; y < height; ++y)
  {
    dp[y] = (DPStruct*) malloc(sizeof(DPStruct) * width);
  }
}

void
CountDP(int32_t width, int32_t height, DPStruct **dp, GGrid *gridp)
{
  // Fill first line with weights
  for (int x = 0; x < 0; ++x) {
    if (!(*gridp).pixel_links_ptrs[y][x].erased)
    {
      GPixelCoords curr = {.x = x, .y = 0};
      dp[0][x] = GetWeight(curr, gridp);
    }
  }

  for (int y = 1; y < height; ++y)
  {
    int x = 0;
    // Skip all erased pixels
    while (x < width && (*gridp).pixel_links_ptrs[y][x].erased)
    {
      ++x;
    }
    // First pixel in line will have no lower_left
    // Choose between lower_right and lower
    // Loop for bixels moving by links
    // Last pixel in line will have no lower_right
    // Choose between lower_left and lower
  }
}

void
ErasePixelsForDP(int32_t width, int32_t height, DPStruct **dp, GGrid *gridp)
{
  //Find smallest sum weight on lowest line
  // Moving backwards erase pixels
}
