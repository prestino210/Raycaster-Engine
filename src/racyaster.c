#include "raycaster.h"
#include "globals.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdlib.h>
unsigned int map_width;
unsigned int map_height;
int** map = NULL;

void cast_rays(Camera camera) {

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

        cols[i] = col_height++;
    }
}

void load_map(char* file_path) {
    FILE* file = fopen(file_path, "r");

    if(file == NULL) {
        destruct(EXIT_FAILURE, "Could not open specified file.");
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t nread;

    unsigned int i = 0;
    while((nread = getline(&line, &len, file)) != -1) {
       
        if(line[nread - 1] == '\n') nread--;

        if(i == 0 && nread > 0) {
            map_width = nread;
        } else if(i == 0 && nread <= 0) {
            destruct(EXIT_FAILURE, "Invalid map width.");
        } else if(i != 0 && nread != map_width) {
            destruct(EXIT_FAILURE, "Inconsistent map width.");
        }

        int* row = malloc((nread) * sizeof(int));

        for(int j = 0; j < nread; j++) {
            row[j] = (int) line[j] - '0';
        }
        map = realloc(map, (i+1) * (map_width * (sizeof(int))));
        map[i] = row;
       
        i++;
    }

    if(i > 0) {
        map_height = i;
    } else {
        destruct(EXIT_FAILURE, "Invalid number of lines.");
    }

    free(line);
    fclose(file);
}
