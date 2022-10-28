/*1   <---------------->
2   Your Name: Aaron Liu
3   Email: Olympus@ude.edu
4   GitHub Username: AaronLiu20
5   Date Code Written: 10/24/2022
6   Features Referenced: independent clause
7   OpenACC Version: 3.2
8   Compiler(s) Used: Nvidia
9
10 this is a example of the explicit declaring data into the data region that can be used wtihin the compute regions
11  <----------------> */

#include <stdio.h>
#include <stdlib.h>

int main(){
    double * Data1 = (double *)malloc( 100 * sizeof(double));
    double * Data2 = (double *)malloc( 100 * sizeof(double));

    for( int x = 0; x < 100; ++x){
        Data1[x] = x;
        Data2[x] = x;
    }

    int error = 0;

    //independent clause is implicted used when
    //1.) if there is no auto or seq clause within withe loop construct
    //2.) it is a ophaned loop construct or the parent compute region is a parallel construct        
    
    #pragma acc parallel loop copy(Data1[0:100])
    for(int x = 0; x < 100; ++x){
        Data1[x] += Data1[x];
    }

    #pragma acc parallel copy(Data2[0:100])
    {
        #pragma loop
        for(int x = 0; x < 100; ++x){
            Data2[x] += Data2[x];
        }

    }

    //independent clause allows for no synchronization between the parallel regions

    //this clause behaves differently for
    //1.) variables within the reduction clause since combining the values is the last step          
    //2.) compute region has atomic clause

    for( int x = 0; x < 100; ++x){
        if(Data1[x] - Data2[x] > .00001){
            error += 1;
        }
    }

    return error;
}
