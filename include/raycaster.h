#ifndef RAYCASTER_H
#define RAYCASTER_H
#include "camera.h"
extern unsigned int map_width;
extern unsigned int map_height;
extern int** map;

void load_map(char* map_file);
void cast_rays(Camera camera);

#endif