#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define X 1920
#define Y 1080

#define R_MAX 1.5
#define R_MIN -2
#define I_MAX 1.0
#define I_MIN -I_MAX

#define MAX_ITER 800

typedef struct {
    long int r;
    long int g;
    long int b;
} Color;

static inline double lerp(double v0, double v1, double t) {
    return (1 - t) * v0 + t * v1;
}

Color* make_palette(int size);


Color mandelbrot(int px, int py, Color* palette){
    double x = 0; // complex (c)
    double y = 0;

    double x0 = R_MIN + (px * ((R_MAX - R_MIN)/(X*1.0))); // complex scale of Px
    double y0 = I_MIN + (py * ((I_MAX - I_MIN)/(Y*1.0))); // complex scale of Py

    double i = 0;
    double x2 = 0;
    double y2 = 0;

    while(x*x + y*y <= 20 && i < MAX_ITER){
        y = 2*x*y + y0;
        x = x2 - y2 + x0;
        x2 = x*x;
        y2 = y*y;
        i++;
    }

    if(i < MAX_ITER){
        double log_zn = log(x*x + y*y) / 2.0;
        double nu = log(log_zn / log(2.0))/log(2.0);
        i += 1.0 - nu;
    }
    Color c1 = palette[(int)i];
    Color c2;
    if((int)i + 1 > MAX_ITER){
        c2 = palette[(int)i];
    }else{
        c2 = palette[((int)i)+1];
    }

    double mod = i - ((int)i) ; // cant mod doubles
    return (Color){
            .r = (int)lerp(c1.r, c2.r, mod),
            .g = (int)lerp(c1.g, c2.g, mod),
            .b = (int)lerp(c1.b, c2.b, mod),
    };

}

void master(int workers, Color* palette){
    MPI_Status status;

    Color** colors = (Color**)malloc(sizeof(Color*)*Y);
    for(int y = 0;y < Y;y++){
        colors[y] = (Color*)malloc(sizeof(Color)*X);
    }

    int size = Y/(workers-1);
    Color* recv = (Color*)malloc(sizeof(Color)*size*X);
    printf("init master\n");
    for(int i=0;i<(workers-1);i++){
        MPI_Recv(recv, sizeof(Color)*size*X, MPI_CHAR, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
        int source = status.MPI_SOURCE - 1;
        printf("Received from %d\n", source);
        for(int x =0;x<size;x++){
            for(int y = 0;y<X;y++){
                colors[source * size + x][y] = recv[y*size+x];
            }
        }
    }

    FILE* fout;
    fout = fopen("output/ms.ppm", "w");
    fprintf(fout, "P3\n");
    fprintf(fout, "%d %d\n", X, Y);
    fprintf(fout, "255\n");
    for(int y = 0; y < Y; y++){
        for(int x = 0; x < X; x++){
            fprintf(fout, "%ld %ld %ld\n", colors[y][x].r, colors[y][x].g, colors[y][x].b);
        }
    }
}

void slave(int workers, int rank, Color* palette){
    int size = Y / (workers-1);
    int ssize = sizeof(Color) * size * X;
    Color* buf = (Color*)malloc(ssize);
    for(int y=0;y<size;y++){
        for(int x=0;x<X;x++){
            int j = x * size + y;
            buf[x*size + y] = mandelbrot(x, ((rank-1)*size) + y, palette, bounds);
        }
    }
    MPI_Send(buf, ssize, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
    free(buf);
}


int main(int argc, char* argv[]){

    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size( MPI_COMM_WORLD, &size);
    MPI_Comm_rank( MPI_COMM_WORLD, &rank);
    
    Color* palette = make_palette(MAX_ITER);

    if(rank == 0){
        master(size, palette);
    }else{
        slave(size, rank, palette);
    }

    MPI_Finalize();
    return 0;
}


Color* make_palette(int size){
    Color* palette = (Color*)malloc(sizeof(Color)*(size+1));
        for(int i=0;i<size+1;i++){
        if (i >= size){
            palette[i] = (Color){.r=0,.g=0,.b=0};
            continue;
        }
        double j;
        if(i == 0){
            j = 3.0;
        }else{
            j = 3.0 * (log(i)/log(size-1.0));
        }

        if (j<1){
            palette[i] = (Color){
                    .r = 0,
                    .g = 255 * j,
                    .b = 0
            };
        }else if(j<2){
            palette[i] = (Color){
                    .r = 255*(j-1),
                    .g = 255,
                    .b = 0,
            };
        }else{
            palette[i] = (Color){
                    .r = 255 * (j-2),
                    .g = 255,
                    .b = 255,
            };
        }
    }
    return palette;
}

