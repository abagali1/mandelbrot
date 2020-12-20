#!/bin/bash
#SBATCH --ntasks=128

export OMPI_MCA_btl=self,tcp
mpirun ./a.out
ffmpeg -pattern_type glob -framerate 25 -i "output/*.ppm" output.gif

