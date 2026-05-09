#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "db.h"

void display(sqlite3 *db) {

    sqlite3_stmt *stmt = db_prepare(db,
        "SELECT username, level, score, last_updated "
        "FROM progress "
        "ORDER BY score DESC, level DESC, last_updated ASC "
        "LIMIT 10"
    );

    if (!stmt) return;

    printf("\033[H"); // move cursor to top

    printf("====== LEADERBOARD (LIVE) ======\n\n");
    printf("Rank  Player          Level   Score     Time\n");
    printf("----------------------------------------------------------\n");

    int rank = 1;

    while (db_step(stmt) == SQLITE_ROW) {

        const char *user = (const char*)sqlite3_column_text(stmt, 0);
        int level = sqlite3_column_int(stmt, 1);
        int score = sqlite3_column_int(stmt, 2);
        long timestamp = sqlite3_column_int64(stmt, 3);

        char buf[64];
        time_t t = (time_t)timestamp;
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));

        printf("%-5d %-15s %-8d %-10d %-20s\n",
               rank++,
               user,
               level,
               score,
               buf);
    }

    sqlite3_finalize(stmt);

    printf("\n(Refreshing every 2 seconds... Ctrl+C to exit)\n");
}

int main() {

    // disable buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    // clear screen once
    printf("\033[2J");

    sqlite3 *db = db_open();
    if (!db) return 1;

    while (1) {
        display(db);
        sleep(2);
    }

    db_close(db);
    return 0;
}