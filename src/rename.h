#include <stdlib.h>
#include <string.h>

// This func is so bad. Should be edited.
void
Rename(char* new_name, char *old_name)
{
  strcpy(new_name, old_name);
  size_t len = strlen(old_name);
  char * to_change = new_name + len - 4;
  sprintf(to_change, "%s", "_edited.png\0");
}
