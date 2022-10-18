
  ! Name: David Appelhans
  ! Email: dappelha@gmail.com
  ! GitHub Username: dappelha
  ! Date Code Written: 10/17/2022
  ! Features Referenced: Parallel, Kernels, Cache, Tiled
  ! OpenACC Version: 3.2
  ! Compiler Used: nvfortran 22.3

  
  ! OpenACC examples of how to use the cache and tile clause to improve performance of codes with strided access patterns (for example matrix transpose).
  ! GPUs memory performance is best when reads and writes to global memory are stride 1 (coalesced memory access).
  ! Data layout may not lead to this naturally, but
  ! 1. using the tile clause can give hints to the compiler about data locality, which can help group access into fewer cache lines
  ! 2. Using the cache clause can give the developer a gang private scratch pad where the assignment of vectors to loop indices can be reversed to achieve coalesced accesses.
  ! 1 is similar to traditional cache blocking and 2 is similar to using the shared memory scratch space of a GPU.



module transposes

contains

  subroutine reference(A,B,n)
    ! Reference implementation for correctness checking
    implicit none

    ! passed variables
    integer, intent(in) :: n
    real(kind=8), intent(in) :: A(n,n)
    real(kind=8), intent(out) :: B(n,n)   

    ! local variables
    integer :: i,j

    do j=1,n
       do i=1,n
          B(j,i) = A(i,j)
       enddo
    enddo
    
    return

  end subroutine reference
  
  
  subroutine transpose_kernels(A,B,n) 
    implicit none

    ! passed variables
    integer, intent(in) :: n
    real(kind=8), intent(in) :: A(n,n)
    real(kind=8), intent(out) :: B(n,n)   

    ! local variables
    integer :: i,j

    !$acc kernels present(A,B)
    do j=1,n
       do i=1,n
          B(j,i) = A(i,j) ! coallesced read, uncoalesced write
       enddo
    enddo
    !$acc end kernels

    ! note same result on nvfortran 22.3 if used
    !!!    !$acc parallel loop gang vector collapse(2)
    
    return

  end subroutine transpose_kernels

  subroutine transpose_accgangvectcollapse(A,B,n) 
    implicit none

    ! passed variables
    real(kind=8), intent(in) :: A(:,:)
    real(kind=8), intent(out) :: B(:,:)   
    integer, intent(in) :: n
    ! local variables
    integer :: i,j

    !$acc parallel loop gang vector collapse(2)
    do j=1,n
       do i=1,n
          B(j,i) = A(i,j) ! coallesced read, uncoalesced write
       enddo
    enddo
    !$acc end parallel
    
    return

  end subroutine transpose_accgangvectcollapse

  
  subroutine transpose_accgangvecttile(A,B,n) 
    implicit none

    ! passed variables
    real(kind=8), intent(in) :: A(:,:)
    real(kind=8), intent(out) :: B(:,:)   
    integer, intent(in) :: n
    ! local variables
    integer :: i,j

    !$acc parallel loop gang vector tile(16,16)
    do j=1,n
       do i=1,n
          B(j,i) = A(i,j) ! coallesced read, uncoalesced write
       enddo
    enddo
    !$acc end parallel
    
    return

  end subroutine transpose_accgangvecttile


  
  subroutine transpose_acc2gang2vect(A,B,n) 
    implicit none

    ! passed variables
    real(kind=8), intent(in) :: A(:,:)
    real(kind=8), intent(out) :: B(:,:)   
    integer, intent(in) :: n
    ! local variables
    integer :: i,j, istart, jstart, ii, jj
    integer :: xthreads, ythreads

    
    ! pick good set of launch params
    xthreads = 16
    ythreads = 16

    !$acc parallel loop gang collapse(2) vector_length(16*16)
    do jstart=1, n, ythreads 
       do istart=1, n, xthreads 
          !$acc loop vector collapse(2)
          do jj = 1, ythreads
             do ii = 1, xthreads
                i = ii+istart-1
                j = jj+jstart-1
                if(i<=n .AND. j<=n) then
                   B(j,i) = A(i,j)
                endif
             enddo
          enddo

       enddo
    enddo    
    !$acc end parallel
    
    return

  end subroutine transpose_acc2gang2vect


  subroutine transpose_acc_cache(A,B,n) 
    implicit none

    ! passed variables
    real(kind=8), intent(in) :: A(:,:)
    real(kind=8), intent(out) :: B(:,:)   
    integer, intent(in) :: n
    ! local variables
    integer :: i,j, istart, jstart, ii, jj
    integer :: xthreads, ythreads
    real(kind=8) :: scratchpad(16,16)
    
    ! pick good set of launch params
    xthreads = 16
    ythreads = 16

    !$acc parallel loop gang collapse(2) vector_length(16*16) private(scratchpad)
    do jstart=1, n, ythreads 
       do istart=1, n, xthreads
          !$acc cache(scratchpad(:,:))
          !$acc loop vector collapse(2)
          do jj = 1, ythreads
             do ii = 1, xthreads
                i = ii+istart-1
                j = jj+jstart-1
                if(i<=n .AND. j<=n) then
                   scratchpad(ii,jj) = A(i,j) ! coalesced read from global mem
                endif
             enddo
          enddo ! <- implicit sync point here
          !$acc loop vector collapse(2)
          do ii = 1, xthreads
             do jj = 1, ythreads
                i = ii+istart-1
                j = jj+jstart-1
                if(i<=n .AND. j<=n) then
                   B(j,i) = scratchpad(ii,jj) ! coalesced write to global mem
                endif
             enddo
          enddo
       enddo
    enddo    
    !$acc end parallel
    
    return

  end subroutine transpose_acc_cache

  
end module transposes


program main
  use transposes
  use omp_lib
  use iso_c_binding
  implicit none
  integer :: n 
  

  character(len=20) :: buffer  
  real(kind=8), allocatable :: A(:,:), B(:,:), B_ref(:,:)
  !real(kind=8), device, allocatable :: d_A(:,:), d_B(:,:)
  real(kind=8) :: t1, t2, T, mem
  integer :: ierr, i, j, testnum, q
  real(kind=8),allocatable :: table(:,:)


  allocate( table(12*1024/128,5) )
  
  open (unit = 22, file = "/dev/stdout")
  write(22,*) "OpenACC Matrix Transpose Benchmarking"
 
  
  write(22,"( 6(A16,2X) )") "R+W Bytes (MB)", "kernels     ", "gangvectcollapse", "tile clause", "manual tile", "cache clause" 
  ! Total size of the data
  do n = 256, 4*1024, 128

     q = q + 1

     mem = 8*real(n*n,kind=8)/real(1000*1000*1000,kind=8)

     ! This kernel establishes the GPU context, which would otherwise
     ! be done on the first time running the kernels that we are timing (skewing the timings).
     !$acc kernels
     !do nothing, dummy kernel. 
     !$acc end kernels

     ! TEST 1

     testnum=1
     allocate(A(n,n), B(n,n))
     !$acc enter data create(A,B)     
     t1 = omp_get_wtime()
     call transpose_kernels(A,B,n)
     t2 = omp_get_wtime()
     T = t2-t1

     table(q,testnum)=2*mem/T

     !$acc exit data delete(A,B)

     deallocate(A,B)

     ! TEST 2

     testnum=2
     allocate(A(n,n), B(n,n))
     !$acc enter data create(A,B)     
     t1 = omp_get_wtime()
     call transpose_accgangvectcollapse(A,B,n)
     t2 = omp_get_wtime()
     T = t2-t1

     table(q,testnum)=2*mem/T

     !$acc exit data delete(A,B)

     deallocate(A,B)


     ! TEST 3

     testnum=3
     allocate(A(n,n), B(n,n))
     !$acc enter data create(A,B)     
     t1 = omp_get_wtime()
     call transpose_accgangvecttile(A,B,n)
     t2 = omp_get_wtime()
     T = t2-t1

     table(q,testnum)=2*mem/T

     !$acc exit data delete(A,B)

     deallocate(A,B)


     ! TEST 4

     testnum=4
     allocate(A(n,n), B(n,n))
     !$acc enter data create(A,B)     
     t1 = omp_get_wtime()
     call transpose_acc2gang2vect(A,B,n)
     t2 = omp_get_wtime()
     T = t2-t1

     table(q,testnum)=2*mem/T

     !$acc exit data delete(A,B)

     deallocate(A,B)


     ! TEST 5

     testnum=5
     allocate(A(n,n), B(n,n))
     !$acc enter data create(A,B)     
     t1 = omp_get_wtime()
     call transpose_acc_cache(A,B,n)
     t2 = omp_get_wtime()
     T = t2-t1

     table(q,testnum)=2*mem/T

     !$acc exit data delete(A,B)

     deallocate(A,B)

     
     write(22,"(6(f9.2,10X))") 2*mem*1000, table(q,:)
     !write(22,"(f9.4,T15,5(2X,f9.2))") 2*mem*1000, table(q,:)
     
  enddo


  
  print*, "completed"
end program main
