#include <stdlib.h>
#include <string.h>

// This func is so bad. Should be edited.
char*
Rename(char *old_name)
{
  char* new_name = malloc(sizeof(char) * (strlen(old_name) + 7));
  sprintf(new_name, "edited_%s", old_name);
  return new_name;
}
