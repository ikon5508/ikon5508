#include <stdio.h>

int main ()
	 {

printf ("hello\n");


#ifdef __FreeBSD__
printf ("hello bsd\n");
#endif


#ifdef __linux__
printf ("hello linux\n");
#endif



return 0;

}
