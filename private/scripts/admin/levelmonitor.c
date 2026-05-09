#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "db.h"

void display(sqlite3 *db) {

    sqlite3_stmt *stmt = db_prepare(db,
        "SELECT username, level, score, completed "
        "FROM progress "
        "ORDER BY level DESC, score DESC"
    );

    if (!stmt) return;

    printf("\033[H");

    printf("====== LEVEL MONITOR (LIVE) ======\n\n");

    printf("%-5s %-15s %-8s %-10s %-12s\n",
           "Rank", "Player", "Level", "Score", "Status");

    printf("------------------------------------------------------\n");

    int rank = 1;

    while (db_step(stmt) == SQLITE_ROW) {

        const char *user = (const char*)sqlite3_column_text(stmt, 0);
        int level = sqlite3_column_int(stmt, 1);
        int score = sqlite3_column_int(stmt, 2);
        int completed = sqlite3_column_int(stmt, 3);

        const char *status = completed ? "DONE" : "PLAYING";

        printf("%-5d %-15s %-8d %-10d %-12s\n",
               rank++,
               user,
               level,
               score,
               status);
    }

    sqlite3_finalize(stmt);

    printf("\n(Refreshing every 2 seconds... Ctrl+C to exit)\n");
}

int main() {

    setvbuf(stdout, NULL, _IONBF, 0);

    printf("\033[2J"); // clear once

    sqlite3 *db = db_open();
    if (!db) return 1;

    while (1) {
        display(db);
        sleep(2);
    }

    db_close(db);
    return 0;
}