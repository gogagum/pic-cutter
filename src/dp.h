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
  #ifdef DEBUG
  printf("%s\n", "CountDP");
  #endif

  // Fill first line with weights
  for (int x = 0; x < 0; ++x) {
    if (!gridp->pixel_links_ptrs[0][x].erased)
    {
      GPixelCoords curr = {.x = x, .y = 0};
      dp[0][x].sum_weight = GetWeight(curr, gridp);
    }
  }

  #ifdef DEBUG
  printf("%s\n", "Filled first line");
  #endif

  for (int y = 1; y < height; ++y)
  {
    #ifdef DEBUG
    printf("%s%i\n", "Current y: ", y);
    #endif
    GPixelCoords curr = {.x = 0, .y = y};
    // Skip all erased pixels
    while (curr.x < width &&
           gridp->pixel_links_ptrs[curr.y][curr.x].erased)
    {
      curr = GetRight(curr, gridp);
      //curr.x += gridp->pixel_links_ptrs[curr.y][curr.x].to_right;
    }

    #ifdef DEBUG
    printf("%s%i %i\n", "current coords: ", curr.x, curr.y);
    #endif
    // First pixel in line will have no lower_left
    // Choose between lower_right and lower

    // Sum weight will be at least GetWeight of currend cell
    dp[curr.y][curr.x].sum_weight = GetWeight(curr, gridp);

    GPixelCoords lower_coords = GetLower(curr, gridp);
    GPixelCoords lr_coords = GetRight(lower_coords, gridp);

    #ifdef DEBUG
    printf("%s%i %i\n", "lower coords: ", lower_coords.x, lower_coords.y);
    printf("%s%i %i\n", "lr coords: ", lr_coords.x, lr_coords.y);
    #endif

    // Pixel is solo
    if (lr_coords.x >= gridp->width)
    {
      dp[curr.y][curr.x].sum_weight+= GetWeight(lower_coords, gridp);
      dp[curr.y][curr.x].to_down = 0;
      continue;
    }

    int lower_weight = GetWeight(lower_coords, gridp);
    int lr_weight = GetWeight(lr_coords, gridp);
    int ll_weight = 0;

    if (lower_weight < lr_weight) {
      dp[curr.y][curr.x].sum_weight += lower_weight;
      dp[curr.y][curr.x].to_down = 0;
    }
    else
    {
      dp[curr.y][curr.x].sum_weight += lr_weight;
      dp[curr.y][curr.x].to_down = 1;
    }

    GPixelCoords ll_coords = lower_coords;
    lower_coords = lr_coords;
    lr_coords = GetRight(lower_coords, gridp);

    curr = GetRight(curr, gridp);

    #ifdef DEBUG
    printf("%i %i\n", curr.x, curr.y);
    #endif

    while (curr.x < gridp->width) {
      #ifdef DEBUG
      printf("%s%i %i\n", "current coords in main: ", curr.x, curr.y);
      #endif

      dp[curr.y][curr.x].sum_weight = GetWeight(curr, gridp);
      if (lr_coords.x >= gridp->width)  // it is the last one
      {
        // Last pixel in line will have no lower_right
        // Choose between lower_left and lower
        ll_weight = GetWeight(ll_coords, gridp);
        lower_weight = GetWeight(lower_coords, gridp);

        if (lower_weight < ll_weight) {
          dp[curr.y][curr.x].sum_weight += lower_weight;
          dp[curr.y][curr.x].to_down = 0;
        }
        else
        {
          dp[curr.y][curr.x].sum_weight += ll_weight;
          dp[curr.y][curr.x].to_down = -1;
        }
      }
      else  // Choose from 3
      {
        ll_weight = GetWeight(ll_coords, gridp);
        lr_weight = GetWeight(lr_coords, gridp);
        lower_weight = GetWeight(lower_coords, gridp);

        if (ll_weight < lr_weight && ll_weight < lower_weight)
        {
          dp[curr.y][curr.x].sum_weight += ll_weight;
          dp[curr.y][curr.x].to_down = -1;
        }
        else if (lr_weight < lower_weight)
        {
          dp[curr.y][curr.x].sum_weight += lr_weight;
          dp[curr.y][curr.x].to_down = 1;
        }
        else
        {
          dp[curr.y][curr.x].sum_weight += lower_weight;
          dp[curr.y][curr.x].to_down = 0;
        }
        // Move to the right
        ll_coords = GetRight(ll_coords, gridp);
        lr_coords = GetRight(lr_coords, gridp);
        lower_coords = GetRight(lower_coords, gridp);
      }
      curr = GetRight(curr, gridp);
    }
  }
}

void
ErasePixelsAccordingToDP(DPStruct **dp, GGrid *gridp)
{
  #ifdef DEBUG
  printf("%s\n", "ErasePixelsAccordingToDP");
  #endif


  //Find smallest sum weight on lowest line
  int start_x = 0;
  int best_weight = (~0)-1;  // TODO better inf const

  for (int x = 0; x < gridp->width; ++x)
  {
    if (!gridp->pixel_links_ptrs[gridp->height-1][x].erased) {
      if (dp[gridp->height-1][x].sum_weight < best_weight) {
        best_weight = dp[gridp->height-1][x].sum_weight;
        start_x = x;
      }
    }
  }

  #ifdef DEBUG
  printf("%s\n", "Found starting x.");
  #endif



  // Moving backwards erase pixels
  GPixelCoords curr = {
    .x = start_x,
    .y = gridp->height-1
  };

  for (int y = gridp->height - 1; y >= 0; --y) {
    #ifdef DEBUG
    printf("%s%i\n", "Current y: ", y);
    #endif
    gridp->pixel_links_ptrs[curr.y][curr.x].erased = true;
    GPixelCoords left = GetLeft(curr, gridp);
    GPixelCoords right = GetRight(curr, gridp);
    if (left.x >= 0) {
      gridp->pixel_links_ptrs[left.y][left.x].to_right +=
          gridp->pixel_links_ptrs[curr.y][curr.x].to_right;
    }
    if (right.x < gridp->width) {
      gridp->pixel_links_ptrs[right.y][right.x].to_left +=
          gridp->pixel_links_ptrs[curr.y][curr.x].to_left;
    }
    // TODO: correct lower and upper links

    if (y == 0) {
      break;
    }
    if (dp[curr.y][curr.x].to_down == 0) {
      curr = GetLower(curr, gridp);
    }
    else if (dp[curr.y][curr.x].to_down == -1)
    {
      curr = GetLower(curr, gridp);
      curr = GetLeft(curr, gridp);
    }
    else
    {
      curr = GetLower(curr, gridp);
      curr = GetRight(curr, gridp);
    }
  }
}
