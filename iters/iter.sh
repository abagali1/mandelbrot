#!/bin/bash
#SBATCH --ntasks=64

export OMPI_MCA_btl=self,tcp
mpirun ./a.out
ffmpeg -pattern_type glob -framerate 15 -i "output/*.ppm" output.mp4

