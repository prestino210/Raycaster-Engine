#include "camera.h"
#include <math.h>
#include <SDL2/SDL.h>
#include "globals.h"

Camera create_camera() {
    Camera camera = {.rotation = 0, .x = SPAWN_X, .y = SPAWN_Y,
        .fov = (float) (FOV * CONV_RAD), .max_vel = MAX_VEL,
        .rot_speed = (float) (ROT_SPEED * CONV_RAD)};
    return camera;
}

void handle_input(Camera *camera) {
    const Uint8* keyboard = SDL_GetKeyboardState(NULL);

    float moveX = 0;
    float moveY = 0;

    if(keyboard[SDL_SCANCODE_W]) {
        moveX += cosf(camera->rotation);
        moveY += sinf(camera->rotation);
    }
    if(keyboard[SDL_SCANCODE_S]) {
        moveX -= cosf(camera->rotation);
        moveY -= sinf(camera->rotation);
    }
    if (keyboard[SDL_SCANCODE_A]) {
        camera->rotation -= camera->rot_speed * delta_time;
    }
    if (keyboard[SDL_SCANCODE_D]) { 
        camera->rotation += camera->rot_speed * delta_time;
    }

    float length = sqrtf(moveX*moveX + moveY*moveY);
    if(length != 0.0f) {
        moveX /= length;
        moveY /= length;
    }

    camera->x += moveX * camera->max_vel * delta_time;
    camera->y += moveY * camera->max_vel * delta_time;
}