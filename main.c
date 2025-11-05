#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define WINDOW_TITLE "Ray Caster"
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1000
#define REF_RATE 120
#define CAMERA_FOV 70 
#define COL_WIDTH 1
#define HEIGHT_FACTOR 200 
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
#define MAX_VEL 1.5
#define ROT_SPEED 130
#define SPAWN_X 0
#define SPAWN_Y 0
#define MAP_FILE "../../map.txt"

typedef struct {
    float rotation; 
    float fov;
    float x, y; 
    float max_vel; 
    float rot_speed; 
} Camera;

const double CONV_RAD = M_PI/180;
const double CONV_DEG = 180/M_PI;
unsigned int map_width = 0;
unsigned int map_height = 0;
double delta_time = 0;
SDL_Event e;
SDL_Window* rc_window = NULL;
SDL_Renderer* rc_renderer = NULL;
const unsigned int NUM_COLS = SCREEN_WIDTH / COL_WIDTH;
const float MAX_SHADE = MAX_SHADE_M * 100;
float* cols; 
int** map = NULL;
Camera camera;

void load_map(void) {
    FILE* file = fopen(MAP_FILE, "r");

    char* line = NULL;
    size_t len = 0;
    ssize_t nread;

    unsigned int i = 0;
    while((nread = getline(&line, &len, file)) != -1) {
       
        if(line[nread - 1] == '\n') nread--;

        if(i == 0) map_width = nread;

        int* row = malloc((nread) * sizeof(int));

        for(int j = 0; j < nread; j++) {
            row[j] = (int) line[j] - '0';
        }
        map = realloc(map, (i *  (map_width * sizeof(int))));
        map[i] = row;
       
        i++;
    }

    map_height = i;

    free(line);
    fclose(file);
}

void init_sdl(void) {
    // Init SDL window
    // Init a renderer attached to the window
    SDL_Init(SDL_INIT_EVERYTHING);
    rc_window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    rc_renderer = SDL_CreateRenderer(rc_window, -1, 0);
    SDL_SetRenderDrawBlendMode(rc_renderer, SDL_BLENDMODE_BLEND);
}

void handle_input(void) {
    const Uint8* keyboard = SDL_GetKeyboardState(NULL);

    float moveX = 0;
    float moveY = 0;

    if(keyboard[SDL_SCANCODE_W]) {
        moveX += cosf(camera.rotation);
        moveY += sinf(camera.rotation);
    }
    if(keyboard[SDL_SCANCODE_S]) {
        moveX -= cosf(camera.rotation);
        moveY -= sinf(camera.rotation);
    }
    if (keyboard[SDL_SCANCODE_A]) {
        camera.rotation -= camera.rot_speed * delta_time;
    }
    if (keyboard[SDL_SCANCODE_D]) { 
        camera.rotation += camera.rot_speed * delta_time;
    }

    float length = sqrtf(moveX*moveX + moveY*moveY);
    if(length != 0.0f) {
        moveX /= length;
        moveY /= length;
    }

    camera.x += moveX * camera.max_vel * delta_time;
    camera.y += moveY * camera.max_vel * delta_time;
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

void cast_rays(void) {

    for (int i = 0; i < NUM_COLS; i++) {
      
        float ray_pos = (float) i / NUM_COLS;
        float ray_angle = (camera.rotation - (camera.fov/2)) + (ray_pos * camera.fov);

        float ray_dx = cosf(ray_angle);
        float ray_dy = sinf(ray_angle);

        int camera_grid_x = (int) camera.x;
        int camera_grid_y = (int) camera.y;

        float delta_dx = (ray_dx == 0) ? 1e30f : fabs(1.0f / ray_dx);
        float delta_dy = (ray_dy == 0) ? 1e30f : fabs(1.0f / ray_dy);

        int step_x, step_y;
        float side_dx, side_dy;

        if (ray_dx < 0) {
            step_x = -1;
            side_dx = (camera.x - camera_grid_x) * delta_dx;
        } else {
            step_x = 1;
            side_dx = (camera_grid_x + 1.0f - camera.x) * delta_dx;
        }

        if (ray_dy < 0) {
            step_y = -1;
            side_dy = (camera.y - camera_grid_y) * delta_dy;
        } else {
            step_y = 1;
            side_dy = (camera_grid_y + 1.0f - camera.y) * delta_dy;
        }

        bool hit = false;
        int side; 

        while (!hit) {
            if (side_dx < side_dy) {
                side_dx += delta_dx;
                camera_grid_x += step_x;
                side = 0;
            } else {
                side_dy += delta_dy;
                camera_grid_y += step_y;
                side = 1;
            }

            if (camera_grid_x < 0 || camera_grid_x >= map_width || camera_grid_y < 0 || camera_grid_y >= map_height) {
                hit = true;
                break;
            }

            if(map[camera_grid_y][camera_grid_x] == 1) hit = true;
        }

        float perp_dist;
        if (side == 0)
            perp_dist = (camera_grid_x - camera.x + (1 - step_x) / 2.0f) / ray_dx;
        else
            perp_dist = (camera_grid_y - camera.y + (1 - step_y) / 2.0f) / ray_dy;

        if (perp_dist <= 0.0f) perp_dist = 0.01f;

        int col_height = (int)(HEIGHT_FACTOR / perp_dist);

        if (col_height > SCREEN_HEIGHT) col_height = SCREEN_HEIGHT;
        if (col_height < 0) col_height = 0;

        cols[i] = col_height;
    }
}

bool update(void) {
    while(SDL_PollEvent(&e)) {
        switch(e.type) {
            case SDL_QUIT:
                return false;
        }
    }

    handle_input();
    cast_rays();
    return true;
}

int main(void) {
    load_map();
    cols = malloc(NUM_COLS * sizeof(float));

    camera.rotation = 0;

    camera.x = SPAWN_X;
    camera.y = SPAWN_Y;
    camera.fov = (float) (CAMERA_FOV * CONV_RAD);
    camera.max_vel = MAX_VEL;
    camera.rot_speed = (float) (ROT_SPEED * CONV_RAD);
   
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

    SDL_DestroyWindow(rc_window);
    SDL_DestroyRenderer(rc_renderer);
    SDL_Quit();
    free(cols);
    for(int i = 0; i < map_height; i++) {
        free(map[i]);
    }
    free(map);
    return 0;
}