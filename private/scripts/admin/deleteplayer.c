#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

// -------- check user exists (system) --------
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

    printf("[!] Removing player: %s\n", user);

    sqlite3 *db = db_open();
    if (!db) {
        printf("[!] DB open failed\n");
        return 1;
    }


    // DELETE FROM DB FIRST
    sqlite3_stmt *stmt;

    // delete submissions (history)
    stmt = db_prepare(db,
        "DELETE FROM submissions WHERE username=?"
    );
    if (stmt) {
        sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // delete progress
    stmt = db_prepare(db,
        "DELETE FROM progress WHERE username=?"
    );
    if (stmt) {
        sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // delete player
    stmt = db_prepare(db,
        "DELETE FROM players WHERE username=?"
    );
    if (stmt) {
        sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    db_close(db);

    // DELETE SYSTEM USER
    if (user_exists(user)) {
        char cmd[MAX];
        snprintf(cmd, sizeof(cmd), "userdel -r %s 2>/dev/null", user);
        system(cmd);
        printf(" System user deleted\n");
    } else {
        printf(" System user not found\n");
    }

    printf(" Player fully removed: %s\n", user);

    return 0;
}