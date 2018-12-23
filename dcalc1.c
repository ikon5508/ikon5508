#include <stdio.h>

struct day {
int count;
float rlist[30];
}day;


int main () {

	float num1, num2, num3;
	float intake;

	int i;
	
for (i = 0; i < 30;i++) {

printf("Enter Ride Total ");
scanf("%f", &day.rlist[i]);
day.count = i;
if (day.rlist[i] == 0){ break; }

}


for (i=0; i<day.count; i++) {

printf("%f\n", day.rlist[i]);
}

   return(0);
}
