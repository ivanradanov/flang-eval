#!/usr/bin/env bash

set -e

NRUNS=1

if (hostname | grep memkf01) ; then
    . enable.sh /scr/ivan/opt/flang.release/install/
    LIBDIR=/scr/ivan/opt/flang.release/install/lib/
    ROCMDIR=/opt/rocm-6.2.2/
else
    . enable.sh /l/ssd/ivanov2/flang-release/install/
    module load rocm
    LIBDIR=/l/ssd/ivanov2/flang-release/install/lib/
    ROCMDIR=/opt/rocm-6.0.2/
fi

export LD_LIBRARY_PATH="$LIBDIR:$ROCMDIR/lib:$LD_LIBRARY_PATH"

function do_run() {
    f=$1
    if [[ "$(echo -n "$f" | tail -c 7)" != 'tmp.f90' ]]; then
        echo "$f"
        tmpf="$f.tmp.f90"
        cat "$f" | sed 's/AXPY_SIZE/20480/g' | sed 's/MATMUL_SIZE/4096/' | sed 's/SQRT_SIZE/20480/g' > "$tmpf"
        flang-new -g -O2 -fopenmp --offload-arch=native \
            "$tmpf" \
            -o a.out \
            -L"$LIBDIR" -lFortranOmpRuntime -L"$ROCMDIR"/lib -lamdhip64 -lrocblas \
            #&> /dev/null
        for i in $(seq 1 "$NRUNS"); do
            ./a.out 2> /dev/null | grep Time
        done
    fi
}

echo HLFIR
for f in axpy/omp-*.f90; do
    do_run $f
done
