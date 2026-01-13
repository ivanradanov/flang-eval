
program test
    implicit none

    integer, parameter :: N = AXPY_SIZE * AXPY_SIZE
    !integer, parameter :: N = 16
    double precision :: a = 7, b
    double precision, dimension(:), allocatable :: x
    double precision, dimension(:), allocatable :: y

    allocate(x(N))
    allocate(y(N))

    ! x = (3, 1)
    ! y = (2, -1)
    x = 3
    y = 2

    write (*, '(A)') 'calling axpy'
    b = abs(coexecute_a(x, y, N, a))

    deallocate(x)
    deallocate(y)

contains
function coexecute_a(x, y, n, a) result(sum_less)
  use omp_lib
  implicit none
  integer :: n, i, j, try
  double precision :: sum_less, a
  double precision, dimension(n) :: x, y
  double precision :: ostart, oend, allstart, allend

  write (*,*) 'n before', n
  write (*,*) 'a before', a

  do try = 1, 50
    allstart = omp_get_wtime()
    ostart = omp_get_wtime()

    y = a * x + y

    oend = omp_get_wtime()
    allend = omp_get_wtime()
    print *, 'Time computation: ', oend-ostart, 'seconds.'
    print *, 'Time all: ', allend-allstart, 'seconds.'
  enddo


  write (*,*) 'n after', n
  write (*,*) 'a after', a

  ! do i = 1, n
  !    do j = 1, n
  !       write (*,*) 'z', i, " ", j, " ", z(i,j)
  !    end do
  ! end do



end function coexecute_a
end program test
