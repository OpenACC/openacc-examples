/*
 *	Name: Daniel Horta
 *	Email: dchorta@udel.edu
 *	GitHub Username: DandaMan65
 *	Date Code Written: 10/16/2022
 *	Features Referenced: Parallel. Reduction
 *	OpenACC Version: 3.2
 *	Compiler(s) Used: NVC 21.7
 *
 *	A loop cycles through array a and assigns each value as 1. I then loop through array a, adding each value to sum using parallel reduction.
 */

#include <stdio.h>
#include <stdlib.h>
#include <openacc.h>

int main(){
	int sum = 0;
	double * a = (double *)malloc(10 * sizeof(double));

	for (int i = 0; i < 10; i++){
		a[i] = 1;
	}

	#pragma acc parallel loop reduction(+:sum)
	for (int i = 0; i < 10; i++){
		sum += a[i];
	}

	if (sum == 10){
		return 0;
	}
	else{
		return 1;
	}
}
