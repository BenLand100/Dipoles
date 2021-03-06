#include <iostream>
#include <cmath>
#include "SDL/SDL.h"
#include "rng.h"

//Electric constants
#define Ke 1

#define fatal(why) { cout << "FATAL ERROR: " << why << '\n'; exit(1); }
#define debug(what) { cout << "DEBUG: " << what << '\n'; }

typedef struct {
    double x,y;
} vec;

typedef struct {
    double cx, cy, theta; //positions
    double vx, vy, omega; //velocities
    double mag; //dipole moment
    double mass; //inertial mass
    double rotinertia; //rotational inertia
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
    for (double r = -dip->size/2.0; r <= -0.5; r += 0.5) {
        pix[(int)(0.5+costheta*r)+(int)(0.5+sintheta*r)*width] = 0xFF0000;
    }
    for (double r = 0.5; r <= dip->size/2.0; r += 0.5) {
        pix[(int)(0.5+costheta*r)+(int)(0.5+sintheta*r)*width] = 0xFFFF;
    }
}

void render(SDL_Surface *surface) {
    draw_fill(surface,0);
    for (int i = 0; i < count; i++) {
        draw_dipole(surface,&dips[i]);
    }
}

void init_crystal() {
    init_genrand(time(0));
    int numperside = 20;
    count = (int)round(numperside*numperside);
    debug("Using " << count << " dipoles");
    field = new vec[count];
    mom = new vec[count];
    dips = new dipole[count];
    for (int i = 0; i < count; i++) {
        dips[i].cx = (i%numperside)/(double)(numperside-1)*(width-80)+40 + ((i/numperside)%2 ? 1.0/(double)(numperside-1)*(width-80)/2.0 : 0);
        dips[i].cy = (i/numperside)/(double)(numperside-1)*(height-80)+40;
        debug(dips[i].cx << ' ' << dips[i].cy);
        dips[i].vx = dips[i].vy = 0;
        dips[i].omega = 0;
        dips[i].mass = 1;
        dips[i].rotinertia = 1;
        dips[i].mag = 20;
        //dips[i].theta = (i/20) % 2 ? M_PI/4.0 : 3.0*M_PI/4.0;
        //dips[i].theta = genrand_res53()*2*M_PI;
        dips[i].theta = genrand_res53()*0.0-0.0+M_PI/2.0;
        dips[i].size = 20;
    }
    debug("Dipoles initilized");
}

void init_grid() {
    init_genrand(time(0));
    int numperside = 20;
    count = (int)round(numperside*numperside);
    debug("Using " << count << " dipoles");
    field = new vec[count];
    mom = new vec[count];
    dips = new dipole[count];
    for (int i = 0; i < count; i++) {
        dips[i].cx = (i%numperside)/(double)(numperside-1)*(width-40)+20;
        dips[i].cy = (i/numperside)/(double)(numperside-1)*(height-40)+20;
        debug(dips[i].cx << ' ' << dips[i].cy);
        dips[i].vx = dips[i].vy = 0;
        dips[i].omega = 0;
        dips[i].mass = 1;
        dips[i].rotinertia = 1;
        dips[i].mag = 20;
        dips[i].theta = (i/20) % 2 ? M_PI/4.0 : 3.0*M_PI/4.0;
        //dips[i].theta = genrand_res53()*2*M_PI;
        //dips[i].theta = genrand_res53()*4.0-2.0+M_PI/2.0;
        dips[i].size = 20;
    }
    debug("Dipoles initilized");
}

void init_rand(int num) {
    init_genrand(time(0));
    count = num;
    field = new vec[count];
    mom = new vec[count];
    dips = new dipole[count];
    for (int i = 0; i < count; i++) {
        dips[i].cx = genrand_res53()*(width-20)+10;
        dips[i].cy = genrand_res53()*(height-20)+10;
        dips[i].vx = dips[i].vy = 0;
        dips[i].omega = 0;
        dips[i].mass = 1;
        dips[i].rotinertia = 1;
        dips[i].mag = 1;
        dips[i].theta = genrand_res53()*2*M_PI;
        dips[i].size = 10;
    }
    debug("Dipoles initilized");
}

void stats() {
	double kinetic = 0;
	double rotational = 0;
	double potential = 0;
    for (int i = 0; i < count; i++) {
    	kinetic += 0.5*dips[i].mass*(dips[i].vx*dips[i].vx + dips[i].vy*dips[i].vy);
    	rotational += 0.5*dips[i].rotinertia*(dips[i].omega*dips[i].omega);
    	potential -= dips[i].mag*(field[i].x*cos(dips[i].theta)+field[i].y*sin(dips[i].theta));
    }
    cout << "Energy (theoretical units)\nK: " << kinetic << "\nR: " << rotational << "\nU: " << potential << "\nE: " << (kinetic+rotational+potential) << '\n';
}

void step(double dt, double damping) {
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
        dips[i].omega += torque/dips[i].rotinertia*dt;
        dips[i].omega -= dips[i].omega*damping*dt;
        dips[i].theta += fmod(dips[i].omega*dt,2.0*M_PI);
        dips[i].vx -= dips[i].vx*damping*dt;
        dips[i].vy -= dips[i].vy*damping*dt;
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
    
    init_crystal();
    
    int i = 0;
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
        step(0.25,0.0001);
        if (!(i++ % 100)) stats();
        render(screen);
        SDL_Flip(screen);
    }
    
    SDL_Quit();
    return 0;
}
