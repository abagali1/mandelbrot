# mandelbrot
Should've done this a long time ago but oh well


![equation](https://render.githubusercontent.com/render/math?math=z_{n%2B1}%20-%3E%20z_n^2%20%2B%20c&mode=inline)


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
mpicc iters/mandelbrot.c -o iters/a.out -lm -Ofast 
sbatch iters/iter.sh
```

![mandelbrot](https://github.com/abagali1/mandelbrot/blob/master/iters/output.gif)

## Multibrot
variation of conventional mandelbrot equation
![equation](https://render.githubusercontent.com/render/math?math=z_{n%2B1}%20-%3E%20z_n^x%20%2B%20c&mode=inline)
where X varies.

in `multibrot/multibrot.c` set `MIN_POWER` and `MAX_POWER` to set range for X 

```bash
mpicc multibrot/multibrot.c -o multibrot -lm -Ofast
mpirun -np $(nproc) -mca btl ^openib multibrot
```

or with Slurm
```bash
mpicc multibrot/multibrot.c -o multibrot/a.out -lm -Ofast
sbatch multibrot/multibrot.sh
```

![multibrot](https://imgur.com/a/fSdZdlA)

## Zoom (WIP)
hands down the coolest part

```bash
mpicc zoom/mandelbrot.c -o mandelbrot -lm
mpirun -np $(nproc) -mca btl ^openib mandelbort
```
or with Slurm
```bash
mpicc zoom/mandelbrot.c -o zoom/a.out -lm -Ofast
sbatch zoom/zoom.sh
```

![mandelbrot-zoom](https://github.com/abagali1/mandelbrot/blob/master/zoom/zoom.gif)

