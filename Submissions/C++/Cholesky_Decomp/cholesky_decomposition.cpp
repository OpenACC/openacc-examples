/*

<---------------------------------------------->

Author Information
---------------------
Name: 				  	Joel Vasanth (joelvv@alumni.iitm.ac.in)
GitHub Username:  joelvarunvasanth
Created: 					4th Aug 2022

Features Referenced:
---------------------
directives: 			parallel, loop, parallel loop, data
clauses: 					present, num_gangs, create, copyin, copyout, reduction

Version
--------
OpenACC Version: 3.1
Compiler(s) Used: NVIDIA PGI compiler: pgcc 20.7-0 target on x86-64 Linux

Code Description
----------------
This code is to test openacc on a  sample linear algebra algorithm - the Cholesky Decomposition.
The Cholesky Decomposition converts a real symmetric square matrix or a Hermitian matrix (if its entries are complex) A of size N into the product of two matrices - a lower triangular matrix L and its conjugate transpose L*. In equation format: A = LL* (* being complex conjugate transpose).
Further description and results can be found in the attached document.


<---------------------------------------------------->

*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define TYPE	float
#define N	10 // matrix size
#define NG 	1 // number of gangs


// INITIALISING THE MATRIX 'A': (Done on the GPU)
// --> A is a real symmetrix matrix with diagonal elements 1.0
// 		 and other elements ii+jj (ii and jj being row and column indices respectively)
// --> The choice of A is arbitrary. The size of A is N x N.
void initmult(TYPE mat[][N]) {
	#pragma acc parallel loop present(mat[0:N][0:N]) num_gangs(NG)
	for (int ii = 0; ii < N; ++ii) {
		for (int jj = 0; jj < ii; ++jj)  {
	      mat[ii][jj] = (ii+jj) / (float)N / N;
	      mat[jj][ii] = (ii+jj) / (float)N / N;
			}
	}
	#pragma acc parallel loop present(mat[0:N][0:N]) num_gangs(NG)
	for (int ii = 0; ii < N; ++ii)
	mat[ii][ii] = 1.0;
}

// COMPUTING THE DECOMPOSITION (Done on the GPU)
void cholesky(TYPE a[][N]) {
	double sm;
	#pragma acc parallel present(a[0:N][0:N]) num_gangs(NG)
	{
	for (int ii = 0; ii < N; ++ii) {
		for (int jj = 0; jj < ii; ++jj) {
			sm = a[ii][jj];
			#pragma acc loop reduction(+:sm)
			for (int kk = 0; kk < jj; ++kk)
           		 { sm += -a[ii][kk] * a[jj][kk]; }
			a[ii][jj] = sm/a[jj][jj];
		}
		sm = a[ii][ii];

	  #pragma acc loop reduction(+:sm)
		for (int kk = 0; kk < ii; ++kk)
			{ sm  += -a[ii][kk] * a[ii][kk]; }
		a[ii][ii] = sqrt(sm);
	}
	}
}


// PRINTING OUTPUT AFTER DECOMPOSITION (Done on the CPU)
void printMat(TYPE a[][N]) {
	for (int ii = N-10; ii < N; ++ii) {
		for (int jj = N-10; jj < N; ++jj)
			printf("%5.4f ", a[ii][jj]);
		printf("\n");
	}
}


int main() {
	TYPE a[N][N], sm;
	#pragma acc data create(a) copyin(sm) copyout(a[0:N][0:N])
	{
		initmult(a);
		cholesky(a);
	}
	printMat(a);
	return 0;
}
