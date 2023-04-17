
/*1   <---------------->
2   Your Name: Aaron Liu
3   Email: Olympus@ude.edu
4   GitHub Username: AaronLiu20
5   Date Code Written: 3/9/2023
6   Features Referenced: collapse
7   OpenACC Version: 3.3
8   Compiler(s) Used: Nvidia
9
10 this is a example of the explicit declaring data into the data region that can be used wtihin the compute regions
11  <----------------> */

#include <stdio.h>
#include <stdlib.h>

int main(){
    int n = 100;
    double **data = (double **)malloc(n * sizeof(double*));
    for(int x = 0; x < n; ++x){
	    data[x] = (double *)malloc(n * sizeof(double));
	    for(int y = 0; y < n; ++y){
	        data[x][y] = rand()%100;
	    }
    }

    double sum = 0.0;
    //collapse clause
    //1.)It will dividing the different itterations into chucks with all of the iterators within the clause 
    //2.)the force clause(the 2 within the collapse) tells the compiler how many for loops are within the collapse clause

    //reduction clause combines all of the values of the variable within the threads at the end
    #pragma acc parallel loop copyin(data[0:n][0:n]) copyout(sum) reduction(+:sum) collapse(2)
    for(int x = 0; x < n; ++x){
        for(int y= 0; y < n; ++y){
            sum += data[x][y];
        }
    }
    return 0;
}
