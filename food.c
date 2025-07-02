#include "food.h"
#include "snake.h"
#include "level.h"
#include <stdlib.h>
#include <time.h>

void food_generate(Food *food, Snake *snake, Level *level) {
    int valid = 0;
    int x, y;
    srand(time(NULL) + rand());
    while (!valid) {
        x = rand() % level->width;
        y = rand() % level->height;
        valid = 1;
        // 检查是否在蛇身上
        if (snake_check_collision(snake, x, y)) {
            valid = 0;
            continue;
        }
        // 检查是否在障碍物上
        for (int i = 0; i < level->obstacle_count; i++) {
            if (level->obstacles[i][0] == x && level->obstacles[i][1] == y) {
                valid = 0;
                break;
            }
        }
    }
    food->x = x;
    food->y = y;
}