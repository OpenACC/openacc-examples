/* <---------------->
 * Name: Daniel Horta
 * Email: dchorta@udel.edu
 * GitHub Username: DandaMan65
 * Date Code Written: 3/10/2023
 * Features Referenced: copy, manual deep copy
 * OpenACC Version: 3.3
 * Compiler(s) Used: nvc openacc/22.11
 *
 * When copying pointers, compilers can struggle with derived types, such as a struct. 
 * Nested data isn't copied to the device and instead stay on the host. 
 * Manual Deep Copy allows all the data to be copied to the device. 
 * The following code proves that nested data on the device matches the nested data on the host.
 *
 * Special thanks to Mathew Colgrove from Nvidia for assistance in writing this code. 
 *
 <----------------> */
#include <stdio.h>
#include <stdlib.h>

int main(){
	typedef struct {
		int *x;
	} A;

	int size = 2;
	A *host = (A *)malloc(size * sizeof(A));
	A *device = (A *)malloc(size * sizeof(A));
	#pragma acc enter data create(host[:size])
	#pragma acc enter data create(device[:size])

	//Initialize host and device
	for(int i = 0; i < size; i++){
		host[i].x = (int *)malloc(size * sizeof(int));
		device[i].x = (int *)malloc(size * sizeof(int));
		
		for(int j = 0; j < size; j++){
			host[i].x[j] = i * size + j;
		}
		#pragma acc enter data copyin(host[i].x[:size])
		#pragma acc enter data create(device[i].x[:size])

	}

	//Begin deep copy
	#pragma acc parallel loop present(host, device)
	for(int i = 0; i < size; i++){
		#pragma acc loop vector
		for(int j = 0; j < size; j++){
			device[i].x[j] = host[i].x[j];
		}
	}

	//Check to see if copy was correct
	for (int i = 0; i < size; i++) {
		#pragma acc update self(device[i].x[:size])
    		for (int j = 0; j < size; j++) {
      			if (device[i].x[j] != host[i].x[j]) {
        			printf("Error: Copy failed\n");
        			return 1;
      			}
    		}
  	}

  	// Free memory
  	for (int i = 0; i < size; i++) {
		#pragma acc exit data delete(host[i].x, device[i].x)
    		free(host[i].x);
    		free(device[i].x);
  	}
	#pragma acc exit data delete(host,device)
  	free(host);
  	free(device);

	return 0;
}
