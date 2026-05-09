#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include "db.h"

int main() {

    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);

    if (!pw || !pw->pw_name) return 1;

    const char *user = pw->pw_name;

    sqlite3 *db = db_open();
    if (!db) return 1;

    int level = db_get_level(db, user);

    db_close(db);

    if (level < 0) return 1;

    printf("%d\n", level);
    return 0;
}