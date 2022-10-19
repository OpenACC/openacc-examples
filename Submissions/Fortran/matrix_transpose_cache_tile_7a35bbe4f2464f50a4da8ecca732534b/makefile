# nvhpc compiler
	F90=nvfortran
	#GPUFLAGS = -Mcuda=cc70,nordc,maxregcount:64,ptxinfo,lineinfo,unroll -Mfprelaxed
	GPUFLAGS = -Mcuda=cc70,nordc,maxregcount:64,ptxinfo,lineinfo -Mfprelaxed
	GPUFLAGS+= -acc=noautopar	
	#	F90FLAGS=-gopt -mp -Minfo -Munroll -Mnovect -fast
	F90FLAGS=-gopt -mp -Minfo -fast



build: driver.o
	${F90} -o ${comp}test driver.o ${F90FLAGS} ${GPUFLAGS} ${LINKFLAGS}

driver.o: driver.F90
	${F90} -c -o driver.o driver.F90 ${F90FLAGS} ${GPUFLAGS}

clean:
	rm -f ${comp}test *.o *.mod

