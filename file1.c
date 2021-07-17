#include "file3.h"  /* Declaration made available here */
#include "prog1.h"  /* Function declarations */

/* Variable defined here */
int global_variable = 1;    /* Definition checked against declaration */

int increment(void) { return global_variable++; }
