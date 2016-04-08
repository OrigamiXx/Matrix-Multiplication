#include <stdio.h>

#define LOWER -50
#define UPPER 100
#define STEP 10
int main(){
	printf("Hello, World!\n");

	//return 0;
	/* print celsius-fahrenheit table
	*/
	int fahr, celsius;
	int lower, upper, step;
	lower = -50;
	upper = 100;
	step = 10;

	celsius = lower;
	while (celsius <= upper){
		fahr = 1.8*celsius + 32;
		printf(" celsius %d\tfahrenheit %d\n", celsius, fahr);
		celsius += step;
	}

	for (celsius = LOWER; celsius <= UPPER; celsius = celsius + STEP)
		printf("celsius %3d fahrenheit %4.1f\n", celsius, 1.8*celsius + 32);

	return 0;

}