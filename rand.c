#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROLLS 10

int main()
{
    int t,r1,r2;

    /* Seed randomizer */
    srand((unsigned int)(time(NULL)));

    /* Roll dice ROLLS times */
    for(t=0;t<ROLLS;t++)
    {
        r1 = (rand() % 6) + 1;
        r2 = (rand() % 6) + 1;
        printf("Roll %d = %d\n",t+1,r1+r2);
    }

    return(0);
}
