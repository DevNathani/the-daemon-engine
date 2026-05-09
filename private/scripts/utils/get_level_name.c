#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "db.h"

int main() {
    char *user = getlogin();

    if (!user) {
        user = getenv("USER"); // fallback
        if (!user) return 1;
    }

    sqlite3 *db = db_open();
    if (!db) return 1;

    int level = db_get_level(db, user);
    const char *name = db_get_level_name(db, level);

    if (name)
        printf("%s\n", name);

    db_close(db);
    return 0;
}