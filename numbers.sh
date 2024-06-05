#!/usr/bin/env bash

set -e

NRUNS=1

. enable.sh /l/ssd/ivanov2/flang-release/install/
module load rocm

for f in matmul/*.f90 synthetic-sqrt/*.f90; do
    echo $f
    flang-new -g -O2 -fopenmp --offload-arch=native $f -o a.out \
        -L/l/ssd/ivanov2/flang-release/install/lib/ -lFortranOmpRuntime -L/opt/rocm-6.0.2/lib -lamdhip64 -lrocblas &> /dev/null
    for i in $(seq 1 "$NRUNS"); do
        ./a.out 2> /dev/null | grep Time
    done
done
