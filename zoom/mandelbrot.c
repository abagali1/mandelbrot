#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define X 1920
#define Y 1080

#define MAX_ITER 8000
#define FRAMES 250

typedef struct {
    int r;
    int g;
    int b;
} Color;

typedef struct {
    double r_max;
    double r_min;
    double i_max;
    double i_min;
} Bounds;

double lerp(double v0, double v1, double t) {
    return (1 - t) * v0 + t * v1;
}

const Bounds initial = (Bounds){.r_max=1.5, .r_min=-1.5, .i_max=1.0, .i_min=-1.0};
const Bounds final = (Bounds){.r_max=-0.738850882975, .r_min=-0.738871642261, .i_max=-0.164141980147, .i_min=-0.164152164232};


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


int main(){
    Color** colors = (Color**)malloc(sizeof(Color*)*Y);
    for(int y = 0;y < Y;y++){
        colors[y] = (Color*)malloc(sizeof(Color)*X);
    }
    Color* palette = (Color*)malloc(sizeof(Color)*MAX_ITER+1);
    printf("made arrays\n");
    for(int i=0;i<MAX_ITER+1;i++){
        if (i >= MAX_ITER){
            palette[i] = (Color){.r=0,.g=0,.b=0};
            continue;
        }
        double j;
        if(i == 0){
            j = 3.0;
        }else{
            j = 3.0 * (log(i)/log(MAX_ITER-1.0));
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
    printf("finished palette\n");

    for(int f = 0;f <= FRAMES;f++){
        double t = f*1.0/FRAMES*1.0;
        Bounds bounds = (Bounds){
            .r_max=lerp(initial.r_max, final.r_max, t),
            .r_min=lerp(initial.r_min, final.r_min, t),
            .i_max=lerp(initial.i_max, final.i_max, t),
            .i_min=lerp(initial.i_min, final.i_min, t),
        };
	    #pragma omp parallel for
	    for(int Py = 0; Py < Y; Py++){
	        for(int Px = 0; Px < X; Px++){
	            colors[Py][Px] = mandelbrot(Px, Py, palette, bounds);
	        }
	    }
	    printf("finished calc %02d\n", f);
	    FILE* fout;
        char buf[14];
        snprintf(buf, sizeof(buf), "output/%02d.ppm", f);
	    fout = fopen(buf, "w");
	    fprintf(fout, "P3\n");
	    fprintf(fout, "%d %d\n", X, Y);
	    fprintf(fout, "255\n");
	    for(int y = 0; y < Y; y++){
	        for(int x = 0; x < X; x++){
	            fprintf(fout, "%ld %ld %ld\n", (int)colors[y][x].r, (int)colors[y][x].g, (int)colors[y][x].b);
	        }
	    }
    }
}
