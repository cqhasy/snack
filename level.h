#ifndef LEVEL_H
#define LEVEL_H

typedef struct {
    int width, height;
    int speed; // ms
    int **obstacles;
    int obstacle_count;
} Level;

void level_load(Level *level, int level_num);
void level_free(Level *level);

#endif