#ifndef CAMERA_H
#define CAMERA_H

#define MAX_VEL 1.5
#define ROT_SPEED 130
#define SPAWN_X 0
#define SPAWN_Y 0
#define FOV 70

typedef struct {
    float rotation; 
    float fov;
    float x, y; 
    float max_vel; 
    float rot_speed; 
} Camera;

Camera create_camera(void);
void handle_input(Camera *camera);

#endif