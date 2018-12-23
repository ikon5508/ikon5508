#include <stdio.h>

struct day {
int count;
float rlist[30];
}day;


int main () {

	float avg, sum, dfare;

	int i = 0;
	
for (i = 0; i < 30;i++) {

printf("Enter Ride Total ");
scanf("%f", &day.rlist[i]);
day.count = i;
if (day.rlist[i] == 0){ break; }

}


for (i=0; i<day.count; i++) {
sum = sum + day.rlist[i];
//printf("%f\n", day.rlist[i]);
}

avg = sum / day.count;
dfare = sum * 0.4;

printf("sum is %f\n", sum);
printf("Driver fare is %f\n", dfare);

printf("Average is %f\n", avg);

   return(0);
}
