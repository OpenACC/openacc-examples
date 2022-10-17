
1   /* <---------------->
2   Your Name: Aaron Liu
3   Email: Olympus@ude.edu
4   GitHub Username: AaronLiu20
5   Date Code Written: 10/17/2022
6   Features Referenced: copy
7   OpenACC Version: 3.2
8   Compiler(s) Used: Nvidia
9
10 this is a example of the explicit declaring data into the data region that can be used wtihin the compute regions
11  <----------------> */

#include <stdio.h>
#include <stdlib.h>

int main(){
    double * data = (double *)malloc( 100 * sizeof(double));
    for( int x = 0; x < 100; ++x){
	data[x] = x;	    
    }

    double sum = 0.0;
    #pragma acc parallel copy(data[0:100], sum) reduction(+:sum)
    for(int x = 0; x < 100; ++x){
	sum += data[x];
    }

    return 0;
}
