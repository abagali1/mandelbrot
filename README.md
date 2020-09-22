# mandelbrot

Should've done this a long time ago but oh well


All ppms are stored at `output/ms.ppm`

Regular:
```bash
gcc mandelbrot.c -o mandelbrot -lm
./mandelbrot
```

OpenMP:
```bash
gcc parallel/mandelbrot_openmp.c -o mandelbrot -fopenmp -lm
./mandelbrot
```

OpenMPI(WIP):
```bash
mpicc parallel/mandelbrot_openmpi.c -o mandelbrot -lm
mpirun -np $(nproc) -mca btl ^openib mandelbrot
```

Scripts in `slurm/` _should_ work with `sbatch` on a Slurm cluster but Slurm can be bad lol

![mandelbrot](https://github.com/abagali1/mandelbrot/raw/master/output/readme.png)
