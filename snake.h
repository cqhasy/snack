#ifndef SNAKE_H
#define SNAKE_H

typedef struct SnakeNode {
    int x, y;
    struct SnakeNode *next;
} SnakeNode;

typedef struct {
    SnakeNode *head;
    SnakeNode *tail;
    int length;
    int direction; // 0:上 1:右 2:下 3:左
} Snake;

void snake_init(Snake *snake, int start_x, int start_y);
void snake_move(Snake *snake, int grow);
int snake_check_collision(Snake *snake, int x, int y);
void snake_free(Snake *snake);

#endif