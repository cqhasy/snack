#ifndef GAME_H
#define GAME_H

#include "snake.h"
#include "food.h"
#include "level.h"

typedef enum { RUNNING, PAUSED, GAME_OVER, NEXT_LEVEL } GameState;

typedef struct {
    Snake snake;
    Food food;
    Level level;
    int score;
    GameState state;
    int current_level;
} Game;

void game_init(Game *game, int level_num);
void game_free(Game *game);

#endif
