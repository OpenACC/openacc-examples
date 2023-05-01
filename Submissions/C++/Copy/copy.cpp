#include <iostream>
#include <stdlib.h>

int main(){
    double * data = (double *)malloc( 100 * sizeof(double));
    for( int x = 0; x < 100; ++x){
	data[x] = x;	    
    }

    double sum = 0.0;
    
    //copy clause
    //1.) performes the copyin action at the start of the data region for variables data and sum
    //2.) performs the copyout action at the end of the data region for variables data and sum
    //reduction clause combines all of the values of the variable within the threads at the end
    #pragma acc parallel copyin(data[0:100], sum) copyout(sum) reduction(+:sum)
    for(int x = 0; x < 100; ++x){
	sum += data[x];
    }

    return 0;
}
