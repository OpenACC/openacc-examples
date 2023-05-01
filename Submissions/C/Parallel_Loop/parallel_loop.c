/*
 *	Name: Daniel Horta
 *	Email: dchorta@udel.edu
 *	GitHub Username: DandaMan65
 *	Date Code Written: 10/6/2022
 *	Features Referenced: Parallel, Loop
 *	OpenACC Version: 3.2
 *	Compiler(s) Used: NVC 21.7
 *
 *	A loop that cycles through the values of array a and assigns the values of array b as the sum of a and a + 1. 
 *
 * */

void pcloop(int n, float *a, float *b){
#pragma acc parallel loop
    for(int i = 0; i < n; i++){
        b[i] = a[i] + a[i + 1];
    }
}
