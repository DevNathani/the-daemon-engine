#include "db.h"
#include <stdio.h>
#include <unistd.h>

#ifdef DEV
#define DB_PATH "/home/k1ra/game-server/var/game/db/game.db"
#else
#define DB_PATH "/var/game/db/game.db"
#endif

// open
sqlite3* db_open() {
    sqlite3 *db;

    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        fprintf(stderr, "DB open error: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, NULL);
    sqlite3_exec(db, "PRAGMA synchronous=NORMAL;", NULL, NULL, NULL);

    return db;
}

// close
void db_close(sqlite3 *db) {
    if (db) sqlite3_close(db);
}

// prepare
sqlite3_stmt* db_prepare(sqlite3 *db, const char *query) {
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    return stmt;
}

// step with retry
int db_step(sqlite3_stmt *stmt) {
    int rc;
    int retries = 5;

    while ((rc = sqlite3_step(stmt)) == SQLITE_BUSY && retries--) {
        usleep(10000);
    }

    return rc;
}

// get level
int db_get_level(sqlite3 *db, const char *user) {
    sqlite3_stmt *stmt = db_prepare(db,
        "SELECT level FROM progress WHERE username=?"
    );

    if (!stmt) return 0;

    sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);

    int level = 0;

    if (db_step(stmt) == SQLITE_ROW) {
        level = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return level;
}

// get level name 
const char* db_get_level_name(sqlite3 *db, int level) {
    static char name[128];

    sqlite3_stmt *stmt = db_prepare(db,
        "SELECT name FROM level_names WHERE level=?"
    );

    if (!stmt) return NULL;

    sqlite3_bind_int(stmt, 1, level);

    if (db_step(stmt) == SQLITE_ROW) {
        snprintf(name, sizeof(name), "%s",
            (const char*)sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);
    return name;
}

// read hash of level
const char* db_get_level_hash(sqlite3 *db, int level) {
    static char hash[128];

    sqlite3_stmt *stmt = db_prepare(db,
        "SELECT hash FROM level_data WHERE level=?"
    );

    if (!stmt) return NULL;

    sqlite3_bind_int(stmt, 1, level);

    if (db_step(stmt) == SQLITE_ROW) {
        snprintf(hash, sizeof(hash), "%s",
            (const char*)sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);
    return hash;
}

// update progress
int db_update_progress(sqlite3 *db, const char *user, int level, int score, int completed) {
    sqlite3_stmt *stmt = db_prepare(db,
        "UPDATE progress SET level=?, score=?, completed=?, gaveup=0, last_updated=strftime('%s','now') WHERE username=?"
    );

    if (!stmt) return -1;

    sqlite3_bind_int(stmt, 1, level);
    sqlite3_bind_int(stmt, 2, score);
    sqlite3_bind_int(stmt, 3, completed);
    sqlite3_bind_text(stmt, 4, user, -1, SQLITE_STATIC);

    int rc = db_step(stmt);

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

// mark giveup
int db_set_giveup(sqlite3 *db, const char *user) {
    sqlite3_stmt *stmt = db_prepare(db,
        "UPDATE progress SET gaveup=1 WHERE username=?"
    );

    if (!stmt) return -1;

    sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);

    int rc = db_step(stmt);

    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}

// insert submission
int db_insert_submission(sqlite3 *db, const char *user, int level, int gaveup, int completed) {
    sqlite3_stmt *stmt = db_prepare(db,
        "INSERT OR REPLACE INTO submissions (username, level, gaveup, completed, time) VALUES (?, ?, ?, ?, strftime('%s','now'))"
    );

    if (!stmt) return -1;

    sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, level);
    sqlite3_bind_int(stmt, 3, gaveup);
    sqlite3_bind_int(stmt, 4, completed);

    int rc = db_step(stmt);

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

// get level message
const char* db_get_level_message(sqlite3 *db, int level) {
    static char msg[1024];   // static → survives function return
    msg[0] = '\0';           // reset

    sqlite3_stmt *stmt = db_prepare(db,
        "SELECT messages FROM level_content WHERE level=?"
    );

    if (!stmt) return NULL;

    sqlite3_bind_int(stmt, 1, level);

    if (db_step(stmt) == SQLITE_ROW) {
        const unsigned char *text = sqlite3_column_text(stmt, 0);
        if (text)
            snprintf(msg, sizeof(msg), "%s", text);
    }

    sqlite3_finalize(stmt);

    return (msg[0] != '\0') ? msg : NULL;
}

const char* db_get_final_hint(sqlite3 *db, int level) {
    static char hint[1024];
    hint[0] = '\0';

    sqlite3_stmt *stmt = db_prepare(db,
        "SELECT final_hint FROM level_content WHERE level=?"
    );

    if (!stmt) return NULL;

    sqlite3_bind_int(stmt, 1, level);

    if (db_step(stmt) == SQLITE_ROW) {
        const unsigned char *text = sqlite3_column_text(stmt, 0);
        if (text)
            snprintf(hint, sizeof(hint), "%s", text);
    }

    sqlite3_finalize(stmt);

    return (hint[0] != '\0') ? hint : NULL;
}

int db_get_max_level(sqlite3 *db) {
    sqlite3_stmt *stmt = db_prepare(db,
        "SELECT MAX(level) FROM level_data"
    );

    if (!stmt) return -1;

    int max_level = -1;

    if (db_step(stmt) == SQLITE_ROW) {
        max_level = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return max_level;
}