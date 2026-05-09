#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <unistd.h>
#include "db.h"
#include<sys/types.h>
#include<pwd.h>

#define MAXP 256

// 🔹 SHA256 helper
void sha256_hex(const char *in, char *out) {
    unsigned char h[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)in, strlen(in), h);
    for (int i = 0; i < 32; i++)
        sprintf(out + i*2, "%02x", h[i]);
    out[64] = 0;
}

const char* get_current_user() {
    uid_t uid = getuid();                 // real user
    struct passwd *pw = getpwuid(uid);
    if (!pw) return NULL;
    return pw->pw_name;
}

int main(int argc, char **argv) {

    if (argc != 2) return 1;
    const char *user = argv[1];

    sqlite3 *db = db_open();
    if (!db) return 1;
    
    // Validate Loggedin User
    const char *real_user = get_current_user();
    if (!real_user || strcmp(user, real_user) != 0) {
        fprintf(stderr, "Unauthorized access\n");
        db_close(db);
        return 1;
    }

    char pass[MAXP];
    char inhash[65];

    // read input
    if (!fgets(pass, sizeof(pass), stdin)) return 1;
    pass[strcspn(pass, "\n")] = 0;

    sha256_hex(pass, inhash);

    // fetch player state
    sqlite3_stmt *stmt = db_prepare(db,
        "SELECT level, score, gaveup, completed FROM progress WHERE username=?"
    );

    int level = 0, score = 0, gaveup = 0, completed = 0;

    if (stmt) {
        sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);

        if (db_step(stmt) == SQLITE_ROW) {
            level     = sqlite3_column_int(stmt, 0);
            score     = sqlite3_column_int(stmt, 1);
            gaveup    = sqlite3_column_int(stmt, 2);
            completed = sqlite3_column_int(stmt, 3);
        }

        sqlite3_finalize(stmt);
    }

    // already completed
    if (completed == 1) {
        db_close(db);
        return 0;
    }

    // get correct hash
    const char *realhash = db_get_level_hash(db, level);
    if (!realhash) {
        db_close(db);
        return 1;
    }

    // compare hashes
    if (strcmp(inhash, realhash) == 0) {
        // scoring logic (unchanged behavior)
        if (gaveup == 0)
            score += ((level+1)*5);

        int new_level = level + 1;

        // dynamic max level
        int max_level = db_get_max_level(db);
        if (max_level < 0) {
            db_close(db);
            return 1;
        }

        int completed_flag = (new_level > max_level) ? 1 : 0;

        // update progress
        db_update_progress(db, user, new_level, score, completed_flag);

        // log submission
        db_insert_submission(db, user, level, gaveup, completed);

        db_close(db);
        return 0;   // SUCCESS
    }

    db_close(db);
    return 2;   // WRONG KEY
}