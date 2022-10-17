/*
 *	Name: Daniel Horta
 *	Email: dchorta@udel.edu
 *	GitHub Username: DandaMan65
 *	Date Code Written: 10/17/2022
 *	Features Referenced: Parallel, Loop
 *	OpenACC Version: 3.2
 *	Compiler(s) Used: NVC 22.3
 *
 *	This is a common example of a combined construct. The parallel construct starts parallel execution on the current device. The loop construct describes what type of parallism to use to execute the loop, as well as describe reduction operations, such as below.
 */

#include <stdio.h>
#include <stdlib.h>
#include <openacc.h>

int main(){
int sum = 0;
        double * a = (double *)malloc(100 * sizeof(double));

        for (int i = 0; i < 100; i++){
                a[i] = 1;
        }

        #pragma acc parallel loop reduction(-:sum)
        for (int i = 0; i < 100; i++){
                sum -= a[i];
        }

        if (sum == -100){
                return 0;
        }
        else{
                return 1;
        }	
}
