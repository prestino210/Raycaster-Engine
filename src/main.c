#include <stdbool.h>
#include <stdint.h>
#include "globals.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "camera.h"
#include "raycaster.h"
#define WINDOW_TITLE "Ray Caster"
#define REF_RATE 120
#define COL_WIDTH 1
#define MAX_SHADE_M 150
#define FG_R 200
#define FG_G 200
#define FG_B 200
#define BG_R 150
#define BG_G 150
#define BG_B 255
#define BG2_R 40
#define BG2_G 90
#define BG2_B 40
#define MAP_FILE "../../map.txt"

const double CONV_RAD = M_PI/180;
const double CONV_DEG = 180/M_PI;
double delta_time = 0;

SDL_Window* rc_window = NULL;
SDL_Renderer* rc_renderer = NULL;
const unsigned int NUM_COLS = SCREEN_WIDTH / COL_WIDTH;
const float MAX_SHADE = MAX_SHADE_M * 100;
float* cols; 
Camera camera;

void init_sdl(void) {
    // Init SDL window
    // Init a renderer attached to the window
    SDL_Init(SDL_INIT_EVERYTHING);
    rc_window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    rc_renderer = SDL_CreateRenderer(rc_window, -1, 0);
    SDL_SetRenderDrawBlendMode(rc_renderer, SDL_BLENDMODE_BLEND);
}


void draw_crosshair(void) {
    SDL_SetRenderDrawColor(rc_renderer, 255, 255, 255, 255);
    SDL_Rect ch_x = {SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 - 2.5, 20, 5};
    SDL_Rect ch_y = {SCREEN_WIDTH/2 - 2.5, SCREEN_HEIGHT/2 - 10, 5, 20};
    SDL_RenderFillRect(rc_renderer, &ch_x);
    SDL_RenderFillRect(rc_renderer, &ch_y);
    SDL_SetRenderDrawColor(rc_renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(rc_renderer, &ch_x);
    SDL_RenderDrawRect(rc_renderer, &ch_y);
}

void draw(void) {
    SDL_SetRenderDrawColor(rc_renderer, BG_R, BG_G, BG_B, 255);
    SDL_RenderClear(rc_renderer);
    
    for(int i = 0; i < NUM_COLS; i++) {
        float alpha = 255 - (MAX_SHADE / cols[i]);
        if(alpha < 0) {
            alpha = 0;
        } else if(alpha > 255) {
            alpha = 255;
        }
        const SDL_Rect col = {i * COL_WIDTH, 0.5*(SCREEN_HEIGHT - cols[i]),
            COL_WIDTH, cols[i]}; 
        const SDL_Rect floor_col = {i * COL_WIDTH, cols[i] + (SCREEN_HEIGHT - cols[i])/2,
            COL_WIDTH, (SCREEN_HEIGHT - cols[i])/2}; 
        SDL_SetRenderDrawColor(rc_renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(rc_renderer, &col);
        SDL_SetRenderDrawColor(rc_renderer, FG_R, FG_G,
            FG_B, alpha);
        SDL_RenderFillRect(rc_renderer, &col);
        SDL_SetRenderDrawColor(rc_renderer, BG2_R, BG2_G,
            BG2_B, 255);
        SDL_RenderFillRect(rc_renderer, &floor_col);
    }

    draw_crosshair();
    
    SDL_RenderPresent(rc_renderer);
}



bool update(void) {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        switch(e.type) {
            case SDL_QUIT:
                return false;
        }
    }

    handle_input(&camera);
    cast_rays(camera);
    return true;
}

int main(void) {
    load_map(MAP_FILE);
    cols = malloc(NUM_COLS * sizeof(float));

    camera = create_camera();
    init_sdl();
 
    double ref_delay_hz = 1000 / REF_RATE;
    Uint64 now, last = SDL_GetPerformanceCounter();
    bool running = true;
    while(running) {
       
        now = SDL_GetPerformanceCounter();
        delta_time = (double) (now - last) / SDL_GetPerformanceFrequency();
        last = now;

        if(!update()) break;
        draw();
       
        double frame_time = (double) (SDL_GetPerformanceCounter() - now) / SDL_GetPerformanceFrequency() * 1000.0;
        if(frame_time < ref_delay_hz) SDL_Delay((Uint8) (ref_delay_hz - frame_time));
    }
    destruct(EXIT_SUCCESS, NULL);
}