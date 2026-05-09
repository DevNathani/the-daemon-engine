#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include "db.h"

#define MAX 256

// -------- validate username --------
int valid_username(const char *u) {
    if (!u || strlen(u) == 0) return 0;

    for (int i = 0; u[i]; i++) {
        if (!isalnum(u[i]) && u[i] != '_')
            return 0;
    }
    return 1;
}

// -------- check system user exists --------
int user_exists(const char *user) {
    char cmd[MAX];
    snprintf(cmd, sizeof(cmd), "id %s > /dev/null 2>&1", user);
    return system(cmd) == 0;
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: %s <username>\n", argv[0]);
        return 1;
    }

    const char *user = argv[1];

    // -------- validation --------
    if (!valid_username(user)) {
        printf("[!] Invalid username\n");
        return 1;
    }

    if (!user_exists(user)) {
        printf("[!] User does not exist: %s\n", user);
        return 1;
    }

    printf(" Resetting player: %s\n", user);

    sqlite3 *db = db_open();
    if (!db) {
        printf("[!] DB open failed\n");
        return 1;
    }

    sqlite3_stmt *stmt;

    //  DELETE SUBMISSIONS
    stmt = db_prepare(db,
        "DELETE FROM submissions WHERE username=?"
    );
    if (stmt) {
        sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    //  Update Progress in DB
    stmt = db_prepare(db,
        "UPDATE progress SET "
        "level=0, score=0, completed=0, gaveup=0, "
        "last_updated=strftime('%s','now') "
        "WHERE username=?"
    );

    if (stmt) {
        sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("[!] Failed to reset progress\n");
            sqlite3_finalize(stmt);
            db_close(db);
            return 1;
        }

        sqlite3_finalize(stmt);
    }

    db_close(db);

    printf(" DB state reset complete\n");

    //  RELOAD ENVIRONMENT
    printf(" Reloading level workspace...\n");

    if (fork() == 0) {
        execl("/opt/game/bin/load_level",
              "load_level", user, NULL);
        _exit(1);
    }

    wait(NULL);

    printf(" Player reset successful\n");

    return 0;
}