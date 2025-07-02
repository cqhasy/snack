#include "leaderboard.h"
#include <stdio.h>
#include <string.h>

#define LEADERBOARD_FILE "assets/leaderboard.txt"

void leaderboard_load(LeaderboardEntry *entries, int *count) {
    FILE *fp = fopen(LEADERBOARD_FILE, "r");
    *count = 0;
    if (!fp) return;
    while (fscanf(fp, "%31s %d", entries[*count].name, &entries[*count].score) == 2) {
        (*count)++;
        if (*count >= MAX_ENTRIES) break;
    }
    fclose(fp);
}

void leaderboard_save(LeaderboardEntry *entries, int count) {
    FILE *fp = fopen(LEADERBOARD_FILE, "w");
    if (!fp) return;
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s %d\n", entries[i].name, entries[i].score);
    }
    fclose(fp);
}

void leaderboard_update(LeaderboardEntry *entries, int *count, const char *name, int score) {
    int inserted = 0;
    for (int i = 0; i < *count; i++) {
        if (score > entries[i].score) {
            for (int j = *count; j > i; j--) {
                if (j < MAX_ENTRIES) entries[j] = entries[j-1];
            }
            strncpy(entries[i].name, name, 31);
            entries[i].score = score;
            if (*count < MAX_ENTRIES) (*count)++;
            inserted = 1;
            break;
        }
    }
    if (!inserted && *count < MAX_ENTRIES) {
        strncpy(entries[*count].name, name, 31);
        entries[*count].score = score;
        (*count)++;
    }
}
