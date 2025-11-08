#ifndef GLOBALS_H
#define GLOABALS_H
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1000
#define HEIGHT_FACTOR 200 
#include <SDL2/SDL.h>
extern const double CONV_RAD;
extern const double CONV_DEG;
extern const unsigned int NUM_COLS;
extern double delta_time;
extern float* cols;
extern SDL_Window* rc_window;
extern SDL_Renderer* rc_renderer;
void destruct(int exit_code, char* error);
#endif