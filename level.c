#include "level.h"
#include <stdlib.h>

void level_load(Level *level, int level_num) {
    // 简单示例：三关，障碍物逐渐增多
    if (level->obstacles) level_free(level);
    if (level_num == 1) {
        level->width = 20;
        level->height = 20;
        level->speed = 200;
        level->obstacle_count = 0;
        level->obstacles = NULL;
    } else if (level_num == 2) {
        level->width = 25;
        level->height = 25;
        level->speed = 150;
        level->obstacle_count = 4;
        level->obstacles = (int**)malloc(sizeof(int*) * 4);
        for (int i = 0; i < 4; i++) {
            level->obstacles[i] = (int*)malloc(sizeof(int) * 2);
        }
        level->obstacles[0][0] = 10; level->obstacles[0][1] = 10;
        level->obstacles[1][0] = 14; level->obstacles[1][1] = 14;
        level->obstacles[2][0] = 5;  level->obstacles[2][1] = 15;
        level->obstacles[3][0] = 15; level->obstacles[3][1] = 5;
    } else {
        level->width = 30;
        level->height = 30;
        level->speed = 100;
        level->obstacle_count = 8;
        level->obstacles = (int**)malloc(sizeof(int*) * 8);
        for (int i = 0; i < 8; i++) {
            level->obstacles[i] = (int*)malloc(sizeof(int) * 2);
        }
        for (int i = 0; i < 8; i++) {
            level->obstacles[i][0] = 5 + i * 3;
            level->obstacles[i][1] = 10 + (i % 2) * 5;
        }
    }
}

void level_free(Level *level) {
    if (level->obstacles) {
        for (int i = 0; i < level->obstacle_count; i++) {
            free(level->obstacles[i]);
        }
        free(level->obstacles);
        level->obstacles = NULL;
    }
}