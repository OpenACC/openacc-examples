/*
 *	Name: Daniel Horta
 *	Email: dchorta@udel.edu
 *	GitHub Username: DandaMan65
 *	Date Code Written: 10/16/2022
 *	Features Referenced: Parallel, Reduction
 *	OpenACC Version: 3.2
 *	Compiler(s) Used: NVC 21.7
 *
 *	This is a common version of the Jacobi Iteration, an algorithm that iteratively converges to the correct value by computing new values at each point from the average of neighboring points.
 */

#include <stdio.h>
#include <stdlib.h>
#include <openacc.h>

while(error > tol && iter < iter_max) {
	error = 0.0;

	#pragma acc parallel loop reduction(max:error)
	for(int j = 1; j < n-1; j++){
		#pragma acc loop reduction(max:error)
		for(int i = 1; i < m-1; i++){
			A[j][i] = 0.25 * (Anew[j][i+1] + Anew[j][i-1] + Anew[j-1][i] + Anew[j+1][i]);
			error = fmax(error, fabs(A[j][i] - Anew[j][i]));
		}
	}

	#pragma acc parallel loop
	for(int j = 1; j < n-1; j++){
		#pragma acc loop
		for(int i = 1; i < m-1; i++){
			A[j][i] = Anew[j][i];
		}
	}

	if(iter % 100 == 0) printf("%5d, %0.6f\n", iter, error);

	iter++;
}
