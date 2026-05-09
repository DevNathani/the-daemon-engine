#ifndef DB_H
#define DB_H

#include <sqlite3.h>

// lifecycle
sqlite3* db_open();
void db_close(sqlite3 *db);

// core helpers
sqlite3_stmt* db_prepare(sqlite3 *db, const char *query);
int db_step(sqlite3_stmt *stmt);

// core game APIs
int db_get_level(sqlite3 *db, const char *user);
const char* db_get_level_name(sqlite3 *db, int level);
const char* db_get_level_hash(sqlite3 *db, int level);

int db_update_progress(sqlite3 *db, const char *user, int level, int score, int completed);
int db_set_giveup(sqlite3 *db, const char *user);

int db_insert_submission(sqlite3 *db, const char *user, int level, int gaveup, int completed);

// level content
const char* db_get_level_message(sqlite3 *db, int level);
const char* db_get_final_hint(sqlite3 *db, int level);

int db_get_max_level(sqlite3 *db);

#endif