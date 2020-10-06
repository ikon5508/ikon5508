#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main (void)
{
  DIR *dp;
  struct dirent *ep;     
  dp = opendir ("./");

  if (dp != NULL)
  {
    while (ep = readdir (dp))
	{ 
	if (ep->d_type == 4) 
// 4 is dir 8 is file
    printf ("type: %uc, %s\n", ep->d_type, ep->d_name);
	}


    (void) closedir (dp);
  }
  else
    perror ("Couldn't open the directory");

  return 0;
}
