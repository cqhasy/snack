#include "snake.h"
#include <stdlib.h>

void snake_init(Snake *snake, int start_x, int start_y) {
    SnakeNode *node = (SnakeNode*)malloc(sizeof(SnakeNode));
    node->x = start_x;
    node->y = start_y;
    node->next = NULL;
    snake->head = node;
    snake->tail = node;
    snake->length = 1;
    snake->direction = 1; // 默认向右
}

void snake_move(Snake *snake, int grow) {
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {-1, 0, 1, 0};
    int new_x = snake->head->x + dx[snake->direction];
    int new_y = snake->head->y + dy[snake->direction];

    SnakeNode *new_head = (SnakeNode*)malloc(sizeof(SnakeNode));
    new_head->x = new_x;
    new_head->y = new_y;
    new_head->next = snake->head;
    snake->head = new_head;
    snake->length++;

    if (!grow) {
        SnakeNode *tmp = snake->tail;
        SnakeNode *prev = snake->head;
        while (prev->next != tmp) prev = prev->next;
        prev->next = NULL;
        snake->tail = prev;
        free(tmp);
        snake->length--;
    }
}

int snake_check_collision(Snake *snake, int x, int y) {
    SnakeNode *cur = snake->head;
    while (cur) {
        if (cur->x == x && cur->y == y) return 1;
        cur = cur->next;
    }
    return 0;
}

void snake_free(Snake *snake) {
    SnakeNode *cur = snake->head;
    while (cur) {
        SnakeNode *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    snake->head = snake->tail = NULL;
    snake->length = 0;
}