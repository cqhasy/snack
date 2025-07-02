#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "leaderboard.h"
#include "snake.h"
#include <locale.h>

#define CELL_SIZE 40
#define MAX_LEVEL 3

Game game;
LeaderboardEntry entries[MAX_ENTRIES];
int entry_count = 0;
char player_name[32] = "Player";
int selected_level = 1;

GtkWidget *drawing_area;
guint timer_id = 0;
GtkWidget *main_menu_window = NULL;
GtkWidget *game_window = NULL;

// 主菜单相关函数声明，消除警告
void on_start_game(GtkWidget *widget, gpointer data);
void on_game_over(GtkWidget *widget, gpointer data);

// --- 游戏主界面相关 ---
void draw_game(GtkWidget *widget, cairo_t *cr, gpointer data) {
    // 背景
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
    cairo_paint(cr);

    // 障碍物
    cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
    for (int i = 0; i < game.level.obstacle_count; i++) {
        cairo_rectangle(cr, game.level.obstacles[i][0]*CELL_SIZE, game.level.obstacles[i][1]*CELL_SIZE, CELL_SIZE, CELL_SIZE);
        cairo_fill(cr);
    }

    // 食物
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_arc(cr, game.food.x*CELL_SIZE+CELL_SIZE/2, game.food.y*CELL_SIZE+CELL_SIZE/2, CELL_SIZE/2-2, 0, 2*G_PI);
    cairo_fill(cr);

    // 蛇
    cairo_set_source_rgb(cr, 0, 0.6, 0);
    SnakeNode *cur = game.snake.head;
    while (cur) {
        cairo_rectangle(cr, cur->x*CELL_SIZE, cur->y*CELL_SIZE, CELL_SIZE, CELL_SIZE);
        cairo_fill(cr);
        cur = cur->next;
    }
}

gboolean game_timeout(gpointer data) {
    if (game.state != RUNNING) return TRUE;
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {-1, 0, 1, 0};
    int new_x = game.snake.head->x + dx[game.snake.direction];
    int new_y = game.snake.head->y + dy[game.snake.direction];
    if (new_x < 0 || new_x >= game.level.width || new_y < 0 || new_y >= game.level.height) {
        game.state = GAME_OVER;
        gtk_widget_queue_draw(drawing_area);
        on_game_over(drawing_area, NULL);
        return FALSE;
    }
    if (snake_check_collision(&game.snake, new_x, new_y)) {
        game.state = GAME_OVER;
        gtk_widget_queue_draw(drawing_area);
        on_game_over(drawing_area, NULL);
        return FALSE;
    }
    for (int i = 0; i < game.level.obstacle_count; i++) {
        if (game.level.obstacles[i][0] == new_x && game.level.obstacles[i][1] == new_y) {
            game.state = GAME_OVER;
            gtk_widget_queue_draw(drawing_area);
            on_game_over(drawing_area, NULL);
            return FALSE;
        }
    }
    int grow = 0;
    if (new_x == game.food.x && new_y == game.food.y) {
        grow = 1;
        game.score += 10;
        food_generate(&game.food, &game.snake, &game.level);
        if (game.snake.length >= 5 + game.current_level * 5) {
            game.state = NEXT_LEVEL;
            gtk_widget_queue_draw(drawing_area);
            return FALSE;
        }
    }
    snake_move(&game.snake, grow);
    gtk_widget_queue_draw(drawing_area);
    return TRUE;
}

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    draw_game(widget, cr, data);
    // 显示分数
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 18);
    char score_str[64];
    sprintf(score_str, "score: %d  level: %d", game.score, game.current_level);
    cairo_move_to(cr, 10, 20);
    cairo_show_text(cr, score_str);

    if (game.state == GAME_OVER) {
        cairo_set_source_rgb(cr, 1, 0, 0);
        cairo_set_font_size(cr, 32);
        cairo_move_to(cr, 50, 100);
        cairo_show_text(cr, "game over");
    } else if (game.state == NEXT_LEVEL) {
        cairo_set_source_rgb(cr, 0, 0, 1);
        cairo_set_font_size(cr, 32);
        cairo_move_to(cr, 50, 100);
        cairo_show_text(cr, "goto next level");
    } else if (game.state == PAUSED) {
        cairo_set_source_rgb(cr, 0, 0, 1);
        cairo_set_font_size(cr, 32);
        cairo_move_to(cr, 50, 100);
        cairo_show_text(cr, "wait");
    }
    return FALSE;
}

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    if (game.state == GAME_OVER || game.state == NEXT_LEVEL) return FALSE;
    if (event->keyval == GDK_KEY_Up && game.snake.direction != 2) game.snake.direction = 0;
    else if (event->keyval == GDK_KEY_Right && game.snake.direction != 3) game.snake.direction = 1;
    else if (event->keyval == GDK_KEY_Down && game.snake.direction != 0) game.snake.direction = 2;
    else if (event->keyval == GDK_KEY_Left && game.snake.direction != 1) game.snake.direction = 3;
    else if (event->keyval == GDK_KEY_space) {
        if (game.state == RUNNING) game.state = PAUSED;
        else if (game.state == PAUSED) game.state = RUNNING;
    }
    return FALSE;
}

void show_leaderboard_dialog(GtkWindow *parent) {
    leaderboard_load(entries, &entry_count);
    char buf[512] = "";
    for (int i = 0; i < entry_count; i++) {
        char line[64];
        sprintf(line, "%d. %s: %d\n", i+1, entries[i].name, entries[i].score);
        strcat(buf, line);
    }
    GtkWidget *dialog = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "排行榜:\n%s", buf);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void ask_player_name(GtkWindow *parent) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("intput name", parent, GTK_DIALOG_MODAL, "确定", GTK_RESPONSE_OK, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), player_name);
    gtk_box_pack_start(GTK_BOX(content), entry, TRUE, TRUE, 0);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        strncpy(player_name, gtk_entry_get_text(GTK_ENTRY(entry)), 31);
    }
    gtk_widget_destroy(dialog);
}

void close_game_window() {
    if (game_window) {
        gtk_widget_destroy(game_window);
        game_window = NULL;
    }
    if (timer_id) {
        g_source_remove(timer_id);
        timer_id = 0;
    }
}

void restart_game(GtkWidget *widget, gpointer data) {
    game_free(&game);
    game_init(&game, selected_level);
    if (timer_id) g_source_remove(timer_id);
    timer_id = g_timeout_add(game.level.speed, game_timeout, NULL);
    gtk_widget_queue_draw(drawing_area);
}

void next_level(GtkWidget *widget, gpointer data) {
    if (game.current_level < MAX_LEVEL) {
        game_free(&game);
        game_init(&game, game.current_level + 1);
        selected_level = game.current_level;
        if (timer_id) g_source_remove(timer_id);
        timer_id = g_timeout_add(game.level.speed, game_timeout, NULL);
        gtk_widget_queue_draw(drawing_area);
    }
}

void on_game_over(GtkWidget *widget, gpointer data) {
    leaderboard_load(entries, &entry_count);
    leaderboard_update(entries, &entry_count, player_name, game.score);
    leaderboard_save(entries, entry_count);
    // 弹窗选择操作
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(game_window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_NONE, "Game Over!\nscore: %d", game.score);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Restart", 1);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Main Menu", 2);
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    close_game_window();
    // 只要不是Restart都回主菜单
    if (response == 1) {
        on_start_game(NULL, NULL);
    } else {
        if (main_menu_window) gtk_widget_show_all(main_menu_window);
    }
}

gboolean on_timer(gpointer data) {
    if (game.state == GAME_OVER) {
        on_game_over(drawing_area, NULL);
        return FALSE;
    }
    if (game.state == NEXT_LEVEL) {
        next_level(drawing_area, NULL);
        return FALSE;
    }
    return TRUE;
}

// --- 主菜单相关 ---
void on_start_game(GtkWidget *widget, gpointer data) {
    if (main_menu_window) gtk_widget_hide(main_menu_window);
    if (game_window) gtk_widget_destroy(game_window);
    game_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(game_window), "贪吃蛇");
    gtk_window_set_resizable(GTK_WINDOW(game_window), FALSE);
    game_init(&game, selected_level);
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, game.level.width*CELL_SIZE, game.level.height*CELL_SIZE+30);
    gtk_container_add(GTK_CONTAINER(game_window), drawing_area);
    g_signal_connect(game_window, "destroy", G_CALLBACK(close_game_window), NULL);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);
    g_signal_connect(game_window, "key-press-event", G_CALLBACK(on_key_press), NULL);
    timer_id = g_timeout_add(game.level.speed, game_timeout, NULL);
    gtk_widget_show_all(game_window);
}

void on_select_level(GtkComboBoxText *combo, gpointer data) {
    selected_level = gtk_combo_box_get_active(GTK_COMBO_BOX(combo)) + 1;
}

void on_show_leaderboard(GtkWidget *widget, gpointer data) {
    show_leaderboard_dialog(GTK_WINDOW(main_menu_window));
}

void on_quit(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

void show_main_menu() {
    main_menu_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_menu_window), "meta");
    gtk_window_set_resizable(GTK_WINDOW(main_menu_window), FALSE);
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *label = gtk_label_new("choose level:");
    GtkWidget *level_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(level_combo), "first level");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(level_combo), "second level");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(level_combo), "third level");
    gtk_combo_box_set_active(GTK_COMBO_BOX(level_combo), 0);
    GtkWidget *start_btn = gtk_button_new_with_label("start");
    GtkWidget *leaderboard_btn = gtk_button_new_with_label("bang");
    GtkWidget *quit_btn = gtk_button_new_with_label("exit");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(vbox), level_combo, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(vbox), start_btn, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(vbox), leaderboard_btn, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(vbox), quit_btn, FALSE, FALSE, 10);
    gtk_container_add(GTK_CONTAINER(main_menu_window), vbox);
    g_signal_connect(start_btn, "clicked", G_CALLBACK(on_start_game), NULL);
    g_signal_connect(level_combo, "changed", G_CALLBACK(on_select_level), NULL);
    g_signal_connect(leaderboard_btn, "clicked", G_CALLBACK(on_show_leaderboard), NULL);
    g_signal_connect(quit_btn, "clicked", G_CALLBACK(on_quit), NULL);
    g_signal_connect(main_menu_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(main_menu_window);
    // 自动弹出输入名字
    ask_player_name(GTK_WINDOW(main_menu_window));
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    gtk_init(&argc, &argv);
    show_main_menu();
    gtk_main();
    return 0;
}
