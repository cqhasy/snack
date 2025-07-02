#ifndef FOOD_H
#define FOOD_H

#include "snake.h"
#include "level.h"

typedef struct {
    int x, y;
} Food;

void food_generate(Food *food, Snake *snake, Level *level);

#endif