#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#define MAX_ENTRIES 10

typedef struct {
    char name[32];
    int score;
} LeaderboardEntry;

void leaderboard_load(LeaderboardEntry *entries, int *count);
void leaderboard_save(LeaderboardEntry *entries, int count);
void leaderboard_update(LeaderboardEntry *entries, int *count, const char *name, int score);

#endif
