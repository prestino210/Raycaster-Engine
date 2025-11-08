#include "globals.h"
#include <stdlib.h>
#include <stdio.h>
#include "raycaster.h"

void destruct(int exit_code, char* error) {
    if(exit_code == EXIT_FAILURE) printf("%s\n", error);
    SDL_DestroyWindow(rc_window);
    SDL_DestroyRenderer(rc_renderer);
    SDL_Quit();
    free(cols);
    for(int i = 0; i < map_height; i++) {
        free(map[i]);
    }
    free(map);
    exit(exit_code);
}