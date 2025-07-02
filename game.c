#include "game.h"
#include "snake.h"
#include "food.h"
#include "level.h"
#include <stdlib.h>

void game_init(Game *game, int level_num) {
    level_load(&game->level, level_num);
    snake_init(&game->snake, game->level.width / 2, game->level.height / 2);
    food_generate(&game->food, &game->snake, &game->level);
    game->score = 0;
    game->state = RUNNING;
    game->current_level = level_num;
}

void game_free(Game *game) {
    snake_free(&game->snake);
    level_free(&game->level);
}
