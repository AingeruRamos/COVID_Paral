#!/bin/bash

display_usage() {

    echo 'Uso: ./run programa nprocs (0 para todos)'
    exit 1

}

if [[ $# -lt 2 ]]; then
        display_usage
fi

if [[ $2 -eq 0 ]]; then
        cmd="$1"
        make compil
else
    	cmd="-np $2 $1"
        make compil
fi

mpirun --mca btl openib,vader,self,tcp --mca mpi_cuda_support 0 -machinefile /export/home/nodos/hostfinal --map-by node --bind-to none $cmd



