#include <stdio.h>

#ifndef PROGRESS_H
#define PROGRESS_H

void
print_progress(size_t count, size_t max)
{
  count = (count * 10000 / max) / 100 + 1;
  max = 100;
  printf("%li %%\n", count);
}

#endif   // PROGRESS_H
