void pcloop(int n, float *a, float *b){
#pragma acc parallel loop
    for(int i = 0; i < n; i++){
        b[i] = a[i] + a[i + 1];
    }
}
