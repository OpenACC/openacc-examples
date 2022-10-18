# nvhpc compiler
	F90=nvfortran
	#CUDAFLAGS = -Mcuda=cc70,nordc,maxregcount:64,ptxinfo,lineinfo,nounroll -fast -Mfprelaxed -L/usr/local/cuda/lib64 -lnvToolsExt
	CUDAFLAGS = -Mcuda=cc70,nordc,maxregcount:64,ptxinfo,lineinfo,nounroll -Mfprelaxed 
	#CUDAFLAGS+= -Minfo=inline,accel -acc=noautopar
	CUDAFLAGS+= -acc=noautopar	
	F90FLAGS=-gopt -mp -Minfo -Mnounroll -Mnovect -fast
	#F90FLAGS=-gopt -mp -Minfo -Munroll -Mnovect



build: driver.o
	${F90} -o ${comp}test driver.o ${F90FLAGS} ${CUDAFLAGS} ${LINKFLAGS}

driver.o: driver.F90
	${F90} -c -o driver.o driver.F90 ${F90FLAGS} ${CUDAFLAGS}

clean:
	rm -f ${comp}test *.o *.mod

