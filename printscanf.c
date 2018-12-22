#include <stdio.h>

int main () {
   char str1[20], str2[30];

	float num;


   printf("Enter name: ");
	scanf ("%[^\n]s", &str1);
/*   scanf("%s", str1); */

   printf("Enter your website name: ");
   scanf("%s", &str2);

   printf("Entered Name: %s\n", str1);
   printf("Entered Website:%s\n", str2);
   
	printf("Number: \n");
	scanf ("%f", &num);

printf("%f", num);
 

   return(0);
}
