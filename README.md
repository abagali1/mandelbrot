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

OpenMPI:
```bash
mpicc parallel/mandelbrot_openmpi.c -o mandelbrot -lm
mpirun -np $(nproc) -mca btl ^openib mandelbrot
```
or with Slurm
```bash
mpicc parallel/mandelbrot_openmpi.c -o mandelbrot -lm
salloc -n {cores} mpiexec mandelbrot
```

![mandelbrot](https://github.com/abagali1/mandelbrot/raw/master/output/readme.png)

## Iterations
looks nice

```bash
mpicc iters/mandelbrot.c -o mandelbrot -lm
mpirun -np $(nproc) -mca btl ^openib mandelbrot
```

or with Slurm
```bash
mpicc iters/mandelbrot.c -lm
sbatch iters/iter.sh
```

![mandelbrot](https://github.com/abagali1/mandelbrot/blob/master/iters/output.gif)

## Zoom (WIP)
hands down the coolest part

```bash
mpicc zoom/mandelbrot.c -o mandelbrot -lm
mpirun -np $(nproc) -mca btl ^openib mandelbort
```
or with Slurm
```bash
mpicc zoom/mandelbrot.c -o zoom/a.out -lm
sbatch zoom/zoom.sh
```

![mandelbrot-zoom](https://github.com/abagali1/mandelbrot/blob/master/zoom/zoom.gif)

