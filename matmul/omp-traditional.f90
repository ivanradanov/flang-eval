
program test
    implicit none

    integer, parameter :: N = MATMUL_SIZE
    !integer, parameter :: N = 16
    double precision :: a = 7, b
    double precision, dimension(:, :), allocatable :: x
    double precision, dimension(:, :), allocatable :: y
    double precision, dimension(:, :), allocatable :: z

    allocate(x(N, N))
    allocate(y(N, N))
    allocate(z(N, N))

    ! x = (3, 1)
    ! y = (2, -1)
    x = 3
    y = 2
    z = 0

    write (*, '(A)') 'calling axpy'
    b = abs(coexecute_a(x, y, z, N, a))

    deallocate(x)
    deallocate(y)
    deallocate(z)

contains
function coexecute_a(x, y, z, n, a) result(sum_less)
  use omp_lib
  implicit none
  integer :: n, i, j, k, try
  double precision :: sum_less, a
  double precision, dimension(n, n) :: x, y, z
  double precision :: ostart, oend, allstart, allend

  write (*,*) 'n before', n
  write (*,*) 'a before', a
  write (*,*) 'z(1,1) before', z(1,1)
  write (*,*) 'checksum before', sum(z(1:n, 1:n))

  do try = 1, 3
    allstart = omp_get_wtime()
    !$omp target data map(tofrom:x,y,z)
    ostart = omp_get_wtime()
    !$omp target teams distribute parallel do collapse(2)
    do i = 1, n
      do j = 1, n
        z(j, i) = 0
        do k = 1, n
          z(j, i) = z(j, i) + x(j, k) * y(k, i)
        enddo
      enddo
    enddo
    !$omp end target teams distribute parallel do
    oend = omp_get_wtime()
    !$omp end target data
    allend = omp_get_wtime()
  enddo


  write (*,*) 'n after', n
  write (*,*) 'a after', a
  write (*,*) 'z(1,1) after', z(1,1)
  write (*,*) 'checksum after', sum(z(1:n, 1:n))

  ! do i = 1, n
  !    do j = 1, n
  !       write (*,*) 'z', i, " ", j, " ", z(i,j)
  !    end do
  ! end do

  print *, 'Time computation: ', oend-ostart, 'seconds.'
  print *, 'Time all: ', allend-allstart, 'seconds.'

  sum_less = sum(z(1:n/2,1:n/3) - 2) / ( n * n)

end function coexecute_a
end program test
