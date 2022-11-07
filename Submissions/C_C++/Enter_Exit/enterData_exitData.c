1   /* <---------------->
2   Your Name: Aaron Liu (Optional)
3   Email: Olympus@udel.edu (Optional)
4   GitHub Username: AaronLiu20 (Optional)
5   Date Code Written: 10/10/2022 (Optional)
6   Features Referenced: parallel,loop,enter data,copyin,present,reduction,exit data,copyout
7   OpenACC Version: 3.2
8   Compiler(s) Used: Nvidia 22.3
9
10  this declares a seperate data region that is can have multiple compute regions inside of it
    Using the reference counter, it changes the behavior of certain data clauses
11  <----------------> */

#include <stdio.h>
#include <stdlib.h>

int main(){
    double *Data1 = (double *)malloc( 100 * sizeof(double));
    double *Data2 = (double *)malloc( 100 * sizeof(double));
    double *Data3 = (double *)malloc( 100 * sizeof(double));
    double sum1 = 0.0;
    double sum2 = 0.0;
    double sum3 = 0.0;
    double test1 = 0.0;
    double test2 = 0.0;
    double test3 = 0.0;
    int error = 0;


    for( int x = 0; x < 100; ++x){
        Data1[x] = x;
        Data2[x] = 2 * x;
        Data3[x] = 3 * x;
        test1 += Data1[x];
        test2 += Data2[x];
        test3 += Data3[x];
    }
    //the use of the enter data creates a data region with copyin making copes of the data
    #pragma acc enter data copyin(Data1[0:100], Data2[0:100], Data3[0:100], sum1, sum2, sum3)

    //this is a compute region wrapped in a data region that must declare the variables to use
    //due to the fact that
    //1.) the structured reference counter has been infremented by the enter data clause
    //2.) the dynamic reference counter has been incremented by the present clause
    //3.) this copyout only decrements the dynamic reference counter
    //the copyput action wll only be perfomred when the exit data decremnents the strucuted reference counter
    #pragma acc parallel loop present(Data1[0:100], sum1) reduction(+:sum1) copyout(sum1)
    for(int x = 0; x < 100; ++x){
        sum1 += Data1[x];
    }

    #pragma acc parallel loop present(Data2[0:100], sum2) reduction(+:sum2) copyout(sum2)
    for(int x = 0; x < 100; ++x){
        sum2 = Data2[x];
    }

    #pragma acc parallel loop present(Data3[0:100], sum3) reduction(+:sum3) copyout(sum3)
    for(int x = 0; x < 100; ++x){
        sum3 = Data3[x];
    }
    #pragma acc exit data copyout(sum1, sum2, sum3)
    //since both the strucutred and dynamic reference counters are 0, then the copyout action is performed
    if( test1 != sum1){
        error += (1 << 0);
    }
    if( test2 != sum2){
        error += (2 << 0);
    }
    if( test3 != sum3){
        error += (3 << 0);
    }
    //this is a tester to make sure the values are correct
    return error;
}
