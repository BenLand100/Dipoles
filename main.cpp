#include <iostream>
#include <cmath>
#include "SDL/SDL.h"
#include "rng.h"

#define Ke 1

#define fatal(why) { cout << "FATAL ERROR: " << why << '\n'; exit(1); }
#define debug(what) { cout << "DEBUG: " << what << '\n'; }

typedef struct {
    double x,y;
} vec;

typedef struct {
    double cx, cy, theta;
    double vx, vy, alpha;
    double mag;
    double size;
} dipole;

using namespace std;

int width, height, bpp;
int count;
dipole *dips;
vec *field, *mom;

void draw_fill(SDL_Surface *surface, Uint32 color) {
    for (int i = 0; i < width*height; i++) {
        ((Uint32*)surface->pixels)[i] = color;
    }
}

void draw_dipole(SDL_Surface *surface, dipole *dip) {
    Uint32 *pix = &((Uint32*)surface->pixels)[(int)(0.5+dip->cx) + (int)(0.5+height-dip->cy)*width];
    //Uint32 color = 0xFFFFFF;//(int)fmod(dip->theta,2*M_PI)/(2*M_PI)*0xFFFFFF;
    double costheta = cos(dip->theta);
    double sintheta = sin(dip->theta);
    for (double r = -5; r <= -0.5; r += 0.5) {
        pix[(int)(0.5+costheta*r)+(int)(0.5+sintheta*r)*width] = 0xFF0000;
    }
    for (double r = 0.5; r <= 5; r += 0.5) {
        pix[(int)(0.5+costheta*r)+(int)(0.5+sintheta*r)*width] = 0xFFFF;
    }
}

void render(SDL_Surface *surface) {
    draw_fill(surface,0);
    for (int i = 0; i < count; i++) {
        draw_dipole(surface,&dips[i]);
    }
}

void init(int num) {
    init_genrand(time(0));
    count = num;
    field = new vec[count];
    mom = new vec[count];
    dips = new dipole[count];
    for (int i = 0; i < count; i++) {
        dips[i].cx = genrand_res53()*width;
        dips[i].cy = genrand_res53()*height;
        dips[i].vx = dips[i].vy = 0;
        dips[i].alpha = 0;
        dips[i].mag = 1;
        dips[i].theta = genrand_res53()*2*M_PI;
        dips[i].size = 50;
    }
    debug("Dipoles initilized");
}

void step(double dt) {
    //debug("Stepping dynamics");
    for (int i = 0; i < count; i++) {
        field[i].x = 0;
        field[i].y = 0;
        mom[i].x = dips[i].mag*cos(dips[i].theta);
        mom[i].y = dips[i].mag*sin(dips[i].theta);;
    }
    for (int i = 0; i < count; i++) {
        field[i].x = 0;
        field[i].y = 0;
        double cx = dips[i].cx, cy = dips[i].cy;
        for (int j = 0; j < count; j++) {
            if (i == j) continue;
            double ux = cx - dips[j].cx; 
            double uy = cy - dips[j].cy;
            double mag = sqrt(ux*ux+uy*uy);
            ux /= mag;
            uy /= mag;
            
            double pdotrhat = ux*mom[j].x+uy*mom[j].y;
            field[i].x += Ke/mag/mag/mag*(3.0*pdotrhat*ux-mom[j].x);
            field[i].y += Ke/mag/mag/mag*(3.0*pdotrhat*uy-mom[j].y);
        }
        double torque = mom[i].x*field[i].y-mom[i].y*field[i].x;
        dips[i].alpha += torque * dt;
        dips[i].theta += dips[i].alpha * dt;
        dips[i].cx += dips[i].vx * dt;
        dips[i].cy += dips[i].vy * dt; 
    }
}

int main(int argc, char **argv) {
    debug("Dipoles by BenLand100");
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) fatal("Could not init SDL");
    
    width = 640;
    height = 480;
    bpp = 32;
    SDL_Surface* screen = SDL_SetVideoMode(width, height, bpp, SDL_SWSURFACE);
    if (!screen) fatal("Could not create window\n");
    SDL_WM_SetCaption("Dipoles by BenLand100", NULL);
    
    init(100);
    
    SDL_Event event;
    bool running = true;
    debug("Entering main loop");
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                debug("Goodbye");
                running = false;
            } 
        }
        //SDL_Delay(10);
        step(1);
        render(screen);
        SDL_Flip(screen);
    }
    
    SDL_Quit();
    return 0;
}
