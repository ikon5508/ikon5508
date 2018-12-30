#include <stdio.h>

struct day {
int count;
float rlist[30];
}day;

char fname[20];

int main () {

	float avg, sum, dfare = 0;
	char v[30];
	FILE *fpt;
	int i = 0;
int a;
	printf("Please Enter Date: ");
	scanf("%s", &fname);
	
	for (i = 0; i < 30;i++) {
		
		printf("Enter Ride Total ");
		scanf("%f", &day.rlist[i]);
		sum = sum + day.rlist[i];

	if (day.rlist[i] == 0){
	day.count = i;
	
	 break;}

	}

avg = sum / day.count;
dfare = sum * 0.4;

fpt = fopen (fname,"w");

fprintf (fpt, "%d\n", day.count);

for (i=0; i<day.count; i++) {

sprintf(v, "%.2f", day.rlist[i]);

printf("%s\n", v);
fprintf (fpt, "%s\n", v);
}




 fclose (fpt);

printf("sum is %.2f\n", sum);
printf("Driver fare is %.2f\n", dfare);

printf("Average is %.2f\n", avg);

   return(0);
}
