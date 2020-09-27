#!/bin/bash
#SBATCH --ntasks=256

export OMPI_MCA_btl=self,tcp
mpirun ./a.out
ffmpeg -pattern_type glob -framerate 30 -i "output/*.ppm" output.mp4

