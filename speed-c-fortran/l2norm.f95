! https://gcc.gnu.org/onlinedocs/gfortran/CPU_005fTIME.html
program test_cpu_time
    real :: start, finish
    integer :: size
    real :: numbers(1000000)
    real :: sum
    integer :: repeats
    integer :: argc
    print '("Fortran version")'
    size = 1000000
    repeats = 100

    argc = command_argument_count()
    print '("argc = ")'
    print *, argc
    do i=1,size
       numbers(i) = FLOAT(i)/FLOAT(size) + FLOAT(argc) - 1
    end do

    print '("numbers(20000) = ",f15.6,".")', numbers(20000)

    call cpu_time(start)
    sum = 0.0
    do j=1,repeats
       numbers = numbers * numbers
       do i=1,size
          sum = sum + numbers(i)
       end do
       ! no idea why the following gives "Error: Unclassifiable statement at (1)"
       !sum = sum + SUM(numbers)
    end do
    call cpu_time(finish)

    print '("Sum = ",f15.6,".")', sum
    print '("Time = ",f6.3," seconds.")',finish-start
end program test_cpu_time
