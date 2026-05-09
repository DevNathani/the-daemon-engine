#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#define MAX 256

// -------- check if Linux user exists --------
int user_exists(const char *user) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "id %s > /dev/null 2>&1", user);
    return system(cmd) == 0;
}

int main() {

    sqlite3 *db = db_open();
    if (!db) {
        fprintf(stderr, "DB open failed\n");
        return 1;
    }

    sqlite3_stmt *stmt = db_prepare(db,
        "SELECT username, password FROM players"
    );

    if (!stmt) {
        db_close(db);
        return 1;
    }

    int created = 0, skipped = 0, initialized = 0;

    printf("[+] Syncing players from DB\n\n");

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        const char *user = (const char*)sqlite3_column_text(stmt, 0);
        const char *pass = (const char*)sqlite3_column_text(stmt, 1);

        if (!user || !pass) continue;

        char cmd[512];

        // SYSTEM USER RECONCILIATION
        if (!user_exists(user)) {

            printf(" Creating system user: %s\n", user);

            // create user
            snprintf(cmd, sizeof(cmd), "useradd -m %s", user);
            system(cmd);

            // set password
            snprintf(cmd, sizeof(cmd),
                     "echo '%s:%s' | chpasswd", user, pass);
            system(cmd);

            // add to group
            snprintf(cmd, sizeof(cmd),
                     "usermod -aG game %s", user);
            system(cmd);

            // secure home
            snprintf(cmd, sizeof(cmd),
                     "chown %s:%s /home/%s && chmod 700 /home/%s",
                     user, user,user,user);
            system(cmd);

            printf("[✓] System user created: %s\n\n", user);
            created++;

        } else {
            printf("[=] Exists: %s\n", user);
            skipped++;
        }

        // PROGRESS RECONCILIATION
        sqlite3_stmt *pstmt = db_prepare(db,
            "SELECT 1 FROM progress WHERE username=?"
        );

        int exists = 0;

        if (pstmt) {
            sqlite3_bind_text(pstmt, 1, user, -1, SQLITE_STATIC);

            if (sqlite3_step(pstmt) == SQLITE_ROW) {
                exists = 1;
            }

            sqlite3_finalize(pstmt);
        }

        if (!exists) {
            pstmt = db_prepare(db,
                "INSERT INTO progress(username, level, score, completed, gaveup, last_updated) "
                "VALUES(?, 0, 0, 0, 0, strftime('%s','now'))"
            );

            if (pstmt) {
                sqlite3_bind_text(pstmt, 1, user, -1, SQLITE_STATIC);

                if (sqlite3_step(pstmt) != SQLITE_DONE) {
                    printf("[!] Failed to init progress: %s\n", user);
                } else {
                    printf("[+] Initialized progress: %s\n", user);
                    initialized++;
                }

                sqlite3_finalize(pstmt);
            }
        }
    }

    sqlite3_finalize(stmt);
    db_close(db);

    // SUMMARY
    printf("=================================\n");
    printf("SUMMARY\n");
    printf("=================================\n");
    printf("Created Users     : %d\n", created);
    printf("Existing Users    : %d\n", skipped);
    printf("Progress Init     : %d\n", initialized);
    printf("=================================\n");

    return 0;
}