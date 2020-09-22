// https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set#Continuous_(smooth)_coloring

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define X 1920
#define Y 1080

#define R_MAX 2.0
#define R_MIN -R_MAX
#define I_MAX 1.5
#define I_MIN -I_MAX

#define MAX_ITER 8000

typedef struct {
    int r;
    int g;
    int b;
} rgb;


int mandelbrot(int px, int py){
    double x = 0; // complex (c)
    double y = 0;

    double x0 = R_MIN + (px * ((R_MAX - R_MIN)/(X*1.0))); // complex scale of Px
    double y0 = I_MIN + (py * ((I_MAX - I_MIN)/(Y*1.0))); // complex scale of Py

    int i = 0;
  
    while(x*x + y*y <= 20 && i < MAX_ITER){
        double xtemp = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = xtemp;
        i++;
    }
    return i;
    /* WIP big brain log coloring 
    if(i < MAX_ITER){
        log_zn = log(x*x + y*y) / 2;
        nu = log(log_zn / log(2))/log(2);
    }
    */
}


int main(){
    rgb** colors = (rgb**)malloc(sizeof(rgb*)*Y);
    for(int y = 0;y < Y;y++){
        colors[y] = (rgb*)malloc(sizeof(rgb)*X);
    }
    rgb* palatte = (rgb*)malloc(sizeof(rgb)*MAX_ITER+1);
    printf("made arrays\n");
    for(int i=0;i<MAX_ITER+1;i++){
        if (i >= MAX_ITER){
            palatte[i] = (rgb){.r=0,.g=0,.b=0};
            continue;
        }
        double j = 3.0 * (i == 0 ? 0:log(i))/log(MAX_ITER-1.0);

        if (j<1){
            palatte[i] = (rgb){
                .r = 255 * j,
                .g = 0,
                .b = 0
            };
        }else if(j<2){
            palatte[i] = (rgb){
                .r = 255,
                .g = 255*(j-1),
                .b = 0,
            };
        }else{
            palatte[i] = (rgb){
                .r = 255,
                .g = 255,
                .b = 255 * (j-2),
            };
        }
    }


    printf("finished palatte\n");
    for(int Py = 0; Py < Y; Py++){
        for(int Px = 0; Px < X; Px++){
            int i = mandelbrot(Px, Py);
            colors[Py][Px] = palatte[i];
        }
    }
    printf("finished calcs\n");
    FILE* fout;
    fout = fopen("ms.ppm", "w");
    fprintf(fout, "P3\n");
    fprintf(fout, "%d %d\n", X, Y);
    fprintf(fout, "255\n");
    for(int y = 0; y < Y; y++){
        for(int x = 0; x < X; x++){
            fprintf(fout, "%d %d %d\n", colors[y][x].r, colors[y][x].g, colors[y][x].b);
        }
    }
}
