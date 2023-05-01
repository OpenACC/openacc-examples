#include "acc_testsuite.h"

int main(){
	int err = 0;
    	int device_type = acc_get_device_type();

    	#pragma acc set device_type(default)
    	if (acc_get_device_type() != device_type){
        	err += 1;
    	}

    	return err;
}
