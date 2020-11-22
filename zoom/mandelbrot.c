#include <mpi.h>
#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define uchar unsigned char
#define X 1920
#define Y 1080
//#define R 1.5 // aspect ratio

#define MAX_ITER 8000
#define FRAMES 50 

typedef struct {
    long int r;
    long int g;
    long int b;
} Color;

typedef struct {
    double r_max;
    double r_min;
    double i_max;
    double i_min;
} Bounds;

static inline double lerp(double v0, double v1, double t) {
    return (1 - t) * v0 + t * v1;
}

static inline double safe_sqrt(double x){
    return x > 0 ? sqrt(x) : -sqrt(-x);
}

Color* make_palette(int size);

const Bounds initial = (Bounds){.r_max=1.5, .r_min=-1.5, .i_max=1.0, .i_min=-1.0};
const Bounds final = (Bounds){.r_max=0.738850882975, .r_min=-0.738850882975, .i_max=0.164141980147, .i_min=-0.164141980147};


Color mandelbrot(int px, int py, Color* palette, Bounds b){
    double x = 0; // complex (c)
    double y = 0;

    double x0 = b.r_min + (px * ((b.r_max - b.r_min)/(X*1.0))); // complex scale of Px
    double y0 = b.i_min + (py * ((b.i_max - b.i_min)/(Y*1.0))); // complex scale of Py

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

void master(int workers, Color* palette, double frame){
    MPI_Status status;
    
    for(int i=1;i<workers;i++){
        MPI_Request req; // non-blocking send for faster broadcast
        MPI_Isend(&frame, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &req);
        MPI_Request_free(&req);
    }
    
    uchar (*colors)[X][3] = malloc(sizeof(uchar[Y][X][3])); //uchar arrays are faster to write
    
    int size = Y/(workers-1);
    Color* recv = (Color*)malloc(sizeof(Color)*size*X);
    for(int i=0;i<(workers-1);i++){
        MPI_Recv(recv, sizeof(Color)*size*X, MPI_CHAR, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
        int source = status.MPI_SOURCE - 1;
        for(int x =0;x<size;x++){
            for(int y = 0;y<X;y++){
                int i = source * size + x;
                Color c = recv[y*size+x];
                colors[i][y][0] = c.r;
                colors[i][y][1] = c.g;
                colors[i][y][2] = c.b;
            }
        }
    }

    FILE* fout;
    char buf[17];
    snprintf(buf, sizeof(buf), "output/%05d.ppm", (int)frame);
    fout = fopen(buf, "w");
    fprintf(fout, "P6\n%d %d\n255\n", X, Y);//P6 to use fwrite
    for(int y = 0; y < Y; y++){
        for(int x = 0; x < X; x++){
            fwrite(colors[y][x], 1, 3, fout);
        }
    }
    fclose(fout);
    printf("Finished %d\n", (int)frame);
    free(colors);
}

void slave(int workers, int rank, Color* palette){
    double frame;
    
    MPI_Recv(&frame, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    double t = frame/FRAMES*1.0;
    
    Bounds bounds = (Bounds){
        .r_max=exp(lerp(log(initial.r_max), log(final.r_max), t)),
        .r_min=exp(lerp(log(initial.r_min), log(final.r_min), t)),
        .i_max=exp(lerp(log(initial.i_max), log(final.i_max), t)),
        .i_min=exp(lerp(log(initial.i_min), log(final.i_min), t)),
    };
    
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

    for(int f=0;f <= FRAMES; f++){
        if(rank == 0){
            master(size, palette, (double)f);
        }else{
            slave(size, rank, palette);
        }
    }
    free(palette);
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



