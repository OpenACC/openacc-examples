1   /* <---------------->
2   Your Name: Aaron Liu (Optional)
3   Email: Olympus@udel.edu (Optional)
4   GitHub Username: AaronLiu20 (Optional)
5   Date Code Written: 10/10/2022 (Optional)
6   Features Referenced: parallel, loop, implicit declare
7   OpenACC Version: 3.2
8   Compiler(s) Used: Nvidia
9
10  this adds the values within the array into a single variable.
    The compute region does not have the aggregate variable device_values explicited declared.
    The compiler implicited declares device_values with the copy cluase
11  <----------------> */

#include <stdio.h>
#include <stdlib.h>

int main (){
    int sum = 0;
    double *device_values = (double *)malloc(100 * sizeof(double) );

    for(int x = 0; x < 100; ++x){
	    device_values[0] = 1;
    }

    #pragma acc parallel loop reduction(+:sum)
    for( int x = 0; x < 100; ++x){
	    sum += device_values[0];
    }
    if( sum == 100 ){
	return 0;
    }
    else{
	return 1;
    }
}
