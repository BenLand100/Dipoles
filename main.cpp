#include <iostream>
#include "SDL/SDL.h"

#define fatal(why) { cout << "FATAL ERROR: " << why << '\n'; exit(1); }
#define debug(what) { cout << "DEBUG: " << what << '\n'; }

using namespace std;

bool running;

int main(int argc, char **argv) {
    debug("Dipoles by BenLand100");
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) fatal("Could not init SDL");
    
    SDL_Surface* screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
    if (!screen) fatal("Could not create window\n");
    SDL_WM_SetCaption("Dipoles by BenLand100", NULL);
    
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
    }
    
    SDL_Quit();
    return 0;
}
