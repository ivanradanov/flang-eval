#!/usr/bin/env bash

set -e

NRUNS=3

. enable.sh /l/ssd/ivanov2/flang-release/install/
module load rocm

for f in matmul/*.f90 axpy/*.f90 synthetic-sqrt/*.f90; do
    if [[ "$(echo -n "$f" | tail -c 7)" != 'tmp.f90' ]]; then
        echo "$f"
        tmpf="$f.tmp.f90"
        cat "$f" | sed 's/AXPY_SIZE/20480/' | sed 's/MATMUL_SIZE/4048/' | sed 's/SQRT_SIZE/20480/' > "$tmpf"
        flang-new -g -O2 -fopenmp --offload-arch=native \
            "$tmpf" \
            -o a.out \
            -L/l/ssd/ivanov2/flang-release/install/lib/ -lFortranOmpRuntime -L/opt/rocm-6.0.2/lib -lamdhip64 -lrocblas \
            #&> /dev/null
        for i in $(seq 1 "$NRUNS"); do
            ./a.out 2> /dev/null | grep Time
        done
    fi
done
