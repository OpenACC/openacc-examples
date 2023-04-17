
/*1  <---------------->
2   Your Name: Aaron Liu
3   Email: Olympus@ude.edu
4   GitHub Username: AaronLiu20
5   Date Code Written: 10/17/2022
6   Features Referenced: atomic construct with update, write, capture
7   OpenACC Version: 3.3
8   Compiler(s) Used: Nvidia
9
10 this is a example of the atomic construct that prevents multiple reads and writes on a variable
11  <----------------> */

#include <stdio.h>
#include <stdlib.h>

int main(){
    int n = 100;
    double * data = (double *)malloc( n * sizeof(double));
    for( int x = 0; x < n; ++x){
	    data[x] = x;	    
    }

    double readSum = 0.0;
    double writeSum = 0.0;
    double captureSum = 0.0;
    double updateSum = 0.0;

    // the atomic construct prevents reading the value while a gang/worker/vector is writing and vice versa
    //this is the read clause read the value of one variable into another variable
    #pragma acc parallel loop copy(data[0:n]) copyout(readSum)
    for(int x = 0; x < n; ++x){
        if(data[x] >= n/2){
            #pragma acc atomic write
            readSum = x;
        }
    }

    // the atomic construct prevents reading the value while a gang/worker/vector is writing and vice versa
    //this is the write clause that only allows a direct write to a variable from a expression
    #pragma acc parallel loop copy(data[0:n]) copyout(writeSum)
    for(int x = 0; x < n; ++x){
        if(data[x] >= n/2){
            #pragma acc atomic write
            writeSum = x*2 + 1;
        }
    }

    //this is the capture clause that the update to the expression into another variable
    #pragma acc parallel loop copy(data[0:n]) copyout(captureSum)
    for(int x = 0; x < n; ++x){
        if(data[x] >= n/2){
            #pragma acc atomic capture
            captureSum = data[x]--;
            //printf("%f. %f. %d\n", captureSum, data[x], x);
        }
    }

    printf("%f\n", captureSum);

    //this is the update clause which locks the update of a particualar variable from certain operations
    #pragma acc parallel loop copy(data[0:n]) copyout(updateSum)
    for(int x = 0; x < n; ++x){
        if(data[x] >= n/2){
            #pragma acc atomic update
            updateSum++;
        }
    }
    return 0;
}
