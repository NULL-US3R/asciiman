#ifndef GUI_H
#define GUI_H

#include "level.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define LOGO_HEIGHT 8
#define LOGO_WIDTH 80
#define COLOR_ERROR 1
#define COLOR_PROMPT 2
#define COLOR_PACMAN 3

static const char logo[8][80] = {
    " $$$$$$\\   $$$$$$\\   $$$$$$\\  $$$$$$\\ $$$$$$\\ $$\\      $$\\  $$$$$$\\  $$\\   $$\\ ",
    "$$  __$$\\ $$  __$$\\ $$  __$$\\ \\_$$  _|\\_$$  _|$$$\\    $$$ |$$  __$$\\ $$$\\  $$ |",
    "$$ /  $$ |$$ /  \\__|$$ /  \\__|  $$ |    $$ |  $$$$\\  $$$$ |$$ /  $$ |$$$$\\ $$ |",
    "$$$$$$$$ |\\$$$$$$\\  $$ |        $$ |    $$ |  $$\\$$\\$$ $$ |$$$$$$$$ |$$ $$\\$$ |",
    "$$  __$$ | \\____$$\\ $$ |        $$ |    $$ |  $$ \\$$$  $$ |$$  __$$ |$$ \\$$$$ |",
    "$$ |  $$ |$$\\   $$ |$$ |  $$\\   $$ |    $$ |  $$ |\\$  /$$ |$$ |  $$ |$$ |\\$$$ |",
    "$$ |  $$ |\\$$$$$$  |\\$$$$$$  |$$$$$$\\ $$$$$$\\ $$ | \\_/ $$ |$$ |  $$ |$$ | \\$$ |",
    "\\__|  \\__| \\______/  \\______/ \\______|\\______|\\__|     \\__|\\__|  \\__|\\__|  \\__|"
};

typedef enum {
    NO_MOVE,
    UP,
    DOWN,
    LEFT,
    RIGHT
} move_direction_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t score;
    move_direction_t move_direction;
    uint16_t fog_radius;
} pacman_t;

static const int move_dx[] = {
    [NO_MOVE] = 0,
    [UP]      = 0,
    [DOWN]    = 0,
    [LEFT]    = -1,
    [RIGHT]   = 1
},
    move_dy[] = {
    [NO_MOVE] = 0,
    [UP]      = -1,
    [DOWN]    = 1,
    [LEFT]    = 0,
    [RIGHT]   = 0
};

static const char* move_direction_names[] = {
    [NO_MOVE] = "NO_MOVE",
    [UP]      = "UP",
    [DOWN]    = "DOWN",
    [LEFT]    = "LEFT",
    [RIGHT]   = "RIGHT"
};

void process_fog(const level_t *level_src, level_t *level_res, const pacman_t *pacman);



#endif
