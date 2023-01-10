#include <stdio.h>
#include <unistd.h>

int main (int argc, char **argv)
{
sleep (5);
printf ("%s\n", argv[1]);
sleep (20);
printf ("%s\n", argv[1]);

}
