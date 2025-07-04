#include "leaderboard.h"
#include <stdio.h>
#include <string.h>

#define LEADERBOARD_FILE "assets/leaderboard.txt"

void leaderboard_load(LeaderboardEntry *entries, int *count) {
    FILE *fp = fopen(LEADERBOARD_FILE, "r");
    *count = 0;
    if (!fp) return;
    while (fscanf(fp, "%31s %d %d", entries[*count].name, &entries[*count].score, &entries[*count].level) == 3) {
        (*count)++;
        if (*count >= MAX_ENTRIES) break;
    }
    fclose(fp);
}

void leaderboard_save(LeaderboardEntry *entries, int count) {
    FILE *fp = fopen(LEADERBOARD_FILE, "w");
    if (!fp) return;
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s %d %d\n", entries[i].name, entries[i].score, entries[i].level);
    }
    fclose(fp);
}

void leaderboard_update(LeaderboardEntry *entries, int *count, const char *name, int score, int level) {
    int inserted = 0;
    for (int i = 0; i < *count; i++) {
        if (entries[i].level == level && strcmp(entries[i].name, name) == 0) {
            if (score > entries[i].score) {
                entries[i].score = score;
            }
            inserted = 1;
            break;
        }
    }
    if (!inserted) {
        int pos = *count;
        for (int i = 0; i < *count; i++) {
            if (entries[i].level == level && score > entries[i].score) {
                pos = i;
                break;
            }
        }
        for (int j = *count; j > pos; j--) {
            if (j < MAX_ENTRIES) entries[j] = entries[j-1];
        }
        strncpy(entries[pos].name, name, 31);
        entries[pos].score = score;
        entries[pos].level = level;
        if (*count < MAX_ENTRIES) (*count)++;
    }
}
