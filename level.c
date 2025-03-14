#include "level.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int get_seed(level_t* level, char* buffer, size_t buffer_size){
    if(buffer_size < SEED_BUFFER_SIZE){
        return -1;
    }

    int written = snprintf(buffer, buffer_size, "%020lu%05hu%05hu%05hu%05hu%05hu",
                           level->random,
                           level->height,
                           level->width,
                           level->paths,
                           level->max_path_length,
                           level->points);

    return (written == SEED_STRING_LENGTH) ? 0 : -2;
}


int read_from_string(const char* buffer, level_t* level){
    if(!buffer || !level){
        return -1;
    }

    if(strlen(buffer) != SEED_STRING_LENGTH){
        return -2;
    }

    int scanned = sscanf(buffer, "%020lu%05hu%05hu%05hu%05hu%05hu",
                         &level->random,
                         &level->height,
                         &level->width,
                         &level->paths,
                         &level->max_path_length,
                         &level->points);

    return (scanned == 6) ? 0 : -3;
}

int alloc_level(level_t* level){
    if(level->height == 0 || level->width == 0){
        return -1;
    }

    if(level->level_arr){
        free(level->level_arr);
    }
    
    level->level_arr = (char**) malloc(sizeof(char *) * level->height);
    if(!level->level_arr){
        return -2;
    }
    for(size_t i = 0; i < level->height; i++){
        level->level_arr[i] = (char*) malloc(sizeof(char) * level->width);
        if (!level->level_arr){
            return -2;
        }
    }
    return 0;
}

void free_level(level_t* level){
    for(size_t i = 0; i < level->height; i++){
        free(level->level_arr[i]);
    }
    free(level->level_arr);
}

void init_level(level_t* level, uint16_t height, uint16_t width,
                uint16_t paths, uint16_t max_path_length, uint16_t points){
    level->random          = time(NULL);
    level->height          = height;
    level->width           = width;
    level->paths           = paths;
    level->max_path_length = max_path_length;
    level->points          = points;
    alloc_level(level);
}

void generate_level(level_t* level){
    char** arr = level->level_arr;
    srand(level->random);
    memset(arr, object_chars[BORDER], level->height * level->width);
    arr[level->height/2][level->width/2] = object_chars[NOT_GENERATED];
    generate_paths(level);
    
}

void generate_paths(level_t* level){
    for(int iter = 0; iter < level->paths; iter++){
        for(int i = 0; i < level->height; i++){
            for(int j = 0; j < level->width; j++){
                if (level->level_arr[i][j] == object_chars[NOT_GENERATED]){
                    int direction = rand() % 4, length = rand() % level->max_path_length;
                    int di[] = {0, 0, 1, -1};  // Direction increments for rows
                    int dj[] = {1, -1, 0, 0};  // Direction increments for columns
                    int limit_i = (di[direction] == 0) ? i : (di[direction] == 1 ? i + length : i - length);
                    int limit_j = (dj[direction] == 0) ? j : (dj[direction] == 1 ? j + length : j - length);

                    if(limit_i > level->height){
                        limit_i = level->height - 1;
                    }
                    if(limit_i < 0){
                        limit_i = 0;
                    }
                    if(limit_j > level->width){
                        limit_j = level->width - 1;
                    }
                    if(limit_j < 0){
                        limit_j = 0;
                    }

                    // Set the path to 0
                    for(int l = 0; l <= length; l++){
                        int ni = i + di[direction] * l;
                        int nj = j + dj[direction] * l;
                        if (ni >= 0 && nj >= 0 && ni < level->height && nj < level->width){
                            level->level_arr[ni][nj] = object_chars[PATH];
                        }
                    }
                    // Restore the end of the path
                    level->level_arr[limit_i][limit_j] = object_chars[NOT_GENERATED];
                }
            }
        }
    }
    // Reset all remaining 3s to 0
    for(int i = 0; i < level->height; i++){
        for(int j = 0; j < level->width; j++){
            if (level->level_arr[i][j] == object_chars[NOT_GENERATED]){
                level->level_arr[i][j] = object_chars[PATH];
            }
        }
    }
}