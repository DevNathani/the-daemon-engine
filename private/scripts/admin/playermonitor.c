#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "db.h"

#define MAX_USERS 100
#define MAX_LEVELS 50
#define CELL_W 16
#define USER_W 14

typedef struct {
    char username[64];
    char cell[MAX_LEVELS][32];   // clean text only
    int gaveup[MAX_LEVELS];      // flag
    time_t level_time[MAX_LEVELS];
    int score;
} Player;

Player players[MAX_USERS];
int player_count = 0;
int max_level = 0;

// get or create player
int get_player(const char *name) {
    for (int i = 0; i < player_count; i++) {
        if (strcmp(players[i].username, name) == 0)
            return i;
    }

    strcpy(players[player_count].username, name);

    for (int j = 0; j < MAX_LEVELS; j++) {
        strcpy(players[player_count].cell[j], "-");
        players[player_count].gaveup[j] = 0;
        players[player_count].level_time[j] = 0;
    }

    players[player_count].score = 0;

    return player_count++;
}

// load data
void load_data(sqlite3 *db) {

    sqlite3_stmt *stmt = db_prepare(db,
        "SELECT s.username, s.level, s.time, s.gaveup, p.score "
        "FROM submissions s "
        "JOIN progress p ON s.username = p.username "
        "ORDER BY s.username, s.level"
    );

    if (!stmt) return;

    while (db_step(stmt) == SQLITE_ROW) {

        const char *user = (const char*)sqlite3_column_text(stmt, 0);
        int level = sqlite3_column_int(stmt, 1);
        time_t t = sqlite3_column_int(stmt, 2);
        int gaveup = sqlite3_column_int(stmt, 3);
        int score = sqlite3_column_int(stmt, 4);

        int idx = get_player(user);

        if (level > max_level) max_level = level;

        players[idx].level_time[level] = t;
        players[idx].score = score;
        players[idx].gaveup[level] = gaveup;

        // format time HH:MM
        struct tm *tm = localtime(&t);
        char timebuf[16];
        strftime(timebuf, sizeof(timebuf), "%H:%M", tm);

        // calculate minutes
        int minutes = 0;
        if (level > 0 && players[idx].level_time[level-1] != 0) {
            minutes = (int)((t - players[idx].level_time[level-1]) / 60);
        }

        if (level == 0)
            snprintf(players[idx].cell[level], sizeof(players[idx].cell[level]), "%s", timebuf);
        else
            snprintf(players[idx].cell[level], sizeof(players[idx].cell[level]),
                     "%s(%dm)", timebuf, minutes);
    }

    sqlite3_finalize(stmt);
}

// sort by score
int compare_score(const void *a, const void *b) {
    return ((Player*)b)->score - ((Player*)a)->score;
}

// print table
void print_table() {

    printf("\033[H\033[J");

    // header
    printf("%-*s ", USER_W, "USER");

    for (int i = 0; i <= max_level; i++)
        printf("L%-*d ", CELL_W - 1, i);

    printf(" SCORE\n");

    // separator
    int total_width = USER_W + (max_level + 1) * (CELL_W + 1) + 10;
    for (int i = 0; i < total_width; i++) printf("-");
    printf("\n");

    // rows
    for (int i = 0; i < player_count; i++) {

        printf("%-*s ", USER_W, players[i].username);

        for (int j = 0; j <= max_level; j++) {

            if (players[i].gaveup[j]) {
                // color applied AFTER formatting → correct alignment
                printf("\033[31m%-*s\033[0m ", CELL_W, players[i].cell[j]);
            } else {
                printf("%-*s ", CELL_W, players[i].cell[j]);
            }
        }

        printf(" %-6d\n", players[i].score);
    }

    printf("\nRefreshing every 2 seconds...\n");
}

// main
int main() {

    sqlite3 *db = db_open();
    if (!db) return 1;

    while (1) {

        player_count = 0;
        max_level = 0;

        load_data(db);

        qsort(players, player_count, sizeof(Player), compare_score);

        print_table();

        sleep(2);
    }

    db_close(db);
    return 0;
}